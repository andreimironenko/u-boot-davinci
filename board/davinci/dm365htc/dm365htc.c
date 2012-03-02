/*
 * Copyright (C) 2009 Texas Instruments Incorporated
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <common.h>
#include <nand.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/emif_defs.h>
#include <asm/arch/nand_defs.h>
#include <asm/arch/gpio_defs.h>
#include <netdev.h>
#include "../common/misc.h"
#ifdef CONFIG_DAVINCI_MMC
#include <mmc.h>
#include <asm/arch/sdmmc_defs.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	// TODO: we really should have MACH_TYPE_DAVINCI_DM365_HTC, even
	// if has the same value as MACH_TYPE_DAVINCI_DM365_EVM
	gd->bd->bi_arch_number = MACH_TYPE_DAVINCI_DM365_EVM;
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

	return 0;
}

#ifdef CONFIG_DRIVER_TI_EMAC
int board_eth_init(bd_t *bis)
{
	uint8_t eeprom_enetaddr[6];
	int i;
	struct davinci_gpio *gpio1_base =
			(struct davinci_gpio *)DAVINCI_GPIO_BANK01;

	/* Configure PINMUX 3 to enable EMAC pins */
	writel((readl(PINMUX3) | 0x1affff), PINMUX3);

	/* Configure GPIO20 as output */
	writel((readl(&gpio1_base->dir) & ~(1 << 20)), &gpio1_base->dir);

	/* Toggle GPIO 20 */
	for (i = 0; i < 20; i++) {
		/* GPIO 20 low */
		writel((readl(&gpio1_base->out_data) & ~(1 << 20)),
						&gpio1_base->out_data);

		udelay(1000);

		/* GPIO 20 high */
		writel((readl(&gpio1_base->out_data) | (1 << 20)),
						&gpio1_base->out_data);
	}

	/* Configure I2C pins so that EEPROM can be read */
	writel((readl(PINMUX3) | 0x01400000), PINMUX3);

	/* Read Ethernet MAC address from EEPROM */
	if (dvevm_read_mac_address(eeprom_enetaddr))
		davinci_sync_env_enetaddr(eeprom_enetaddr);

	davinci_emac_initialize();

	return 0;
}
#endif

#ifdef CONFIG_NAND_DAVINCI
static void nand_dm365evm_select_chip(struct mtd_info *mtd, int chip)
{
	struct nand_chip	*this = mtd->priv;
	unsigned long		wbase = (unsigned long) this->IO_ADDR_W;
	unsigned long		rbase = (unsigned long) this->IO_ADDR_R;

	if (chip == 1) {
		__set_bit(14, &wbase);
		__set_bit(14, &rbase);
	} else {
		__clear_bit(14, &wbase);
		__clear_bit(14, &rbase);
	}
	this->IO_ADDR_W = (void *)wbase;
	this->IO_ADDR_R = (void *)rbase;
}

int board_nand_init(struct nand_chip *nand)
{
	davinci_nand_init(nand);
	nand->select_chip = nand_dm365evm_select_chip;
	return 0;
}
#endif

#ifdef CONFIG_DAVINCI_MMC

static struct davinci_mmc mmc_sd0 = {
	.reg_base = (struct davinci_mmc_regs *)DAVINCI_MMC_SD0_BASE,
	.input_clk = 121500000,
	.host_caps = MMC_MODE_4BIT,
	.voltages = MMC_VDD_32_33 | MMC_VDD_33_34,
	.version = MMC_CTLR_VERSION_2,
};

#ifdef CONFIG_DAVINCI_MMC_SD1
static struct davinci_mmc mmc_sd1 = {
	.reg_base = (struct davinci_mmc_regs *)DAVINCI_MMC_SD1_BASE,
	.input_clk = 121500000,
	.host_caps = MMC_MODE_4BIT,
	.voltages = MMC_VDD_32_33 | MMC_VDD_33_34,
	.version = MMC_CTLR_VERSION_2,
};
#endif

int board_mmc_init(bd_t *bis)
{
	int err;

	/* Add slot-0 to mmc subsystem */
	err = davinci_mmc_init(bis, &mmc_sd0);
	if (err)
		return err;

#ifdef CONFIG_DAVINCI_MMC_SD1
#define PUPDCTL1		0x01c4007c
	/* PINMUX(4)-DAT0-3/CMD;  PINMUX(0)-CLK */
	writel((readl(PINMUX4) | 0x55400000), PINMUX4);
	writel((readl(PINMUX0) | 0x00010000), PINMUX0);

	/* Configure MMC/SD pins as pullup */
	writel((readl(PUPDCTL1) & ~0x07c0), PUPDCTL1);

	/* Add slot-1 to mmc subsystem */
	err = davinci_mmc_init(bis, &mmc_sd1);
#endif

	return err;
}

#endif

#ifdef CONFIG_BOOTCOUNT_LIMIT
void bootcount_store (ulong a)
{
	uint blk_start, blk_cnt, n;

	struct mmc *mmc = find_mmc_device(CONFIG_SYS_MMC_BOOTCOUNT_DEV);
	if (!mmc) {
		puts("No MMC card found\n");
		return;
	}

	if (mmc_init(mmc)) {
		puts("MMC init failed\n");
		return;
	}

	n = mmc->write_bl_len > 8 ? mmc->write_bl_len : 8;
	int buffer[n];
	memset(buffer, 0, n);

	blk_start = ALIGN(CONFIG_BOOTCOUNT_OFFSET, mmc->write_bl_len) / mmc->write_bl_len;
	blk_cnt   = ALIGN(8, mmc->write_bl_len) / mmc->write_bl_len;

	buffer[0] = BOOTCOUNT_MAGIC;
	buffer[1] = a;

	n = mmc->block_dev.block_write(CONFIG_SYS_MMC_ENV_DEV, blk_start,
					blk_cnt, (u_char *)buffer);

	if(n != blk_cnt)
		puts("Error writing boot count\n");
}

ulong bootcount_load (void)
{
	uint blk_start, blk_cnt, n;

	struct mmc *mmc = find_mmc_device(CONFIG_SYS_MMC_BOOTCOUNT_DEV);
	if (!mmc) {
		puts("No MMC card found\n");
		return 0;
	}

	if (mmc_init(mmc)) {
		puts("MMC init failed\n");
		return  0;
	}

	n = mmc->read_bl_len > 8 ? mmc->read_bl_len : 8;
	int buffer[n];

	blk_start = ALIGN(CONFIG_BOOTCOUNT_OFFSET, mmc->read_bl_len) / mmc->read_bl_len;
	blk_cnt   = ALIGN(8, mmc->read_bl_len) / mmc->read_bl_len;

	n = mmc->block_dev.block_read(CONFIG_SYS_MMC_BOOTCOUNT_DEV, blk_start,
					blk_cnt, (uchar *)buffer);

	if(n != blk_cnt)
		return 0;

	if(buffer[0] != BOOTCOUNT_MAGIC)
		return 0;
	return buffer[1];
}

#endif /* CONFIG_BOOTCOUNT_LIMIT */
