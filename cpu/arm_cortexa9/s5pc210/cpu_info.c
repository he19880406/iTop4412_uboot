/*
 * Copyright (C) 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <asm/io.h>

#ifdef CONFIG_PM
#include "pmic.h"
#endif

ulong get_APLL_CLK(void);
ulong get_MPLL_CLK(void);

void check_bootmode(void);

#define REG_EXYNOS4412_PRO_ID	             0x10000000
#define REG_EXYNOS4412_PACKAGE_ID         0x10000004
#define EXYNOS4412_PRO_ID_VAL                  0xe4412011
#define EXYNOS4412_PACKAGE_ID_VAL         0x0604e008

#define SECURE_PHY_BASE	0x10100018

unsigned int s5pc210_cpu_id;

char * CORE_NUM_STR="\0"; 

int arch_cpu_init(void)
{	
	s5pc210_cpu_id = readl(REG_EXYNOS4412_PRO_ID);
	printf("PRO_ID: 0x%08x, PACKAGE_ID: 0x%08x\n", readl(REG_EXYNOS4412_PRO_ID), readl(REG_EXYNOS4412_PACKAGE_ID));	
	printf("APLL = %ldMHz, MPLL = %ldMHz\n", get_APLL_CLK()/1000000, get_MPLL_CLK()/1000000);
	printf("ARM_CLOCK = %ldMHz\n", get_ARM_CLK()/1000000);
	return 0;
}

u32 get_device_type(void)
{
	return s5pc210_cpu_id;
}

#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
	arch_cpu_init();

#ifdef CONFIG_PM
	printf("PMIC:	");
	PMIC_InitIp();
#endif
	return 0;
}
#endif

