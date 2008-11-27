/*
 * (C) Copyright 2008
 * Texas Instruments, <www.ti.com>
 *
 * Author :
 *     Manikandan Pillai <mani.pillai@ti.com>
 *
 * Initial Code from:
 *     Richard Woodruff <r-woodruff2@ti.com>
 *     Syed Mohammed Khasim <khasim@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/bits.h>
#include <asm/arch/mem.h>
#include <asm/arch/sys_proto.h>
#include <command.h>

/*
 * Only One NAND allowed on board at a time.
 * The GPMC CS Base for the same
 */
unsigned int boot_flash_base;
unsigned int boot_flash_off;
unsigned int boot_flash_sec;
unsigned int boot_flash_type;
volatile unsigned int boot_flash_env_addr;

/* help common/env_flash.c */
#ifdef ENV_IS_VARIABLE

uchar(*boot_env_get_char_spec) (int index);
int (*boot_env_init) (void);
int (*boot_saveenv) (void);
void (*boot_env_relocate_spec) (void);

/* 16 bit NAND */
uchar env_get_char_spec(int index);
int env_init(void);
int saveenv(void);
void env_relocate_spec(void);
extern char *env_name_spec;

#if defined(CONFIG_CMD_NAND)
u8 is_nand;
#endif

#if defined(CONFIG_CMD_ONENAND)
u8 is_onenand;
#endif

#endif /* ENV_IS_VARIABLE */

#if defined(CONFIG_CMD_NAND)
static u32 gpmc_m_nand[GPMC_MAX_REG] = {
	M_NAND_GPMC_CONFIG1,
	M_NAND_GPMC_CONFIG2,
	M_NAND_GPMC_CONFIG3,
	M_NAND_GPMC_CONFIG4,
	M_NAND_GPMC_CONFIG5,
	M_NAND_GPMC_CONFIG6, 0
};

u32 *nand_cs_base;

#if defined(CONFIG_ENV_IS_IN_NAND)
#define GPMC_CS 0
#else
#define GPMC_CS 1
#endif

#endif

#if defined(CONFIG_CMD_ONENAND)
static u32 gpmc_onenand[GPMC_MAX_REG] = {
	ONENAND_GPMC_CONFIG1,
	ONENAND_GPMC_CONFIG2,
	ONENAND_GPMC_CONFIG3,
	ONENAND_GPMC_CONFIG4,
	ONENAND_GPMC_CONFIG5,
	ONENAND_GPMC_CONFIG6, 0
};

u32 *onenand_cs_base;

#if defined(CONFIG_ENV_IS_IN_ONENAND)
#define GPMC_CS 0
#else
#define GPMC_CS 1
#endif

#endif

/**************************************************************************
 * make_cs1_contiguous() - for es2 and above remap cs1 behind cs0 to allow
 *  command line mem=xyz use all memory with out discontinuous support
 *  compiled in.  Could do it at the ATAG, but there really is two banks...
 * Called as part of 2nd phase DDR init.
 **************************************************************************/
void make_cs1_contiguous(void)
{
	u32 size, a_add_low, a_add_high;

	size = get_sdr_cs_size(SDRC_CS0_OSET);
	size /= SZ_32M;			/* find size to offset CS1 */
	a_add_high = (size & 3) << 8;	/* set up low field */
	a_add_low = (size & 0x3C) >> 2;	/* set up high field */
	writel((a_add_high | a_add_low), SDRC_CS_CFG);

}

/********************************************************
 *  mem_ok() - test used to see if timings are correct
 *             for a part. Helps in guessing which part
 *             we are currently using.
 *******************************************************/
u32 mem_ok(u32 cs)
{
	u32 val1, val2, addr;
	u32 pattern = 0x12345678;

	addr = OMAP34XX_SDRC_CS0 + get_sdr_cs_offset(cs);

	writel(0x0, addr + 0x400);	/* clear pos A */
	writel(pattern, addr);		/* pattern to pos B */
	writel(0x0, addr + 4);		/* remove pattern off the bus */
	val1 = readl(addr + 0x400);	/* get pos A value */
	val2 = readl(addr);		/* get val2 */

	if ((val1 != 0) || (val2 != pattern))	/* see if pos A val changed */
		return 0;
	else
		return 1;
}

/********************************************************
 *  sdrc_init() - init the sdrc chip selects CS0 and CS1
 *  - early init routines, called from flash or
 *  SRAM.
 *******************************************************/
void sdrc_init(void)
{
	/* only init up first bank here */
	do_sdrc_init(SDRC_CS0_OSET, EARLY_INIT);
}

/*************************************************************************
 * do_sdrc_init(): initialize the SDRAM for use.
 *  -code sets up SDRAM basic SDRC timings for CS0
 *  -optimal settings can be placed here, or redone after i2c
 *      inspection of board info
 *
 *  - code called ones in C-Stack only context for CS0 and a possible 2nd
 *      time depending on memory configuration from stack+global context
 **************************************************************************/

void do_sdrc_init(u32 offset, u32 early)
{
	u32 actim_offs = offset? 0x28: 0;

	if (early) {
		/* reset sdrc controller */
		writel(SOFTRESET, SDRC_SYSCONFIG);
		wait_on_value(BIT0, BIT0, SDRC_STATUS, 12000000);
		writel(0, SDRC_SYSCONFIG);

		/* setup sdrc to ball mux */
		writel(SDP_SDRC_SHARING, SDRC_SHARING);

		/* Disble Power Down of CKE cuz of 1 CKE on combo part */
		writel(0x00000081, SDRC_POWER);

		writel(0x0000A, SDRC_DLLA_CTRL);
		sdelay(0x20000);
	}

	writel(0x02584099,	SDRC_MCFG_0 + offset);
	writel(0x4e201,		SDRC_RFR_CTRL + offset);
	writel(0xaa9db4c6,	SDRC_ACTIM_CTRLA_0 + actim_offs);
	writel(0x11517,		SDRC_ACTIM_CTRLB_0 + actim_offs);

	writel(CMD_NOP,		SDRC_MANUAL_0 + offset);
	writel(CMD_PRECHARGE,	SDRC_MANUAL_0 + offset);
	writel(CMD_AUTOREFRESH,	SDRC_MANUAL_0 + offset);
	writel(CMD_AUTOREFRESH,	SDRC_MANUAL_0 + offset);

	/*  CAS latency 3, Write Burst = Read Burst, Serial Mode,
	    Burst length = 4 */
	writel(0x00000032,	SDRC_MR_0 + offset);

	if (!mem_ok(offset))
		writel(0, SDRC_MCFG_0 + offset);
}

void enable_gpmc_config(u32 *gpmc_config, u32 *gpmc_cs_base, u32 base,
			u32 size)
{
	writel(0, gpmc_cs_base + OFFS(GPMC_CONFIG7));
	sdelay(1000);
	/* Delay for settling */
	writel(gpmc_config[0], gpmc_cs_base + OFFS(GPMC_CONFIG1));
	writel(gpmc_config[1], gpmc_cs_base + OFFS(GPMC_CONFIG2));
	writel(gpmc_config[2], gpmc_cs_base + OFFS(GPMC_CONFIG3));
	writel(gpmc_config[3], gpmc_cs_base + OFFS(GPMC_CONFIG4));
	writel(gpmc_config[4], gpmc_cs_base + OFFS(GPMC_CONFIG5));
	writel(gpmc_config[5], gpmc_cs_base + OFFS(GPMC_CONFIG6));
	/* Enable the config */
	writel((((size & 0xF) << 8) | ((base >> 24) & 0x3F) |
		(1 << 6)), gpmc_cs_base + OFFS(GPMC_CONFIG7));
	sdelay(2000);
}

/*****************************************************
 * gpmc_init(): init gpmc bus
 * Init GPMC for x16, MuxMode (SDRAM in x32).
 * This code can only be executed from SRAM or SDRAM.
 *****************************************************/
void gpmc_init(void)
{
	/* putting a blanket check on GPMC based on ZeBu for now */
	u32 *gpmc_config = NULL;
	u32 *gpmc_base = (u32 *)GPMC_BASE;
	u32 *gpmc_cs_base = (u32 *)GPMC_CONFIG_CS0_BASE;
	u32 base = 0;
	u32 size = 0;
	u32 f_off = CONFIG_SYS_MONITOR_LEN;
	u32 f_sec = 0;
	u32 config = 0;

	/* global settings */
	writel(0, gpmc_base + OFFS(GPMC_IRQENABLE)); /* isr's sources masked */
	writel(0, gpmc_base + OFFS(GPMC_TIMEOUT_CONTROL));/* timeout disable */

	config = readl(gpmc_base + OFFS(GPMC_CONFIG));
	config &= (~0xf00);
	writel(config, gpmc_base + OFFS(GPMC_CONFIG));

	/*
	 * Disable the GPMC0 config set by ROM code
	 * It conflicts with our MPDB (both at 0x08000000)
	 */
	writel(0, gpmc_cs_base + OFFS(GPMC_CONFIG7));
	sdelay(1000);

#if defined(CONFIG_CMD_NAND)	/* CS 0 */
	gpmc_config = gpmc_m_nand;
	nand_cs_base = (u32 *)(GPMC_CONFIG_CS0_BASE +
			       (GPMC_CS * GPMC_CONFIG_WIDTH));
	base = PISMO1_NAND_BASE;
	size = PISMO1_NAND_SIZE;
	enable_gpmc_config(gpmc_config, nand_cs_base, base, size);
	is_nand = 1;
#if defined(CONFIG_ENV_IS_IN_NAND)
	f_off = SMNAND_ENV_OFFSET;
	f_sec = SZ_128K;
	/* env setup */
	boot_flash_base = base;
	boot_flash_off = f_off;
	boot_flash_sec = f_sec;
	boot_flash_env_addr = f_off;
#endif
#endif

#if defined(CONFIG_CMD_ONENAND)
	gpmc_config = gpmc_onenand;
	onenand_cs_base = (u32 *)(GPMC_CONFIG_CS0_BASE +
				  (GPMC_CS * GPMC_CONFIG_WIDTH));
	base = PISMO1_ONEN_BASE;
	size = PISMO1_ONEN_SIZE;
	enable_gpmc_config(gpmc_config, onenand_cs_base, base, size);
	is_onenand = 1;
#if defined(CONFIG_ENV_IS_IN_ONENAND)
	f_off = ONENAND_ENV_OFFSET;
	f_sec = SZ_128K;
	/* env setup */
	boot_flash_base = base;
	boot_flash_off = f_off;
	boot_flash_sec = f_sec;
	boot_flash_env_addr = f_off;
#endif
#endif

#ifdef ENV_IS_VARIABLE
	boot_env_get_char_spec = env_get_char_spec;
	boot_env_init = env_init;
	boot_saveenv = saveenv;
	boot_env_relocate_spec = env_relocate_spec;
#endif
}
