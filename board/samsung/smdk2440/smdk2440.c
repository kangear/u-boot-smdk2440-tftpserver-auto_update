/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
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
#include <netdev.h>
#include <s3c2410.h>

/*^@@+1@@<2>: 提供规范的IO操作*/
#include <asm/io.h>
/*$*/

DECLARE_GLOBAL_DATA_PTR;

#define FCLK_SPEED 1

#if FCLK_SPEED==0		/* Fout = 203MHz, Fin = 12MHz for Audio */
#define M_MDIV	0xC3
#define M_PDIV	0x4
#define M_SDIV	0x1
#elif FCLK_SPEED==1	

/*^@@+1@@<2>: s3c2410 MPLL 202.8MHz*/
#if defined(CONFIG_S3C2410)
/*$*/
#define M_MDIV	0xA1
#define M_PDIV	0x3
#define M_SDIV	0x1
/*^@@+5@@<2>: s3c2440 MPLL 405MHz*/
#elif defined(CONFIG_S3C2440)
#define M_MDIV 0x7f	
#define M_PDIV 0x2
#define M_SDIV 0x1
#endif
/*$*/

#endif

#define USB_CLOCK 1

#if USB_CLOCK==0
#define U_M_MDIV	0xA1
#define U_M_PDIV	0x3
#define U_M_SDIV	0x1
#elif USB_CLOCK==1
/* UPLL 48MHz */
#define U_M_MDIV	0x48
#define U_M_PDIV	0x3
#define U_M_SDIV	0x2
#endif

static inline void delay (unsigned long loops)
{
	__asm__ volatile ("1:\n"
	  "subs %0, %1, #1\n"
	  "bne 1b":"=r" (loops):"0" (loops));
}

/*
 * Miscellaneous platform dependent initialisations
 */

int board_init (void)
{
	struct s3c24x0_clock_power * const clk_power =
					s3c24x0_get_base_clock_power();
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();

	/* to reduce PLL lock time, adjust the LOCKTIME register */
	clk_power->LOCKTIME = 0xFFFFFF;

	/* configure MPLL */
	clk_power->MPLLCON = ((M_MDIV << 12) + (M_PDIV << 4) + M_SDIV);

	/* some delay between MPLL and UPLL */
	delay (4000);

	/* configure UPLL */
	clk_power->UPLLCON = ((U_M_MDIV << 12) + (U_M_PDIV << 4) + U_M_SDIV);

	/* some delay between MPLL and UPLL */
	delay (8000);

	/* set up the I/O ports */
	gpio->GPACON = 0x007FFFFF;
/*^@@-1,+1@@<2>: GPIO设置需要参考原理图*/
//    gpio->GPBCON = 0x00044555;
	gpio->GPBCON = 0x00295551;
/*$*/	
    gpio->GPBUP = 0x000007FF;
	gpio->GPCCON = 0xAAAAAAAA;
	gpio->GPCUP = 0x0000FFFF;
	gpio->GPDCON = 0xAAAAAAAA;
	gpio->GPDUP = 0x0000FFFF;
	gpio->GPECON = 0xAAAAAAAA;
	gpio->GPEUP = 0x0000FFFF;

/*
 * ^@@-1,+1@@<5>: GPF7接DM9000的INT端口，因此我们将其配置
 * 为EINT7，虽然u-boot的DM9000并未采用中断方式。
 */
//	gpio->GPFCON = 0x000055AA;
	gpio->GPFCON = 0x0000AAAA;
/*$*/	

	gpio->GPFUP = 0x000000FF;
	gpio->GPGCON = 0xFF95FFBA;
	gpio->GPGUP = 0x0000FFFF;
	gpio->GPHCON = 0x002AFAAA;
	gpio->GPHUP = 0x000007FF;

/*
 * ^@@-2,+2@@<6>: 修改机器类型号，MACH_TYPE_SMDK2440
 * 宏需要在include/asm-arm/mach-types.h中添加*/
//	/* arch number of SMDK2410-Board */
//	gd->bd->bi_arch_number = MACH_TYPE_SMDK2410;
	/* arch number of SMDK2440-Board */
	gd->bd->bi_arch_number = MACH_TYPE_SMDK2440;
/*$*/	

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x30000100;

	icache_enable();
	dcache_enable();

	return 0;
}

int dram_init (void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

	return 0;
}

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_CS8900
	rc = cs8900_initialize(0, CONFIG_CS8900_BASE);

/*^@@+2@@<2>*/
#elif defined(CONFIG_DRIVER_DM9000)
	rc = dm9000_initialize(bis);
/*$*/	
#endif
	return rc;
}
#endif

/*
 * ^@@+47@@<3>: LED操作函数
 * LED1~4对应GPB5～8，mini2440/GQ2440的LED不分颜色，
 * 这里用LEDred, green, yellow, blue分别表示LED1~4
 */
#ifdef CONFIG_SMDK2440_LED
void inline coloured_LED_init (void) {
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
    writel(readl(&gpio->GPBCON) & ~0x3fc00 | 0x15400, &gpio->GPBCON);    
    writel(readl(&gpio->GPBDAT) | 0xf<<5, &gpio->GPBDAT);
}

void inline red_LED_on (void) {
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
    writel(readl(&gpio->GPBDAT) & ~(1<<5), &gpio->GPBDAT);
}

void inline red_LED_off(void) {
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
    writel(readl(&gpio->GPBDAT) | 1<<5, &gpio->GPBDAT);
}

void inline green_LED_on(void) {
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
    writel(readl(&gpio->GPBDAT) & ~(1<<6), &gpio->GPBDAT);
}

void inline green_LED_off(void) {
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
    writel(readl(&gpio->GPBDAT) | 1<<6, &gpio->GPBDAT);
}

void inline yellow_LED_on(void) {
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
    writel(readl(&gpio->GPBDAT) & ~(1<<7), &gpio->GPBDAT);
}

void inline yellow_LED_off(void) {
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
    writel(readl(&gpio->GPBDAT) | 1<<7, &gpio->GPBDAT);
}

void inline blue_LED_on(void) {
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
    writel(readl(&gpio->GPBDAT) & ~(1<<8), &gpio->GPBDAT);
}

void inline blue_LED_off(void) {
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
    writel(readl(&gpio->GPBDAT) | 1<<8, &gpio->GPBDAT);
}
#endif
/*$*/
