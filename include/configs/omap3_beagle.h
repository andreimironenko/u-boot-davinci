/*
 * (C) Copyright 2006-2008
 * Texas Instruments.
 * Richard Woodruff <r-woodruff2@ti.com>
 * Syed Mohammed Khasim <x0khasim@ti.com>
 *
 * Configuration settings for the TI OMAP3530 Beagle board.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H
#include <asm/sizes.h>

/*
 * High Level Configuration Options
 */
#define CONFIG_ARMCORTEXA8	1	/* This is an ARM V7 CPU core */
#define CONFIG_OMAP		1	/* in a TI OMAP core */
#define CONFIG_OMAP34XX		1	/* which is a 34XX */
#define CONFIG_OMAP3430		1	/* which is in a 3430 */
#define CONFIG_OMAP3_BEAGLE	1	/* working with BEAGLE */

#include <asm/arch/cpu.h>		/* get chip and board defs */
#include <asm/arch/omap3.h>

/* Clock Defines */
#define V_OSCK			26000000	/* Clock output from T2 */
#define V_SCLK			(V_OSCK >> 1)

#undef CONFIG_USE_IRQ				/* no support for IRQs */
#define CONFIG_MISC_INIT_R

#define CONFIG_CMDLINE_TAG		1	/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS	1
#define CONFIG_INITRD_TAG		1
#define CONFIG_REVISION_TAG		1

/*
 * Size of malloc() pool
 */
#define CONFIG_ENV_SIZE			SZ_128K	/* Total Size Environment */
						/* Sector */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + SZ_128K)
#define CONFIG_SYS_GBL_DATA_SIZE	128	/* bytes reserved for */
						/* initial data */

/*
 * Hardware drivers
 */

/*
 * NS16550 Configuration
 */
#define V_NS16550_CLK			48000000	/* 48MHz (APLL96/2) */

#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_CLK		V_NS16550_CLK

/*
 * select serial console configuration
 */
#define CONFIG_CONS_INDEX		3
#define CONFIG_SYS_NS16550_COM3		OMAP34XX_UART3
#define CONFIG_SERIAL3			3	/* UART3 on Beagle Rev 2 */

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{4800, 9600, 19200, 38400, 57600,\
					115200}
#define CONFIG_MMC			1
#define CONFIG_OMAP3_MMC		1
#define CONFIG_SYS_MMC_BASE		0xF0000000
#define CONFIG_DOS_PARTITION		1

/* commands to include */

#define CONFIG_CMD_EXT2		/* EXT2 Support			*/
#define CONFIG_CMD_FAT		/* FAT support			*/
#define CONFIG_CMD_JFFS2	/* JFFS2 Support		*/

#define CONFIG_CMD_I2C		/* I2C serial bus support	*/
#define CONFIG_CMD_MMC		/* MMC support			*/
#define CONFIG_CMD_NAND		/* NAND support			*/

#define CONFIG_CMD_AUTOSCRIPT	/* autoscript support		*/
#define CONFIG_CMD_BDI		/* bdinfo			*/
#define CONFIG_CMD_BOOTD	/* bootd			*/
#define CONFIG_CMD_CONSOLE	/* coninfo			*/
#define CONFIG_CMD_ECHO		/* echo arguments		*/
#define CONFIG_CMD_ENV		/* saveenv			*/
#define CONFIG_CMD_ITEST	/* Integer (and string) test	*/
#define CONFIG_CMD_LOADB	/* loadb			*/
#define CONFIG_CMD_MEMORY	/* md mm nm mw cp cmp crc base loop mtest */
#define CONFIG_CMD_MISC		/* misc functions like sleep etc*/
#define CONFIG_CMD_RUN		/* run command in env variable	*/

#define CONFIG_SYS_NO_FLASH
#define CONFIG_SYS_I2C_SPEED		100000
#define CONFIG_SYS_I2C_SLAVE		1
#define CONFIG_SYS_I2C_BUS		0
#define CONFIG_SYS_I2C_BUS_SELECT	1
#define CONFIG_DRIVER_OMAP34XX_I2C	1

/*
 * Board NAND Info.
 */
#define CONFIG_NAND_OMAP_GPMC
#define CONFIG_SYS_NAND_ADDR		NAND_BASE	/* physical address */
							/* to access nand */
#define CONFIG_SYS_NAND_BASE		NAND_BASE	/* physical address */
							/* to access nand at */
							/* CS0 */
#define GPMC_NAND_ECC_LP_x16_LAYOUT	1

#define CONFIG_SYS_MAX_NAND_DEVICE	1		/* Max number of NAND */
							/* devices */
#define SECTORSIZE			512

#define NAND_ALLOW_ERASE_ALL
#define ADDR_COLUMN			1
#define ADDR_PAGE			2
#define ADDR_COLUMN_PAGE		3

#define NAND_ChipID_UNKNOWN		0x00
#define NAND_MAX_FLOORS			1
#define NAND_MAX_CHIPS			1
#define NAND_NO_RB			1
#define CONFIG_SYS_NAND_WP

#define CONFIG_JFFS2_NAND
/* nand device jffs2 lives on */
#define CONFIG_JFFS2_DEV		"nand0"
/* start of jffs2 partition */
#define CONFIG_JFFS2_PART_OFFSET	0x680000
#define CONFIG_JFFS2_PART_SIZE		0xf980000	/* size of jffs2 */
							/* partition */

/* Environment information */
#define CONFIG_BOOTDELAY		10

#define CONFIG_BOOTCOMMAND		"nand read 80200000 280000 400000 ; " \
					"bootm 80200000"

#define CONFIG_BOOTARGS			"setenv bootargs console=ttyS2," \
					"115200n8 noinitrd " \
					"root=/dev/mtdblock4 " \
					"rw rootfstype=jffs2"

#define CONFIG_NETMASK			255.255.254.0
#define CONFIG_BOOTFILE			"uImage"
#define CONFIG_AUTO_COMPLETE		1
/*
 * Miscellaneous configurable options
 */
#define V_PROMPT			"OMAP3 beagleboard.org # "

#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_PROMPT		V_PROMPT
#define CONFIG_SYS_CBSIZE		256	/* Console I/O Buffer Size */
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS		16	/* max number of command args */
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		(CONFIG_SYS_CBSIZE)

#define CONFIG_SYS_MEMTEST_START	(OMAP34XX_SDRC_CS0)	/* memtest */
								/* works on */
#define CONFIG_SYS_MEMTEST_END		(OMAP34XX_SDRC_CS0 + \
					0x01F00000) /* 31MB */

#undef	CONFIG_SYS_CLKS_IN_HZ		/* everything, incl board info, in Hz */

#define CONFIG_SYS_LOAD_ADDR		(OMAP34XX_SDRC_CS0)	/* default */
							/* load address */

/*
 * 2430 has 12 GP timers, they can be driven by the SysClk (12/13/19.2) or by
 * 32KHz clk, or from external sig. This rate is divided by a local divisor.
 */
#define V_PVT				7

#define CONFIG_SYS_TIMERBASE		(OMAP34XX_GPT2)
#define CONFIG_SYS_PVT			V_PVT	/* 2^(pvt+1) */
#define CONFIG_SYS_HZ			((V_SCLK) / (2 << CONFIG_SYS_PVT))

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	SZ_128K	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	SZ_4K	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	SZ_4K	/* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	2	/* CS1 may or may not be populated */
#define PHYS_SDRAM_1		OMAP34XX_SDRC_CS0
#define PHYS_SDRAM_1_SIZE	SZ_32M	/* at least 32 meg */
#define PHYS_SDRAM_2		OMAP34XX_SDRC_CS1

/* SDRAM Bank Allocation method */
#define SDRC_R_B_C		1

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */

/* **** PISMO SUPPORT *** */

/* Configure the PISMO */
#define PISMO1_NOR_SIZE_SDPV2		GPMC_SIZE_128M
#define PISMO1_NOR_SIZE			GPMC_SIZE_64M

#define PISMO1_NAND_SIZE		GPMC_SIZE_128M
#define PISMO1_ONEN_SIZE		GPMC_SIZE_128M
#define DBG_MPDB_SIZE			GPMC_SIZE_16M
#define PISMO2_SIZE			0

#define CONFIG_SYS_MAX_FLASH_SECT	520	/* max number of sectors on */
						/* one chip */
#define CONFIG_SYS_MAX_FLASH_BANKS	2	/* max number of flash banks */
#define CONFIG_SYS_MONITOR_LEN		SZ_256K	/* Reserve 2 sectors */

#define PHYS_FLASH_SIZE_SDPV2		SZ_128M
#define PHYS_FLASH_SIZE			SZ_32M

#define CONFIG_SYS_FLASH_BASE		boot_flash_base
#define PHYS_FLASH_SECT_SIZE		boot_flash_sec
/* Dummy declaration of flash banks to get compilation right */
#define CONFIG_SYS_FLASH_BANKS_LIST	{0, 0}

/* Monitor at start of flash */
#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_ONENAND_BASE		ONENAND_MAP

#define CONFIG_ENV_IS_IN_NAND		1
#define ONENAND_ENV_OFFSET		0x260000 /* environment starts here */
#define SMNAND_ENV_OFFSET		0x260000 /* environment starts here */

#define CONFIG_SYS_ENV_SECT_SIZE	boot_flash_sec
#define CONFIG_ENV_OFFSET		boot_flash_off
#define CONFIG_ENV_ADDR			SMNAND_ENV_OFFSET

/*-----------------------------------------------------------------------
 * CFI FLASH driver setup
 */
/* timeout values are in ticks */
#define CONFIG_SYS_FLASH_ERASE_TOUT	(100 * CONFIG_SYS_HZ)
#define CONFIG_SYS_FLASH_WRITE_TOUT	(100 * CONFIG_SYS_HZ)

/* Flash banks JFFS2 should use */
#define CONFIG_SYS_MAX_MTD_BANKS	(CONFIG_SYS_MAX_FLASH_BANKS + \
					CONFIG_SYS_MAX_NAND_DEVICE)
#define CONFIG_SYS_JFFS2_MEM_NAND
/* use flash_info[2] */
#define CONFIG_SYS_JFFS2_FIRST_BANK	CONFIG_SYS_MAX_FLASH_BANKS
#define CONFIG_SYS_JFFS2_NUM_BANKS	1

#define ENV_IS_VARIABLE		1
#define CONFIG_ENV_IS_RUNTIME_SEL	1

#ifndef __ASSEMBLY__
extern unsigned int *nand_cs_base;
extern unsigned int boot_flash_base;
extern volatile unsigned int boot_flash_env_addr;
extern unsigned int boot_flash_off;
extern unsigned int boot_flash_sec;
extern unsigned int boot_flash_type;
#endif


#define WRITE_NAND_COMMAND(d, adr)\
			writel(d, (nand_cs_base + OFFS(GPMC_NAND_CMD)))
#define WRITE_NAND_ADDRESS(d, adr)\
			writel(d, (nand_cs_base + OFFS(GPMC_NAND_ADR)))
#define WRITE_NAND(d, adr) writew(d, (nand_cs_base + OFFS(GPMC_NAND_DAT)))
#define READ_NAND(adr) readl((nand_cs_base + OFFS(GPMC_NAND_DAT)))

/* Other NAND Access APIs */
#define NAND_WP_OFF() do {readl(GPMC_CONFIG_REG) |= GPMC_CONFIG_WP; } while (0)
#define NAND_WP_ON() do {readl(GPMC_CONFIG_REG) &= ~GPMC_CONFIG_WP; } while (0)
#define NAND_DISABLE_CE(nand)
#define NAND_ENABLE_CE(nand)
#define NAND_WAIT_READY(nand)	udelay(10)

#endif /* __CONFIG_H */
