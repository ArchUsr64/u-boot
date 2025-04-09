/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2025 Texas Instruments Incorporated - https://www.ti.com/
 *	Anshul Dalal <anshuld@ti.com>
 */

/*
	This jump stub is to be loaded at the older PRELOADED_BL33_BASE to forward
	execution to SPL_TEXT_BASE when using older ATF builds.
 */

.text

jump_stub:
	mov x15, CONFIG_SPL_TEXT_BASE
	br x15
