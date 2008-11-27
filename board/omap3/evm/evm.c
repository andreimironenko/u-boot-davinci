/*
 * (C) Copyright 2004-2008
 * Texas Instruments, <www.ti.com>
 *
 * Author :
 *	Manikandan Pillai <mani.pillai@ti.com>
 *
 * Derived from Beagle Board and 3430 SDP code by
 *	Richard Woodruff <r-woodruff2@ti.com>
 *	Syed Mohammed Khasim <khasim@ti.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
#include <asm/arch/mux.h>
#include <asm/arch/sys_proto.h>
#include <i2c.h>
#include <asm/mach-types.h>
#include "evm.h"

/******************************************************************************
 * Routine: board_init
 * Description: Early hardware init.
 *****************************************************************************/
int board_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	gpmc_init(); /* in SRAM or SDRAM, finish GPMC */
	/* board id for Linux */
	gd->bd->bi_arch_number = MACH_TYPE_OMAP3EVM;
	/* boot param addr */
	gd->bd->bi_boot_params = (OMAP34XX_SDRC_CS0 + 0x100);

	return 0;
}

/******************************************************************************
 * Routine: misc_init_r
 * Description: Init ethernet (done here so udelay works)
 *****************************************************************************/
int misc_init_r(void)
{

#ifdef CONFIG_DRIVER_OMAP34XX_I2C
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
#endif

#if defined(CONFIG_CMD_NET)
	setup_net_chip();
#endif

	return 0;
}

/******************************************************************************
 * Routine: set_muxconf_regs
 * Description: Setting up the configuration Mux registers specific to the
 *		hardware. Many pins need to be moved from protect to primary
 *		mode.
 *****************************************************************************/
void set_muxconf_regs(void)
{
	MUX_EVM();
}

/******************************************************************************
 * Routine: setup_net_chip
 * Description: Setting up the configuration GPMC registers specific to the
 *		Ethernet hardware.
 *****************************************************************************/
static void setup_net_chip(void)
{
	unsigned int *gpio3_base = (unsigned int *)OMAP34XX_GPIO3_BASE;
	unsigned int *gpmc_cs6_base = (unsigned int *)GPMC_CONFIG_CS6_BASE;
	unsigned short *ctrl_base = (unsigned short *)OMAP34XX_CTRL_BASE;

	/* Configure GPMC registers */
	writel(NET_GPMC_CONFIG1, gpmc_cs6_base + OFFS(GPMC_CONFIG1));
	writel(NET_GPMC_CONFIG2, gpmc_cs6_base + OFFS(GPMC_CONFIG2));
	writel(NET_GPMC_CONFIG3, gpmc_cs6_base + OFFS(GPMC_CONFIG3));
	writel(NET_GPMC_CONFIG4, gpmc_cs6_base + OFFS(GPMC_CONFIG4));
	writel(NET_GPMC_CONFIG5, gpmc_cs6_base + OFFS(GPMC_CONFIG5));
	writel(NET_GPMC_CONFIG6, gpmc_cs6_base + OFFS(GPMC_CONFIG6));
	writel(NET_GPMC_CONFIG7, gpmc_cs6_base + OFFS(GPMC_CONFIG7));

	/* Enable off mode for NWE in PADCONF_GPMC_NWE register */
	writew(readw(ctrl_base + (CONTROL_PADCONF_GPMC_NWE >> 1)) | 0x0E00,
	       ctrl_base + (CONTROL_PADCONF_GPMC_NWE >> 1));
	/* Enable off mode for NOE in PADCONF_GPMC_NADV_ALE register */
	writew(readw(ctrl_base + (CONTROL_PADCONF_GPMC_NOE >> 1)) | 0x0E00,
		ctrl_base + (CONTROL_PADCONF_GPMC_NOE >> 1));
	/* Enable off mode for ALE in PADCONF_GPMC_NADV_ALE register */
	writew(readw(ctrl_base + (CONTROL_PADCONF_GPMC_NADV_ALE >> 1)) | 0x0E00,
		ctrl_base + (CONTROL_PADCONF_GPMC_NADV_ALE >> 1));

	/* Make GPIO 64 as output pin */
	writel(readl(gpio3_base + OFFS(GPIO_OE)) & ~(GPIO0),
		gpio3_base + OFFS(GPIO_OE));

	/* Now send a pulse on the GPIO pin */
	writel(GPIO0, gpio3_base + OFFS(GPIO_SETDATAOUT));
	udelay(1);
	writel(GPIO0, gpio3_base + OFFS(GPIO_CLEARDATAOUT));
	udelay(1);
	writel(GPIO0, gpio3_base + OFFS(GPIO_SETDATAOUT));
}

