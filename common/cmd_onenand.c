/*
 *  U-Boot command for OneNAND support
 *
 *  Copyright (C) 2005-2007 Samsung Electronics
 *  Kyungmin Park <kyungmin.park@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <common.h>
#include <command.h>

#include <linux/mtd/compat.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/onenand.h>

#include <asm/io.h>

extern struct mtd_info onenand_mtd;
extern struct onenand_chip onenand_chip;

int do_onenand(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int ret = 0;
	loff_t	ofs = 0;

	switch (argc) {
	case 0:
	case 1:
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;

	case 2:
		if (strncmp(argv[1], "open", 4) == 0) {
			onenand_init();
			return 0;
		}
		if (strncmp(argv[1], "info", 4) != 0)
			printf("OneNAND: Incorrect command/number of args\n");
		printf("%s\n", onenand_mtd.name);
		return 0;

	case 3:
		if ((strncmp(argv[1], "markbad", 7) == 0) && (argc == 3)) {
			int ret ;
			ofs = simple_strtoul(argv[2], NULL, 16);
			if (ofs >= onenand_mtd.size) {
				printf("Error : offset exceeds size\n");
				return 0;
			} else
				ret = onenand_block_markbad(&onenand_mtd, ofs);

			if (ret)
				printf("Error marking bad-block\n");
			else
				printf("Done\n");
			return 0;
		}
		printf("OneNAND : wrong number of arguments\n");
		onenand_print_device_info(onenand_chip.device_id);
		printf("Usage:\n%s\n", cmdtp->usage);
		return 0;

	default:
		/* At least 4 args */
		if (((strncmp(argv[1], "erase", 5) == 0) ||
			(strncmp(argv[1], "scrub", 5) == 0)) &&
			(argc == 4)) {
			struct erase_info instr = {
				.callback	= NULL,
			};
			ulong start = 0, end = 0;
			ulong block = 0;
			char *endtail;

			if (strncmp(argv[2], "block", 5) == 0) {
				start = simple_strtoul(argv[3], NULL, 10);
				endtail = strchr(argv[3], '-');
				end = simple_strtoul(endtail + 1, NULL, 10);
				if (end < start) {
					printf("Error : erase failed - ");
					printf("end block incorrect\n");
					break;
				}
			} else {
				start = simple_strtoul(argv[2], NULL, 10);
				end = simple_strtoul(argv[3], NULL, 10);

				start >>= onenand_chip.erase_shift;
				end >>= onenand_chip.erase_shift;
				/* Don't include the end block */
				end--;
			}

			if (!end || (end < start)) {
				printf("Error : erase failed ");
				printf("end address incorrect\n");
				break;
			}

			printf("Erase block from %lu to %lu\n", start, end);

			for (block = start; block <= end; block++) {
				instr.addr = block << onenand_chip.erase_shift;
				instr.len = 1 << onenand_chip.erase_shift;
				if (strncmp(argv[1], "scrub", 5) == 0)
					instr.priv = ONENAND_SCRUB;
				else
					instr.priv = 0;
				ret = onenand_erase(&onenand_mtd, &instr);
				if (ret) {
					printf("erase failed %lu\n", block);
					break;
				}
			}

			return 0;
		} else if ((strncmp(argv[1], "read", 4) == 0) &&
				(argc == 5)) {
			ulong addr = simple_strtoul(argv[2], NULL, 16);
			ulong ofs = simple_strtoul(argv[3], NULL, 16);
			size_t len = simple_strtoul(argv[4], NULL, 16);
			int oob = strncmp(argv[1], "read.oob", 8) ? 0 : 1;
			struct mtd_oob_ops ops;

			ops.mode = MTD_OOB_PLACE;

			if (oob) {
				ops.len = 0;
				ops.datbuf = NULL;
				ops.ooblen = len;
				ops.oobbuf = (u_char *) addr;
				ops.ooboffs = 0;
			} else {
				ops.len = len;
				ops.datbuf = (u_char *) addr;
				ops.ooblen = 0;
				ops.oobbuf = NULL;
			}
			ops.retlen = ops.oobretlen = 0;

			ret = onenand_mtd.read_oob(&onenand_mtd, ofs, &ops);
			if (ret)
				printf("Error reading from device\n");
			else
				printf("Done\n");

			return 0;

		} else if ((strncmp(argv[1], "write", 5) == 0) &&
			(argc == 5)) {
			ulong addr = simple_strtoul(argv[2], NULL, 16);
			ulong ofs = simple_strtoul(argv[3], NULL, 16);
			size_t len = simple_strtoul(argv[4], NULL, 16);
			size_t retlen = 0;

			ret = onenand_write(&onenand_mtd, ofs, len, &retlen,
				      (u_char *) addr);
			if (ret)
				printf("Error writing to device\n");
			else
				printf("Done\n");

			return 0;
		} else if (strncmp(argv[1], "block", 5) == 0) {
			ulong addr = simple_strtoul(argv[2], NULL, 16);
			ulong block = simple_strtoul(argv[3], NULL, 10);
			ulong page;
			size_t len;
			ulong ofs;
			int oob = strncmp(argv[1], "block.oob", 9) ? 0 : 1;
			struct mtd_oob_ops ops;

			ops.mode = MTD_OOB_PLACE;

			if (argc >= 5)
				page = simple_strtoul(argv[4], NULL, 10);
			if (argc >= 6)
				len = simple_strtol(argv[5], NULL, 10);
			ofs = block << onenand_chip.erase_shift;
			if (page)
				ofs += page << onenand_chip.page_shift;

			if (!len) {
				if (oob)
					ops.ooblen = 64;
				else
					ops.len = 512;
			}

			if (oob) {
				ops.datbuf = NULL;
				ops.oobbuf = (u_char *) addr;
			} else {
				ops.datbuf = (u_char *) addr;
				ops.oobbuf = NULL;
			}
			ops.retlen = ops.oobretlen = 0;

			onenand_read_oob(&onenand_mtd, ofs, &ops);
			return 0;
		} else {
			printf("OneNAND : wrong number of arguments\n");
			onenand_print_device_info(onenand_chip.device_id);
			printf("Usage:\n%s\n", cmdtp->usage);
		}

		break;
	}

	return 0;
}

U_BOOT_CMD(
	onenand,	6,	1,	do_onenand,
	"onenand - OneNAND sub-system\n",
	"info   - show available OneNAND devices\n"
	"onenand read[.oob] addr ofs len - read data at ofs with len to addr\n"
	"       ---ofs and len(not for oob) have to be page aligned\n"
	"onenand write addr ofs len - write data at ofs with len from addr\n"
	"       ---ofs and len have to be page aligned\n"
	"onenand erase sofs eofs - erase block start addr ofs to end addr ofs\n"
	"onenand erase block sblk-endblk - erase blocks sblk to endblk\n"
	"       ---erase command does not erase bad blocks\n"
	"onenand scrub block start-end - erase block from start to end\n"
	"onenand scrub sofs eofs - erase blocks start ofs to end ofs\n"
	"       ---CAUTION :scrub command erases bad blocks also\n"
	"onenand block[.oob] addr block [page] [len] - "
		"read data with (block [, page]) to addr\n"
	"onenand markbad ofs - mark bad-block at ofs\n"
);
