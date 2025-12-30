// SPDX-License-Identifier: GPL-2.0+
/*
 * Board specific initialization for AM62Lx platforms
 *
 * Copyright (C) 2025 Texas Instruments Incorporated - https://www.ti.com/
 *
 */

#include <asm/arch/hardware.h>
#include <asm/io.h>
#include <dm/uclass.h>
#include <env.h>
#include <fdt_support.h>
#include <spl.h>
#include <serial.h>

#include "../common/fdt_ops.h"

int board_init(void)
{
	return 0;
}

#if IS_ENABLED(CONFIG_SPL_OS_BOOT)
int spl_start_uboot(void)
{
	printf("SPL: booting kernel\n");
	/* break into full u-boot on ' ' */
	return serial_tstc() && serial_getc() == ' ';
}
#endif

#if IS_ENABLED(CONFIG_BOARD_LATE_INIT)
int board_late_init(void)
{
	ti_set_fdt_env(NULL, NULL);
	return 0;
}
#endif
