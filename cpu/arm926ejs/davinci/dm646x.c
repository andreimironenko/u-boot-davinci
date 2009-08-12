/*
 * davinci_dm6467_evm.h
 *  SoC-specific code for TMS320DM6467
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <asm/arch/hardware.h>

void davinci_enable_uart0(void)
{
	lpsc_on(DAVINCI_LPSC_UART0);

}

#ifdef CONFIG_DRIVER_TI_EMAC
void davinci_enable_emac(void)
{
	lpsc_on(DAVINCI_LPSC_EMAC);
	lpsc_on(DAVINCI_LPSC_EMAC_WRAPPER);
	lpsc_on(DAVINCI_LPSC_MDIO);

	/* Enable GIO3.3V cells used for EMAC */
	REG(VDD3P3V_PWDN) = 0;
}
#endif

#ifdef CONFIG_DRIVER_DAVINCI_I2C
void davinci_enable_i2c(void)
{
	lpsc_on(DAVINCI_LPSC_I2C);
}
#endif

