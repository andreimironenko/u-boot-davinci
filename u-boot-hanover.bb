require ${OEBASE}/arago-oe-dev/recipes/u-boot/u-boot.inc
inherit srctree gitver siteinfo


#SECTION = "kernel"
#DESCRIPTION = "Hanover Linux Kernel"
#LICENSE = "GPLv2"
#KERNEL_IMAGETYPE = "uImage"

DESCRIPTION = "u-boot bootloader for DaVinci devices"

PV = "${GITVER}"

FILE_DIRNAME = "${OEBASE}/arago-hanover/recipes/u-boot-hanover"

COMPATIBLE_MACHINE = "dm365-htc"


