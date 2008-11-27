/*
 * Maintainer : Steve Sakoman <steve@sakoman.com>
 *
 * Derived from Beagle Board, 3430 SDP, and OMAP3EVM code by
 *	Richard Woodruff <r-woodruff2@ti.com>
 *	Syed Mohammed Khasim <khasim@ti.com>
 *	Sunil Kumar <sunilsaini05@gmail.com>
 *	Shashi Ranjan <shashiranjanmca05@gmail.com>
 *
 * (C) Copyright 2004-2008
 * Texas Instruments, <www.ti.com>
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
#include <asm/arch/mux.h>
#include <asm/arch/sys_proto.h>
#include <i2c.h>
#include <asm/mach-types.h>
#include "overo.h"

/******************************************************************************
 * Routine: board_init
 * Description: Early hardware init.
 *****************************************************************************/
int board_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	gpmc_init(); /* in SRAM or SDRAM, finish GPMC */
	/* board id for Linux */
	gd->bd->bi_arch_number = MACH_TYPE_OVERO;
	/* boot param addr */
	gd->bd->bi_boot_params = (OMAP34XX_SDRC_CS0 + 0x100);

	return 0;
}

/******************************************************************************
 * Routine: misc_init_r
 * Description: Configure power supply
 *****************************************************************************/
int misc_init_r(void)
{

	unsigned char byte;
	unsigned int *gpio5_base = (unsigned int *)OMAP34XX_GPIO5_BASE;
	unsigned int *gpio6_base = (unsigned int *)OMAP34XX_GPIO6_BASE;

#ifdef CONFIG_DRIVER_OMAP34XX_I2C
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
#endif

	/*
	 * Configure OMAP3 supply voltages in power management
	 * companion chip.
	 */

	/* set VAUX2 to 2.8V */
	byte = DEV_GRP_P1;
	i2c_write(PWRMGT_ADDR_ID4, VAUX2_DEV_GRP, 1, &byte, 1);
	byte = VAUX2_VSEL_28;
	i2c_write(PWRMGT_ADDR_ID4, VAUX2_DEDICATED, 1, &byte, 1);

	/* set VAUX3 to 2.8V */
	byte = DEV_GRP_P1;
	i2c_write(PWRMGT_ADDR_ID4, VAUX3_DEV_GRP, 1, &byte, 1);
	byte = VAUX3_VSEL_28;
	i2c_write(PWRMGT_ADDR_ID4, VAUX3_DEDICATED, 1, &byte, 1);

	/* set VPLL2 to 1.8V */
	byte = DEV_GRP_ALL;
	i2c_write(PWRMGT_ADDR_ID4, VPLL2_DEV_GRP, 1, &byte, 1);
	byte = VPLL2_VSEL_18;
	i2c_write(PWRMGT_ADDR_ID4, VPLL2_DEDICATED, 1, &byte, 1);

	/* set VDAC to 1.8V */
	byte = DEV_GRP_P1;
	i2c_write(PWRMGT_ADDR_ID4, VDAC_DEV_GRP, 1, &byte, 1);
	byte = VDAC_VSEL_18;
	i2c_write(PWRMGT_ADDR_ID4, VDAC_DEDICATED, 1, &byte, 1);

	/* enable LED */
	byte = LEDBPWM | LEDAPWM | LEDBON | LEDAON;
	i2c_write(PWRMGT_ADDR_ID3, LEDEN, 1, &byte, 1);

	/* Configure GPIOs to output */
	writel(~((GPIO10) | GPIO9 | GPIO3 | GPIO2),
		gpio6_base + OFFS(GPIO_OE));
	writel(~(GPIO31 | GPIO30 | GPIO29 | GPIO28 | GPIO22 | GPIO21 |
		GPIO15 | GPIO14 | GPIO13 | GPIO12),
		gpio5_base + OFFS(GPIO_OE));

	/* Set GPIOs */
	writel(GPIO10 | GPIO9 | GPIO3 | GPIO2,
		gpio6_base + OFFS(GPIO_SETDATAOUT));
	writel(GPIO31 | GPIO30 | GPIO29 | GPIO28 | GPIO22 | GPIO21 |
		GPIO15 | GPIO14 | GPIO13 | GPIO12,
		gpio5_base + OFFS(GPIO_SETDATAOUT));

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
	MUX_OVERO();
}

