/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * Gary Jennejohn <garyj@denx.de>
 * David Mueller <d.mueller@elsoft.ch>
 *
 * Configuation settings for the SAMSUNG SMDK2410 board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_ARM920T		1	/* This is an ARM920T Core	*/

/*^@@-2,+2@@<2>*/
//#define	CONFIG_S3C2410		1	/* in a SAMSUNG S3C2410 SoC     */
//#define CONFIG_SMDK2410		1	/* on a SAMSUNG SMDK2410 Board  */
#define	CONFIG_S3C2440		1	/* in a SAMSUNG S3C2440 SoC     */
#define CONFIG_SMDK2440		1	/* on a SAMSUNG SMDK2440 Board  */
/*$*/

/* input clock of PLL */
#define CONFIG_SYS_CLK_FREQ	12000000/* the SMDK2410 has 12MHz input clock */

#define USE_920T_MMU		1
#undef CONFIG_USE_IRQ			/* we don't need IRQ/FIQ stuff */

/*^@@+1@@<3>: 主要是为了跳过SDRAM的初始化，这对将程序下载到内存调试是必要的*/
//#define CONFIG_SKIP_LOWLEVEL_INIT
//#define CONFIG_SKIP_LOWLEVEL_INIT 1
//#define CONFIG_SKIP_RELOCATE_UBOOT 1
/*$*/

/*^@@+2@@<3>*/
//#define CONFIG_ASM_DEBUG     /* 底层调试开关 */
#define CONFIG_SMDK2440_LED  /* LED指示灯 */
/*$*/

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 128*1024)
#define CONFIG_SYS_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

/*
 * Hardware drivers
 */
/*^@@-4,+8@@<5>*/
//#define CONFIG_NET_MULTI
//#define CONFIG_CS8900		/* we have a CS8900 on-board */
//#define CONFIG_CS8900_BASE	0x19000300
//#define CONFIG_CS8900_BUS16	/* the Linux driver does accesses as shorts */
#define CONFIG_NET_MULTI		    1
#define CONFIG_NET_RETRY_COUNT		20
#define CONFIG_DRIVER_DM9000		1
#define CONFIG_DM9000_BASE		0x20000000
#define DM9000_IO 			    CONFIG_DM9000_BASE
#define DM9000_DATA 			(CONFIG_DM9000_BASE+4)
#define CONFIG_DM9000_USE_16BIT		1
#define CONFIG_DM9000_NO_SROM		1
/*$*/

/*
 * select serial console configuration
 */
#define CONFIG_S3C24X0_SERIAL
#define CONFIG_SERIAL1          1	/* we use SERIAL 1 on SMDK2410 */

/************************************************************
 * RTC
 ************************************************************/
#define	CONFIG_RTC_S3C24X0	1

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_BAUDRATE		115200


/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME


/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_CACHE
#define CONFIG_CMD_DATE
#define CONFIG_CMD_ELF

/*^@@+1@@<7>: 添加对nand flash相关命令的支持*/
#define CONFIG_CMD_NAND
/*$*/

/*^@@+1@@<5>: 添加对ping命令的支持*/
#define CONFIG_CMD_PING
/*$*/

/*^@@+2@@<8>: 添加对write.yaffs的支持*/
#define CONFIG_CMD_YAFFS
#define CONFIG_YAFFS_SKIPFB
/*$*/

/*^@@+2@@<8>: 添加调试支持*/
//#define DEBUG 1
/*$*/

/*^@@+2@@<8>: 添加对update的支持*/
#define CONFIG_CMD_UPDATE
#if defined CONFIG_CMD_UPDATE
#define CONFIG_AUTOBOOT_STOP_BUTTON 1
#define KEY1_DOWN (!(readl(&gpio->GPGDAT) & (1<<0)))
#define KEY2_DOWN (!(readl(&gpio->GPGDAT) & (1<<3)))
#define KEY3_DOWN (!(readl(&gpio->GPGDAT) & (1<<5)))
#define KEY4_DOWN (!(readl(&gpio->GPGDAT) & (1<<6)))
#endif
/*$*/

/*^@@+3@@<6>*/
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_CMDLINE_TAG
#define CONFIG_BOOTARGS	"noinitrd root=/dev/mtdblock3 console=ttySAC0,115200 init=/linuxrc mem64M"
/*$*/

/*^@@-1,+4@@<6>: u-boot自启动支持*/
//#define CONFIG_BOOTDELAY	3
#define CONFIG_BOOTDELAY	1
#define CONFIG_BOOTCOMMAND "nand read 0x30008000 0x60000 0x250000;bootm"
/*$*/

/*#define CONFIG_BOOTARGS	"root=ramfs devfs=mount console=ttySA0,9600" */
/*#define CONFIG_ETHADDR	08:00:3e:26:0a:5b */

/*^@@-3,+5@@<5>*/
//#define CONFIG_NETMASK    255.255.255.0
//#define CONFIG_IPADDR		10.0.0.110
//#define CONFIG_SERVERIP	10.0.0.1
#define CONFIG_ETHADDR	    08:90:00:A0:90:90
#define CONFIG_NETMASK      255.255.255.0
#define CONFIG_IPADDR		192.168.1.230
#define CONFIG_SERVERIP		192.168.1.229
#define CONFIG_GATEWAYIP	192.168.1.1
/*$*/

/*#define CONFIG_BOOTFILE	"elinos-lart" */
/*#define CONFIG_BOOTCOMMAND	"tftp; bootm" */

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	115200		/* speed to run kgdb serial port */
/* what's this ? it's not used anywhere */
#define CONFIG_KGDB_SER_INDEX	1		/* which serial port to use */
#endif

/*
 * Miscellaneous configurable options
 */
#define	CONFIG_SYS_LONGHELP				/* undef to save memory		*/
/*^@@-1,+1@@<4>: 修改命令行提示符*/
//#define	CONFIG_SYS_PROMPT		"SMDK2410 # "	/* Monitor Command Prompt	*/
#define	CONFIG_SYS_PROMPT		"SMDK2440 # "	/* Monitor Command Prompt	*/
/*$*/
#define	CONFIG_SYS_CBSIZE		256		/* Console I/O Buffer Size	*/
#define	CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) /* Print Buffer Size */
#define	CONFIG_SYS_MAXARGS		16		/* max number of command args	*/
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size	*/

#define CONFIG_SYS_MEMTEST_START	0x30000000	/* memtest works on	*/
#define CONFIG_SYS_MEMTEST_END		0x33F00000	/* 63 MB in DRAM	*/

/*^@@-1,+1@@<6>: 修改默认内核镜像加载地址，该地址同样也是tftp下载的默认地址*/
//#define	CONFIG_SYS_LOAD_ADDR		0x33000000	/* default load address	*/
#define	CONFIG_SYS_LOAD_ADDR		0x30008000	/* default load address	*/
/*$*/

#define	CONFIG_SYS_HZ			1000

/* valid baudrates */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*^@@+2@@<4>: 使命令行支持自动补齐和方向键*/
#define CONFIG_CMDLINE_EDITING
#define CONFIG_AUTO_COMPLETE
/*$*/

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128*1024)	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1	   /* we have 1 bank of DRAM */
#define PHYS_SDRAM_1		0x30000000 /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE	0x04000000 /* 64 MB */

#define PHYS_FLASH_1		0x00000000 /* Flash Bank #1 */

#define CONFIG_SYS_FLASH_BASE		PHYS_FLASH_1

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */

/*^@@-1,+1@@<4>*/
//#define CONFIG_AMD_LV400	1	/* uncomment this if you have a LV400 flash */
#define CONFIG_AMD_LV160	1	
/*$*/
#if 0
#define CONFIG_AMD_LV800	1	/* uncomment this if you have a LV800 flash */
#endif

#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks */
/*^@@+5@@<4>*/
#ifdef CONFIG_AMD_LV160
#define PHYS_FLASH_SIZE		0x00200000 /* 2MB */
#define CONFIG_SYS_MAX_FLASH_SECT	(35)	/* max number of sectors on one chip */
#define CONFIG_ENV_ADDR		(CONFIG_SYS_FLASH_BASE + 0x080000) /* addr of environment */
#endif
/*$*/
#ifdef CONFIG_AMD_LV800
#define PHYS_FLASH_SIZE		0x00100000 /* 1MB */
#define CONFIG_SYS_MAX_FLASH_SECT	(19)	/* max number of sectors on one chip */
#define CONFIG_ENV_ADDR		(CONFIG_SYS_FLASH_BASE + 0x0F0000) /* addr of environment */
#endif
#ifdef CONFIG_AMD_LV400
#define PHYS_FLASH_SIZE		0x00080000 /* 512KB */
#define CONFIG_SYS_MAX_FLASH_SECT	(11)	/* max number of sectors on one chip */
#define CONFIG_ENV_ADDR		(CONFIG_SYS_FLASH_BASE + 0x070000) /* addr of environment */
#endif

/* timeout values are in ticks */
#define CONFIG_SYS_FLASH_ERASE_TOUT	(5*CONFIG_SYS_HZ) /* Timeout for Flash Erase */
#define CONFIG_SYS_FLASH_WRITE_TOUT	(5*CONFIG_SYS_HZ) /* Timeout for Flash Write */

/*^@@-2,+3@@<7>*/
//#define	CONFIG_ENV_IS_IN_FLASH	1
//#define CONFIG_ENV_SIZE		0x10000	/* Total Size of Environment Sector */
#define	CONFIG_ENV_IS_IN_NAND	1
#define CONFIG_ENV_OFFSET		0x80000	
#define CONFIG_ENV_SIZE		    0x80000	
/*$*/

/*
 * ^@@+8@@<7>
 */
#if defined(CONFIG_CMD_NAND)
#define CONFIG_NAND_S3C2440           /* 板级相关的NAND FLASH驱动支持 */
#define CONFIG_SYS_NAND_BASE 0x4E000000 /* NAND FLASH 控制器寄存器基地址 */ 
#define CONFIG_SYS_MAX_NAND_DEVICE	1 /* NAND FLASH 设备数 */
#define CONFIG_SYS_64BIT_VSPRINTF     /* 使vsprintf支持%L格式，nand_util.c要求定义此宏 */
#define CONFIG_MTD_NAND_VERIFY_WRITE  /* 写后再读出验证 */
#define CONFIG_NAND_HWECC             /* 使用s3c2440硬件ECC */
#endif	/* CONFIG_CMD_NAND */
/*$*/

/*^@@+1@@<7>: 从nand flash拷贝u-boot到内存时使用*/
#define CONFIG_UBOOT_SIZE      (CONFIG_ENV_ADDR - 0)
/*$*/

#endif	/* __CONFIG_H */
