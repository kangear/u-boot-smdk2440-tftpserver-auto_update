/*
 * cmd_update.c
 *
 *  Created on: 2013-3-18
 *      Author: root
 */
#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <s3c2410.h>
#include <linux/types.h>

extern void red_LED_on(void);
extern void green_LED_on(void);
extern void yellow_LED_on(void);
extern void blue_LED_on(void);
extern void red_LED_off(void);
extern void green_LED_off(void);
extern void yellow_LED_off(void);
extern void blue_LED_off(void);

extern int run_command (const char *cmd, int flag);

static unsigned char led_ctl_num;

static int all_update(void);
static int check_header(unsigned long *ram_addr);
static void led_ctl(unsigned char no);

#define ALL_LED_ON 	0
#define LED1_ON    	1
#define LED2_ON    	2
#define LED3_ON    	3
#define LED4_ON    	4
#define ALL_LED_OFF 5

int do_update(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	   printf("Ready to start tftpserver........................\n");
	   printf("Button [ KEY3 ] : start server.\n");
	   printf("Button [ KEY1 ] : exit update.\n");

	   struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
	   //coloured_LED_init();
	   for(;;)
	   {
            /*update ....*/
		   if (KEY3_DOWN)  //KEY3 DOWN update mode.
		   {

                printf("[Info] tftpserver start (Please put up date) ...\n");
                printf("To Start Server.........v1.0\n");
                blue_LED_off();

                if (all_update() < 0)
                {
                	printf("***************************\n");
                	printf("Program Error!\n");
                	printf("Please try again!\n");
                	printf("***************************\n");
					printf("Button [ KEY3 ] : start server.\n");
					printf("Button [ KEY1 ] : exit update.\n");
                }
                else
                {
					led_ctl(led_ctl_num);
					if ( 0 == (readl(&gpio->GPBDAT) & ((1<<5) | (1<<6) | (1<<7))))
					{
						debug("all_update,led4 on\n");
						led_ctl(LED4_ON);  //if bootloader,kernel,rootfs all update turn on led4
					}
					else
					{
						printf("Button [ KEY3 ] : start server.\n");
						printf("Button [ KEY1 ] : exit update.\n");
					}

                }


		   }
            /*Leaving the update mode.*/
		   else if (KEY1_DOWN)
		   {
			   led_ctl(ALL_LED_OFF);
			   printf("Leaving the update mode.\n");
			   break;
		   }
            /*if all leds ON, reboot*/
		   else if ( 0 == (readl(&gpio->GPBDAT) & ((1<<5) | (1<<6) | (1<<7) | (1<<8))))
		   {
			   printf("Restart now...\n");
			   run_command("reset", 0);
		   }
		   udelay(10000);
	   }
	   return 0;
}

typedef struct nand_cmd
{
	char nand_erase[256] ;
	char nand_write[256] ;
}BURN_CMD;

BURN_CMD cmd;

typedef struct check_header
{
	unsigned long magic;
	char type[4];
	int model;
	unsigned long load_addr;
	unsigned long burn_addr;
	unsigned long size;
	unsigned long CRC32;
}CHACK_HEADER_t;

CHACK_HEADER_t img;

static int check_header(ulong *ram_addr)
{
	int ret = 0;
	unsigned int type   = 0;

	img.burn_addr = ntohl(*(ulong*)((ulong)ram_addr + 4 * 5));
	img.size  = ntohl(*(ulong*)((ulong)ram_addr + 4 * 3));
	img.load_addr = ntohl(*(ulong*)((ulong)ram_addr + 4 * 4)) + 0x40;  //addr + 40;

	printf("          To check file              \n\n");
	printf("        <   Basic  data >            \n");
	printf("*************************************\n");
	printf("     Boot file name : filename       \n");
	printf("flash_address  = 0x%08lx             \n", img.burn_addr);
	printf("real file size = 0x%08lx             \n", img.size);
	printf("real file addr = 0x%08lx             \n", img.load_addr);
	printf("*************************************\n\n");

	printf("Magic number ............");
	img.magic = ntohl(*(ulong*)((ulong)ram_addr + 4 * 0));
	debug("img.magic = 0x%08lx", img.magic);
	if ( IH_MAGIC != img.magic )
	{
		printf("[ ERROR ]\nERROR.");
		return -1;
	}
	printf("[  %s   ]\n","OK");


	printf("CRC32 number ............");
	img.CRC32 = ntohl(*(ulong*)((ulong)ram_addr + 4 * 6));
	debug("img.CRC32 = 0x%08lx", img.CRC32);
	if (crc32(0, (unsigned char *)img.load_addr, img.size) != img.CRC32)
	{
		debug("crc32(0, img.load_addr, img.size) = 0x%08lx", crc32(0, img.load_addr, img.size));
		printf("[ ERROR ]\nERROR.");
		return -1;
	}
	printf("[  OK   ]\n");


	printf("Header Type  ............");
	type = ntohl(*(ulong*)((ulong)ram_addr + 4 * 8));
	switch (type)
	{
		case 0X424F4F54: /*bootloader*/
			strncpy(img.type, "BOOT", 4);
			sprintf( cmd.nand_erase, "nand erase %lx 40000", img.burn_addr);
			sprintf( cmd.nand_write, "nand write %lx %lx 40000", img.load_addr, img.burn_addr);
			led_ctl_num = LED1_ON;
			break;

		case 0X4B45524E:  /*kernel*/
			strncpy(img.type,"KERN",4);
			sprintf( cmd.nand_erase, "nand erase %lx 4ff800", img.burn_addr);
			sprintf( cmd.nand_write, "nand write %lx %lx 4c0000", img.load_addr, img.burn_addr);
			led_ctl_num = LED2_ON;
			break;

		case 0X524F4F54: /*rootfs*/
			strncpy(img.type,"ROOT",4);
			sprintf( cmd.nand_erase, "nand erase %lx 2000000", img.burn_addr);
			sprintf( cmd.nand_write, "nand write.yaffs %lx %lx %lx", img.load_addr, img.burn_addr, img.size);
			led_ctl_num = LED3_ON;
			break;

		default:
			printf("[ ERROR ]\nERROR.");
			return -1;
	}
	debug("img.type  = %s", img.type);
	printf("[ %s  ]\n", img.type);

	return ret;
}



static int all_update(void)
{
	int ret;
    run_command ("tftpserver", 0);
    if (0 != check_header ((ulong *)load_addr))
    {
    	printf("check_header error!\n");
    	return -1;
    }
    printf("cmd.nand_erase = %s\n", cmd.nand_erase);
    printf("cmd.nand_write = %s\n", cmd.nand_write);
    run_command (cmd.nand_erase, 0);
    ret = run_command (cmd.nand_write, 0);

    return ret;
}

static void led_ctl(unsigned char no)
{
	switch (no)
	{
		case ALL_LED_ON: /*all light*/
			red_LED_on(); // turn off all led
			green_LED_on();
			yellow_LED_on();
			blue_LED_on();
			break;

		case LED1_ON:
			red_LED_on();
			break;

		case LED2_ON:
			green_LED_on();
			break;

		case LED3_ON:
			yellow_LED_on();
			break;

		case LED4_ON:
			blue_LED_on();
			break;

		case ALL_LED_OFF:
			red_LED_off();
			green_LED_off();
			yellow_LED_off();
			blue_LED_off();
			break;

		default:
			printf("led_ctl: led_num error!!");
			break;
	}
}

U_BOOT_CMD(update, CONFIG_SYS_MAXARGS, 1, do_update, "usage info",  "help info");
