/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright 2023 Toradex - https://www.toradex.com/
 */

#ifndef _K3_COMMON_FDT_H
#define _K3_COMMON_FDT_H

int fdt_fixup_msmc_ram_k3(void *blob);
int fdt_del_node_path(void *blob, const char *path);
int fdt_fixup_reserved(void *blob, const char *name,
		       unsigned int new_address, unsigned int new_size);
/**
 * fdt_set_assigned_clock_rate() - Assigns a new clock value to the
 * "assigned-clock-rates" property for a matching entry in "clock-names".
 *
 * @blob: Fdt blob
 * @nodeoffset: Offset to the parent node
 * @clock_name: The matching string entry in "clock-names" stringlist
 * @new_clock: New value to set in "assigned-clock-rates"
 * Return: 0 if ok, negative on error
 */
int fdt_set_assigned_clock_rate(void *blob, int nodeoffset,
				const char *clock_name, unsigned int new_clock);
void fdt_fixup_thermal_critical_trips_k3(void *blob, int maxc);

#endif /* _K3_COMMON_FDT_H */
