/*
 *
 * Copyright (C) 2009 Texas Instruments.
 *
 * ----------------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ----------------------------------------------------------------------------
 *
 */
#include <common.h>
#include <i2c.h>
#include <asm/arch/hardware.h>
#include <asm/arch/i2c_defs.h>

extern int timer_init(void);
extern int eth_hw_init(void);

/*******************************************
 Routine: board_init
 Description:  Board Initialization routine
*******************************************/
int board_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	
	/* arch number of DaVinci DM365 */
	gd->bd->bi_arch_number = 1939;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = LINUX_BOOT_PARAM_ADDR;
        
	/* Set the Bus Priority Register to appropriate value */
	REG(VBPR) = 0x20;
	
	timer_init();
	return 0;
}


/******************************
 Routine: misc_init_r
 Description:  Misc. init
******************************/
int misc_init_r (void)
{
	int i;
	u_int8_t	tmp[20], buf[10];


	REG(0x01c4000c) = 0x1affff;
		
	REG(0x01c67010) = 0x0;

	for (i = 0; i < 20; i++)
	{
		REG(0x01c67014) &= 0xffefffff;
		udelay(1000);
		REG(0x01c67014) |= 0x00100000;
	}
	REG(0x01c4000c) |= 0x01400000;

	/* Set Ethernet MAC address from EEPROM */
	if (i2c_read(CONFIG_SYS_I2C_EEPROM_ADDR, 0x7f00, CONFIG_SYS_I2C_EEPROM_ADDR_LEN, buf, 6)) {
		printf("\nEEPROM @ 0x%02x read FAILED!!!\n", CONFIG_SYS_I2C_EEPROM_ADDR);
	} else {
		tmp[0] = 0xff;
		for (i = 0; i < 6; i++)
			tmp[0] &= buf[i];
		
		if ((tmp[0] != 0xff) && (getenv("ethaddr") == NULL)) {
			sprintf((char *)&tmp[0], "%02x:%02x:%02x:%02x:%02x:%02x",
				buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
			setenv("ethaddr", (char *)&tmp[0]);
		}
	}

	
	if (!eth_hw_init())
		printf("ethernet init failed!\n");

	return (0);
}

/******************************
 Routine: dram_init
 Description:  Memory Info
******************************/
int dram_init (void)
{
        DECLARE_GLOBAL_DATA_PTR;

	      gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	      gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

        return 0;
}

