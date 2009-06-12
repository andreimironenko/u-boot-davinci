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
#include <asm/arch/mem.h>
#include <asm/arch/sys_proto.h>
#include <command.h>

/* Definitions for EMIF4 configuration values */
#define EMIF4_TIM1_T_RP         0x3
#define EMIF4_TIM1_T_RCD        0x3
#define EMIF4_TIM1_T_WR         0x3
#define EMIF4_TIM1_T_RAS        0x8
#define EMIF4_TIM1_T_RC         0xA
#define EMIF4_TIM1_T_RRD        0x2
#define EMIF4_TIM1_T_WTR        0x2

#define EMIF4_TIM2_T_XP         0x2
#define EMIF4_TIM2_T_ODT        0x0
#define EMIF4_TIM2_T_XSNR       0x1C
#define EMIF4_TIM2_T_XSRD       0xC8
#define EMIF4_TIM2_T_RTP        0x1
#define EMIF4_TIM2_T_CKE        0x2

#define EMIF4_TIM3_T_TDQSCKMAX  0x0
#define EMIF4_TIM3_T_RFC        0x25
#define EMIF4_TIM3_T_RAS_MAX    0x7

#define EMIF4_PWR_IDLE          0x2
#define EMIF4_PWR_DPD_EN        0x0
#define EMIF4_PWR_PM_EN         0x0
#define EMIF4_PWR_PM_TIM        0x0

#define EMIF4_INITREF_DIS       0x0
#define EMIF4_PASR              0x0
#define EMIF4_REFRESH_RATE      0x50F

#define EMIF4_CFG_SDRAM_TYP     0x2
#define EMIF4_CFG_IBANK_POS     0x0
#define EMIF4_CFG_DDR_TERM      0x0
#define EMIF4_CFG_DDR2_DDQS     0x1
#define EMIF4_CFG_DYN_ODT       0x0
#define EMIF4_CFG_DDR_DIS_DLL   0x0
#define EMIF4_CFG_SDR_DRV       0x0
#define EMIF4_CFG_CWL           0x0
#define EMIF4_CFG_NARROW_MD     0x0
#define EMIF4_CFG_CL            0x5
#define EMIF4_CFG_ROWSIZE       0x0
#define EMIF4_CFG_IBANK         0x3
#define EMIF4_CFG_EBANK         0x0
#define EMIF4_CFG_PGSIZE        0x2

/*
 * EMIF4 PHY Control 1 register configuration
 */
#define EMIF4_DDR1_READ_LAT	0x6
#define EMIF4_DDR1_PWRDN_DIS    0x1
#define EMIF4_DDR1_STRBEN_EXT   0x1
#define EMIF4_DDR1_DLL_MODE     0x0
#define EMIF4_DDR1_VTP_DYN	0x1
#define EMIF4_DDR1_LB_CK_SEL    0x0

/*
 * Only One NAND allowed on board at a time.
 * The GPMC CS Base for the same
 */
unsigned int boot_flash_base;
unsigned int boot_flash_off;
unsigned int boot_flash_sec;
unsigned int boot_flash_type;
volatile unsigned int boot_flash_env_addr;

#if defined(CONFIG_CMD_NAND)
static u32 gpmc_m_nand[GPMC_MAX_REG] = {
	M_NAND_GPMC_CONFIG1,
	M_NAND_GPMC_CONFIG2,
	M_NAND_GPMC_CONFIG3,
	M_NAND_GPMC_CONFIG4,
	M_NAND_GPMC_CONFIG5,
	M_NAND_GPMC_CONFIG6, 0
};

gpmc_csx_t *nand_cs_base;
gpmc_t *gpmc_cfg_base;

#if !defined (CONFIG_OMAP3_OMAP3517TEB)
#if defined(CONFIG_ENV_IS_IN_NAND)
#define GPMC_CS 0
#else
#define GPMC_CS 1
#endif
#else
#define GPMC_CS 2
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

gpmc_csx_t *onenand_cs_base;

#if defined(CONFIG_ENV_IS_IN_ONENAND)
#define GPMC_CS 0
#else
#define GPMC_CS 1
#endif

#endif

static sdrc_t *sdrc_base = (sdrc_t *)OMAP34XX_SDRC_BASE;
static emif4_t *emif4_base = (emif4_t *)OMAP34XX_SDRC_BASE;

/**************************************************************************
 * make_cs1_contiguous() - for es2 and above remap cs1 behind cs0 to allow
 *  command line mem=xyz use all memory with out discontinuous support
 *  compiled in.  Could do it at the ATAG, but there really is two banks...
 * Called as part of 2nd phase DDR init.
 **************************************************************************/
void make_cs1_contiguous(void)
{
	u32 size, a_add_low, a_add_high;

	size = get_sdr_cs_size(CS0);
	size /= SZ_32M;			/* find size to offset CS1 */
	a_add_high = (size & 3) << 8;	/* set up low field */
	a_add_low = (size & 0x3C) >> 2;	/* set up high field */
	writel((a_add_high | a_add_low), &sdrc_base->cs_cfg);

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
	do_sdrc_init(CS0, EARLY_INIT);
}

/*************************************************************************
 * do_sdrc_init(): initialize the SDRAM for use.
 *  -code sets up SDRAM basic SDRC timings for CS0
 *  -optimal settings can be placed here, or redone after i2c
 *      inspection of board info
 *
 *  - code called once in C-Stack only context for CS0 and a possible 2nd
 *      time depending on memory configuration from stack+global context
 **************************************************************************/

void do_sdrc_init(u32 cs, u32 early)
{
	sdrc_actim_t *sdrc_actim_base;

	if(cs)
		sdrc_actim_base = (sdrc_actim_t *)SDRC_ACTIM_CTRL1_BASE;
	else
		sdrc_actim_base = (sdrc_actim_t *)SDRC_ACTIM_CTRL0_BASE;

	if (early) {
		/* reset sdrc controller */
		writel(SOFTRESET, &sdrc_base->sysconfig);
		wait_on_value(RESETDONE, RESETDONE, &sdrc_base->status,
			      12000000);
		writel(0, &sdrc_base->sysconfig);

		/* setup sdrc to ball mux */
		writel(SDP_SDRC_SHARING, &sdrc_base->sharing);

		/* Disable Power Down of CKE cuz of 1 CKE on combo part */
		writel(SRFRONRESET | PAGEPOLICY_HIGH, &sdrc_base->power);

		writel(ENADLL | DLLPHASE_90, &sdrc_base->dlla_ctrl);
		sdelay(0x20000);
	}

	writel(RASWIDTH_13BITS | CASWIDTH_10BITS | ADDRMUXLEGACY |
		RAMSIZE_128 | BANKALLOCATION | B32NOT16 | B32NOT16 |
		DEEPPD | DDR_SDRAM, &sdrc_base->cs[cs].mcfg);
	writel(ARCV | ARE_ARCV_1, &sdrc_base->cs[cs].rfr_ctrl);
	writel(V_ACTIMA_165, &sdrc_actim_base->ctrla);
	writel(V_ACTIMB_165, &sdrc_actim_base->ctrlb);

	writel(CMD_NOP, &sdrc_base ->cs[cs].manual);
	writel(CMD_PRECHARGE, &sdrc_base->cs[cs].manual);
	writel(CMD_AUTOREFRESH, &sdrc_base->cs[cs].manual);
	writel(CMD_AUTOREFRESH, &sdrc_base->cs[cs].manual);

	/*
	 * CAS latency 3, Write Burst = Read Burst, Serial Mode,
	 * Burst length = 4
	 */
	writel(CASL3 | BURSTLENGTH4, &sdrc_base->cs[cs].mr);

	if (!mem_ok(cs))
		writel(0, &sdrc_base->cs[cs].mcfg);
}

/********************************************************
 * emif4 _init() - init the emif4 module for DDR access
 *  - early init routines, called from flash or
 *  SRAM.
 *******************************************************/
void emif4_init(void)
{
	unsigned int regval;
	/* Set the DDR PHY parameters in PHY ctrl registers */
	regval = (EMIF4_DDR1_READ_LAT | (EMIF4_DDR1_VTP_DYN << 15) |
		(EMIF4_DDR1_STRBEN_EXT << 7) | (EMIF4_DDR1_DLL_MODE << 12) |
		(EMIF4_DDR1_VTP_DYN << 15) | (EMIF4_DDR1_LB_CK_SEL << 23));
	writel(regval, &emif4_base->ddr_phyctrl1);
	writel(regval, &emif4_base->ddr_phyctrl1_shdw);
	writel(0, &emif4_base->ddr_phyctrl2);

	/* Reset the DDR PHY and wait till completed */
	regval = readl(&emif4_base->sdram_iodft_tlgc);
	regval |= (1<<10);
	writel(regval, &emif4_base->sdram_iodft_tlgc);
	/*Wait till that bit clears*/
	while ((readl(&emif4_base->sdram_iodft_tlgc) & (1<<10)) == 0x1);
	/*Re-verify the DDR PHY status*/
	while ((readl(&emif4_base->sdram_sts) & (1<<2)) == 0x0);

	regval |= (1<<0);
	writel(regval, &emif4_base->sdram_iodft_tlgc);
	/* Set SDR timing registers */
	regval = (EMIF4_TIM1_T_WTR | (EMIF4_TIM1_T_RRD << 3) |
		(EMIF4_TIM1_T_RC << 6) | (EMIF4_TIM1_T_RAS << 12) |
		(EMIF4_TIM1_T_WR << 17) | (EMIF4_TIM1_T_RCD << 21) |
		(EMIF4_TIM1_T_RP << 25));
	writel(regval, &emif4_base->sdram_time1);
	writel(regval, &emif4_base->sdram_time1_shdw);

	regval = (EMIF4_TIM2_T_CKE | (EMIF4_TIM2_T_RTP << 3) |
		(EMIF4_TIM2_T_XSRD << 6) | (EMIF4_TIM2_T_XSNR << 16) |
		(EMIF4_TIM2_T_ODT << 25) | (EMIF4_TIM2_T_XP << 28));
	writel(regval, &emif4_base->sdram_time2);
	writel(regval, &emif4_base->sdram_time2_shdw);

	regval = (EMIF4_TIM3_T_RAS_MAX | (EMIF4_TIM3_T_RFC << 4) |
		(EMIF4_TIM3_T_TDQSCKMAX << 13));
	writel(regval, &emif4_base->sdram_time3);
	writel(regval, &emif4_base->sdram_time3_shdw);

	/* Set the PWR control register */
	regval = (EMIF4_PWR_PM_TIM | (EMIF4_PWR_PM_EN << 8) |
		(EMIF4_PWR_DPD_EN << 10) | (EMIF4_PWR_IDLE << 30));
	writel(regval, &emif4_base->sdram_pwr_mgmt);
	writel(regval, &emif4_base->sdram_pwr_mgmt_shdw);

	/* Set the DDR refresh rate control register */
	regval = (EMIF4_REFRESH_RATE | (EMIF4_PASR << 24) |
		(EMIF4_INITREF_DIS << 31));
	writel(regval, &emif4_base->sdram_refresh_ctrl);
	writel(regval, &emif4_base->sdram_refresh_ctrl_shdw);

	/* set the SDRAM configuration register */
	regval = (EMIF4_CFG_PGSIZE | (EMIF4_CFG_EBANK << 3) |
		(EMIF4_CFG_IBANK << 4) | (EMIF4_CFG_ROWSIZE << 7) |
		(EMIF4_CFG_CL << 10) | (EMIF4_CFG_NARROW_MD << 14) |
		(EMIF4_CFG_CWL << 16) | (EMIF4_CFG_SDR_DRV << 18) |
		(EMIF4_CFG_DDR_DIS_DLL << 20) | (EMIF4_CFG_DYN_ODT << 21) |
		(EMIF4_CFG_DDR2_DDQS << 23) | (EMIF4_CFG_DDR_TERM << 24) |
		(EMIF4_CFG_IBANK_POS << 27) | (EMIF4_CFG_SDRAM_TYP << 29));
	writel(regval, &emif4_base->sdram_config);
}

void enable_gpmc_config(u32 *gpmc_config, gpmc_csx_t *gpmc_cs_base, u32 base,
			u32 size)
{
	writel(0, &gpmc_cs_base->config7);
	sdelay(1000);
	/* Delay for settling */
	writel(gpmc_config[0], &gpmc_cs_base->config1);
	writel(gpmc_config[1], &gpmc_cs_base->config2);
	writel(gpmc_config[2], &gpmc_cs_base->config3);
	writel(gpmc_config[3], &gpmc_cs_base->config4);
	writel(gpmc_config[4], &gpmc_cs_base->config5);
	writel(gpmc_config[5], &gpmc_cs_base->config6);
	/* Enable the config */
	writel((((size & 0xF) << 8) | ((base >> 24) & 0x3F) |
		(1 << 6)), &gpmc_cs_base->config7);
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
	gpmc_t *gpmc_base = (gpmc_t *)GPMC_BASE;
	gpmc_csx_t *gpmc_cs_base = (gpmc_csx_t *)GPMC_CONFIG_CS0_BASE;
	u32 base = 0;
	u32 size = 0;
	u32 f_off = CONFIG_SYS_MONITOR_LEN;
	u32 f_sec = 0;
	u32 config = 0;

	/* global settings */
	writel(0, &gpmc_base->irqenable); /* isr's sources masked */
	writel(0, &gpmc_base->timeout_control);/* timeout disable */

	config = readl(&gpmc_base->config);
	config &= (~0xf00);
	writel(config, &gpmc_base->config);

	/*
	 * Disable the GPMC0 config set by ROM code
	 * It conflicts with our MPDB (both at 0x08000000)
	 */
	writel(0, &gpmc_cs_base->config7);
	sdelay(1000);

#if defined(CONFIG_CMD_NAND)	/* CS 0 */
	gpmc_config = gpmc_m_nand;
	gpmc_cfg_base = gpmc_base;
	nand_cs_base = (gpmc_csx_t *)(GPMC_CONFIG_CS0_BASE +
					(GPMC_CS * GPMC_CONFIG_WIDTH));
	base = PISMO1_NAND_BASE;
	size = PISMO1_NAND_SIZE;
	enable_gpmc_config(gpmc_config, nand_cs_base, base, size);
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
	onenand_cs_base = (gpmc_csx_t *)(GPMC_CONFIG_CS0_BASE +
					(GPMC_CS * GPMC_CONFIG_WIDTH));
	base = PISMO1_ONEN_BASE;
	size = PISMO1_ONEN_SIZE;
	enable_gpmc_config(gpmc_config, onenand_cs_base, base, size);
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
}
