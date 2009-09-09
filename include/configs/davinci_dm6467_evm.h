/*
 * davinci_dm6467_evm.h
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

#ifndef __CONFIG_H
#define __CONFIG_H
#include <asm/sizes.h>

/*=======*/
/* Board */
/*=======*/
#define CFG_DM6467_EVM
#define CONFIG_SYS_USE_NAND
#define CONFIG_SYS_NAND_SMALLPAGE

/*===================*/
/* SoC Configuration */
/*===================*/
#define CONFIG_ARM926EJS			/* arm926ejs CPU core */

/* Clock rates detection */
#ifndef __ASSEMBLY__
extern unsigned int davinci_arm_clk_get(void);
extern unsigned int davinci_ddr_clk_get(void);
extern unsigned int davinci_evm_refclk_in(void);
#endif

/* Default CLKREF value. Needs to be hardcoded here since the actual reference
 * clock frequency may not be available till  environment variable is set or the
 * value is set compile time. This value gets overridden by env variable clkref
 * (if it is set form U-Boot prompt.
 *
 * Note: Since environment is accessed only after timer is initialized, the
 * first setting of timer load value uses CFG_REFCLK_FREQ as reference, which
 * can then be reloaded. Also, a reboot is required after setting 'clkref' for
 * new value to take effect.
 *
 * The environment variable value  will generally change depending on the part
 * used and/or board.  e..g, For DM6467 594/729MHz variants -> 27000000
 * For 1GHz (DM6467T) part EVM -> 33000000
 */
#define CFG_REFCLK_FREQ		27000000
#define CONFIG_SYS_CLK_FREQ	davinci_arm_clk_get() /* Arm Clock frequency    */

#define CONFIG_SYS_TIMERBASE		0x01c21400	/* use timer 0 */
#define CONFIG_SYS_HZ_CLOCK		CONFIG_SYS_CLK_FREQ/2	/* Timer Input clock freq */
#define CONFIG_SYS_HZ			1000
#define CONFIG_SOC_DM646X
/*====================================================*/
/* EEPROM definitions for EEPROM */
/*====================================================*/
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN		2
#define CONFIG_SYS_I2C_EEPROM_ADDR		0x50
#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS	6
#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS	20
/*=============*/
/* Memory Info */
/*=============*/
#define CONFIG_SYS_MALLOC_LEN		(0x10000 + 128*1024)	/* malloc() len */
#define CONFIG_SYS_GBL_DATA_SIZE	128		/* reserved for initial data */
#define CONFIG_SYS_MEMTEST_START	0x80000000	/* memtest start address */
#define CONFIG_SYS_MEMTEST_END		0x81000000	/* 16MB RAM test */
#define CONFIG_NR_DRAM_BANKS	1		/* we have 1 bank of DRAM */
#define CONFIG_STACKSIZE	(256*1024)	/* regular stack */
#define PHYS_SDRAM_1		0x80000000	/* DDR Start */
#define PHYS_SDRAM_1_SIZE	0x10000000	/* DDR size 256MB */
#define DDR_8BANKS				/* 8-bank DDR2 (256MB) */
/*====================*/
/* Serial Driver info */
/*====================*/
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	4 		/* NS16550 register size, byteorder */
#define CONFIG_SYS_NS16550_COM1		0x01c20000	/* Base address of UART0 */
#define CONFIG_SYS_NS16550_CLK		24000000	/* Input clock to NS16550 */
#define CONFIG_CONS_INDEX		1		/* use UART0 for console */
#define CONFIG_BAUDRATE			115200		/* Default baud rate */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
/*===================*/
/* I2C Configuration */
/*===================*/
#define CONFIG_HARD_I2C
#define CONFIG_DRIVER_DAVINCI_I2C
#define CONFIG_SYS_I2C_SPEED		80000	/* 100Kbps won't work, silicon bug */
#define CONFIG_SYS_I2C_SLAVE		10	/* Bogus, master-only in U-Boot */
/*==================================*/
/* Network & Ethernet Configuration */
/*==================================*/
#define CONFIG_DRIVER_TI_EMAC
#define CONFIG_MII
#define CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_DNS
#define CONFIG_BOOTP_DNS2
#define CONFIG_BOOTP_SEND_HOSTNAME
#define CONFIG_NET_RETRY_COUNT	10
#define CONFIG_NET_MULTI
/*=====================*/
/* Flash & Environment */
/*=====================*/
#define CONFIG_SYS_NO_FLASH
#ifdef CONFIG_SYS_USE_NAND
#define CONFIG_NAND_DAVINCI
#undef CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_IS_IN_NAND		/* U-Boot env in NAND Flash  */
#ifdef CONFIG_SYS_NAND_SMALLPAGE
#define CONFIG_ENV_SECT_SIZE	512	/* Env sector Size */
#define CONFIG_ENV_SIZE		SZ_16K
#else
#define CONFIG_ENV_SECT_SIZE	2048	/* Env sector Size */
#define CONFIG_ENV_SIZE		SZ_128K
#endif
#define CONFIG_SYS_NAND_BASE		0x42000000
#define CONFIG_SYS_NAND_HW_ECC
#define CONFIG_SYS_MAX_NAND_DEVICE	1	/* Max number of NAND devices */
#define	CONFIG_MASK_CLE			0x80000
#define	CONFIG_MASK_ALE			0x40000
#define CONFIG_ENV_OFFSET		0	/* Block 0--not used by bootcode */
#define DEF_BOOTM		""
#else
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SIZE		SZ_4K
#endif

/*
 * Additional environment variables:
 * 	addclk: Appends value of clkref variable to kernel command line
 */
#define	CONFIG_EXTRA_ENV_SETTINGS					\
	"addclk=setenv bootargs ${bootargs} clkref=${clkref}\0"		\
	""

/*==============================*/
/* U-Boot general configuration */
/*==============================*/
#undef	CONFIG_USE_IRQ			/* No IRQ/FIQ in U-Boot */
#define CONFIG_MISC_INIT_R
#define CONFIG_SKIP_LOWLEVEL_INIT	/* U-Boot is loaded by a bootloader */
#define CONFIG_SKIP_RELOCATE_UBOOT	/* to a proper address, init done */
#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTFILE			"uImage"	/* Boot file name */
#define CONFIG_SYS_PROMPT		"DM6467 EVM > "	/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE		1024		/* Console I/O Buffer Size  */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)	/* Print buffer sz */
#define CONFIG_SYS_MAXARGS		16		/* max number of command args */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size */
#define CONFIG_SYS_LOAD_ADDR		0x80700000	/* default Linux kernel load address */
#define CONFIG_VERSION_VARIABLE
#define CONFIG_AUTO_COMPLETE		/* Won't work with hush so far, may be later */
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_LONGHELP
#define CONFIG_CRC32_VERIFY
#define CONFIG_MX_CYCLIC
/*===================*/
/* Linux Information */
/*===================*/
#define LINUX_BOOT_PARAM_ADDR		0x80000100
#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#ifdef CONFIG_ENV_IS_NOWHERE
#define CONFIG_BOOTCOMMAND   		"autoscr 0x82080000"
#else
#define CONFIG_BOOTARGS			"mem=120M console=ttyS0,115200n8 root=/dev/hda1 rw noinitrd ip=dhcp"
#define CONFIG_BOOTCOMMAND		"dhcp; bootm"
#endif
/*=================*/
/* U-Boot commands */
/*=================*/
#include <config_cmd_default.h>
#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_DIAG
#define CONFIG_CMD_I2C
#define CONFIG_CMD_MII
#define CONFIG_CMD_PING
#define CONFIG_CMD_SAVES
#define CONFIG_CMD_EEPROM
#undef CONFIG_CMD_BDI
#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_SETGETDCR
#ifdef CONFIG_SYS_USE_NAND
#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_IMLS
#define CONFIG_CMD_NAND
#endif
/*=======================*/
/* KGDB support (if any) */
/*=======================*/
#ifdef CONFIG_CMD_KGDB
#define CONFIG_KGDB_BAUDRATE	115200	/* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	1	/* which serial port to use */
#endif
#endif /* __CONFIG_H */
