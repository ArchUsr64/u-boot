# SPDX-License-Identifier: GPL-2.0+
#
# Makefile
#
# (C) Copyright 2013 Siemens Schweiz AG
# (C) Heiko Schocher, DENX Software Engineering, hs@denx.de.
#
# Based on:
# u-boot:/board/ti/am335x/Makefile
# Copyright (C) 2011 Texas Instruments Incorporated - https://www.ti.com/

ifdef CONFIG_XPL_BUILD
obj-y	:= mux.o
endif

obj-y	+= board.o
obj-y += ../common/board_am335x.o
obj-y += ../common/eeprom.o
ifndef CONFIG_XPL_BUILD
obj-y += ../common/factoryset.o
endif
