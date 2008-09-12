/*
 * (C) Copyright 2008
 * Texas Instruments, <www.ti.com>
 *
 * Author :
 *      Manikandan Pillai <mani.pillai@ti.com>
 *
 * Derived from Beagle Board and 3430 SDP code by
 *      Richard Woodruff <r-woodruff2@ti.com>
 *      Syed Mohammed Khasim <khasim@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
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
#include <asm/arch/mem.h>	/* get mem tables */
#include <asm/arch/sys_proto.h>
#include <i2c.h>

extern omap3_sysinfo sysinfo;

/**************************************************************************
 * get_gpmc0_type()
 ***************************************************************************/
u32 get_gpmc0_type(void)
{
#if defined(CFG_ENV_IS_IN_ONENAND)
	return 1; /* OneNAND */
#else
	return 2; /* NAND */
#endif
}

/****************************************************
 * get_cpu_type() - low level get cpu type
 * - no C globals yet.
 ****************************************************/
u32 get_cpu_type(void)
{
	/* fixme, need to get register defines for OMAP3 */
	return CPU_3430;
}

/******************************************
 * get_cpu_rev(void) - extract version info
 ******************************************/
u32 get_cpu_rev(void)
{
	u32 cpuid = 0;
	/* On ES1.0 the IDCODE register is not exposed on L4
	 * so using CPU ID to differentiate
	 * between ES2.0 and ES1.0.
	 */
	__asm__ __volatile__("mrc p15, 0, %0, c0, c0, 0":"=r"(cpuid));
	if ((cpuid & 0xf) == 0x0)
		return CPU_3430_ES1;
	else
		return CPU_3430_ES2;

}

/****************************************************
 * is_mem_sdr() - return 1 if mem type in use is SDR
 ****************************************************/
u32 is_mem_sdr(void)
{
	volatile u32 *burst = (volatile u32 *) (SDRC_MR_0 + SDRC_CS0_OSET);
	if (*burst == SDP_SDRC_MR_0_SDR)
		return 1;
	return 0;
}

/***********************************************************
 * get_mem_type() - identify type of mDDR part used.
 ***********************************************************/
u32 get_mem_type(void)
{
	/* Current SDP3430 uses 2x16 MDDR Infenion parts */
	return DDR_DISCRETE;
}

/***********************************************************************
 * get_cs0_size() - get size of chip select 0/1
 ************************************************************************/
u32 get_sdr_cs_size(u32 offset)
{
	u32 size;

	/* get ram size field */
	size = __raw_readl(SDRC_MCFG_0 + offset) >> 8;
	size &= 0x3FF;		/* remove unwanted bits */
	size *= SZ_2M;		/* find size in MB */
	return size;
}

/***********************************************************************
 * get_board_type() - get board type based on current production stats.
 *  - NOTE-1-: 2 I2C EEPROMs will someday be populated with proper info.
 *    when they are available we can get info from there.  This should
 *    be correct of all known boards up until today.
 *  - NOTE-2- EEPROMs are populated but they are updated very slowly.  To
 *    avoid waiting on them we will use ES version of the chip to get info.
 *    A later version of the FPGA migth solve their speed issue.
 ************************************************************************/
u32 get_board_type(void)
{
	if (get_cpu_rev() == CPU_3430_ES2)
		return sysinfo.board_type_v2;
	else
		return sysinfo.board_type_v1;
}

/******************************************************************
 * get_sysboot_value() - get init word settings
 ******************************************************************/
inline u32 get_sysboot_value(void)
{
	return 0x0000003F & __raw_readl(CONTROL_STATUS);
}

/***************************************************************************
 *  get_gpmc0_base() - Return current address hardware will be
 *     fetching from. The below effectively gives what is correct, its a bit
 *   mis-leading compared to the TRM.  For the most general case the mask
 *   needs to be also taken into account this does work in practice.
 *   - for u-boot we currently map:
 *       -- 0 to nothing,
 *       -- 4 to flash
 *       -- 8 to enent
 *       -- c to wifi
 ****************************************************************************/
u32 get_gpmc0_base(void)
{
	u32 b;

	b = __raw_readl(GPMC_CONFIG_CS0 + GPMC_CONFIG7);
	b &= 0x1F;		/* keep base [5:0] */
	b = b << 24;		/* ret 0x0b000000 */
	return b;
}

/*******************************************************************
 * get_gpmc0_width() - See if bus is in x8 or x16 (mainly for nand)
 *******************************************************************/
u32 get_gpmc0_width(void)
{
	return WIDTH_16BIT;
}

/*************************************************************************
 * get_board_rev() - setup to pass kernel board revision information
 * returns:(bit[0-3] sub version, higher bit[7-4] is higher version)
 *************************************************************************/
u32 get_board_rev(void)
{
	return 0x20;
}

/*********************************************************************
 *  display_board_info() - print banner with board info.
 *********************************************************************/
void display_board_info(u32 btype)
{
	char *bootmode[] = {
		"NOR",
		"ONENAND",
		"NAND",
		"P2a",
		"NOR",
		"NOR",
		"P2a",
		"P2b",
	};
	u32 brev = get_board_rev();
	char db_ver[] = "0.0";	/* board type */
	char mem_sdr[] = "mSDR";	/* memory type */
	char mem_ddr[] = "LPDDR";
	char t_tst[] = "TST";	/* security level */
	char t_emu[] = "EMU";
	char t_hs[] = "HS";
	char t_gp[] = "GP";
	char unk[] = "?";
#ifdef CONFIG_LED_INFO
	char led_string[CONFIG_LED_LEN] = { 0 };
#endif
	char p_l3[] = "165";
	char p_cpu[] = "2";

	char *db_s, *mem_s, *sec_s;
	u32 cpu, rev, sec;

	rev = get_cpu_rev();
	cpu = get_cpu_type();
	sec = get_device_type();

	if (is_mem_sdr())
		mem_s = mem_sdr;
	else
		mem_s = mem_ddr;

	db_s = db_ver;
	db_s[0] += (brev >> 4) & 0xF;
	db_s[2] += brev & 0xF;

	switch (sec) {
	case TST_DEVICE:
		sec_s = t_tst;
		break;
	case EMU_DEVICE:
		sec_s = t_emu;
		break;
	case HS_DEVICE:
		sec_s = t_hs;
		break;
	case GP_DEVICE:
		sec_s = t_gp;
		break;
	default:
		sec_s = unk;
	}

	printf("OMAP%s-%s rev %d, CPU-OPP%s L3-%sMHz\n", sysinfo.cpu_string,
	       sec_s, rev, p_cpu, p_l3);
	printf("%s + %s/%s\n", sysinfo.board_string,
	       mem_s, bootmode[get_gpmc0_type()]);

}

/********************************************************
 *  get_base(); get upper addr of current execution
 *******************************************************/
u32 get_base(void)
{
	u32 val;

	__asm__ __volatile__("mov %0, pc \n":"=r"(val)::"memory");
	val &= 0xF0000000;
	val >>= 28;
	return val;
}

/********************************************************
 *  running_in_flash() - tell if currently running in
 *   flash.
 *******************************************************/
u32 running_in_flash(void)
{
	if (get_base() < 4)
		return 1;	/* in flash */

	return 0;		/* running in SRAM or SDRAM */
}

/********************************************************
 *  running_in_sram() - tell if currently running in
 *   sram.
 *******************************************************/
u32 running_in_sram(void)
{
	if (get_base() == 4)
		return 1;	/* in SRAM */

	return 0;		/* running in FLASH or SDRAM */
}

/********************************************************
 *  running_in_sdram() - tell if currently running in
 *   flash.
 *******************************************************/
u32 running_in_sdram(void)
{
	if (get_base() > 4)
		return 1;	/* in sdram */

	return 0;		/* running in SRAM or FLASH */
}

/***************************************************************
 *  get_boot_type() - Is this an XIP type device or a stream one
 *   bits 4-0 specify type.  Bit 5 sys mem/perif
 ***************************************************************/
u32 get_boot_type(void)
{
	u32 v;

	v = get_sysboot_value() & (BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
	return v;
}

/*************************************************************
 *  get_device_type(): tell if GP/HS/EMU/TST
 *************************************************************/
u32 get_device_type(void)
{
	int mode;

	mode = __raw_readl(CONTROL_STATUS) & (DEVICE_MASK);
	return mode >>= 8;
}
