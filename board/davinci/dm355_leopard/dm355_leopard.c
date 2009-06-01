/*
 *
 * Copyright (C) 2008 Texas Instruments.
 *
 * ----------------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ----------------------------------------------------------------------------
 *
 */
#include <common.h>
#include <i2c.h>
#include <asm/arch/hardware.h>

/* GIO register */
#define	GIO_BINTEN		              0x01C67008		/* GPIO Interrupt Per-Bank Enable Register */
#define GIO_DIR01			       0x01C67010		
#define GIO_OUT_DATA01			0x01C67014		
#define GIO_SET_DATA01			0x01C67018		
#define GIO_CLR_DATA01			0x01C6701C		
#define	GIO_SET_RIS_TRIG01	0x01C67024		    
#define	GIO_SET_FAL_TRIG01	0x01C6702c		
#define	GIO_A2CR		              0x01e10014

#define GIO_DIR23			       0x01C67038		
#define GIO_OUT_DATA23			0x01C6703c		
#define GIO_SET_DATA23			0x01C67040		
#define GIO_CLR_DATA23			0x01C67044		

#define GIO_DIR45			       (0x01C67060)		
#define GIO_OUT_DATA45			(0x01C67064)		
#define GIO_SET_DATA45			(0x01C67068)		
#define GIO_CLR_DATA45			(0x01C6706C)

#define GIO_DIR06			      (0x01C67088)		
#define GIO_OUT_DATA06			(0x01C6708C)		
#define GIO_SET_DATA06			(0x01C67090)		
#define GIO_CLR_DATA06			(0x01C67094)

extern int timer_init(void);

/*******************************************
 Routine: board_init
 Description:  Board Initialization routine
*******************************************/
int board_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	
	/* arch number of DaVinci DM355 */
	gd->bd->bi_arch_number = 1381;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = LINUX_BOOT_PARAM_ADDR;

#if 1	
#define PINMUX3	    0x01C4000C
		*(volatile unsigned int *)PINMUX3 &= 0XF8FFFFFF; // GIO9 & 10 are IO
		
 	      /* Interrupt set GIO9	*/
	      *((volatile unsigned int *) GIO_BINTEN) |=0x01; //bank 0	
	      /* set GIO9input  */
	       *((volatile unsigned int *) GIO_DIR01) |=(1<<9);	 
	      /* Both edge tigger GIO9 */
	       *((volatile unsigned int *) GIO_SET_RIS_TRIG01) |=(1<<9);          	

		*((volatile unsigned int *) GIO_DIR01) &= ~(1<<5);	
		*((volatile unsigned int *) GIO_SET_DATA01) &= ~(1<<5); // output Low
	
			/* set GIO10 output  */
	//		printf("pull up gio10\n");
	       *((volatile unsigned int *) GIO_DIR01) &= ~(1<<10);	
		   *((volatile unsigned int *) GIO_SET_DATA01) |= (1<<10); // output Hi


	      	/* set GIO32 output	*/
	      *((volatile unsigned int *) GIO_DIR23) &= ~(1<<0);  
	      *((volatile unsigned int *) GIO_SET_DATA23) |= (1<<0); // output Hi
	      	/* set GIO102 output	*/	   
#define PINMUX0     0x01C40000
        /* Enable UART1 MUX Lines */
	*(volatile unsigned int *)PINMUX0 &= ~3;	
	*((volatile unsigned int *) GIO_DIR06) &= ~(1<<6);     
	*((volatile unsigned int *) GIO_SET_DATA06) |= (1<<6);	 						
#endif        
	/* Set the Bus Priority Register to appropriate value */
	REG(VBPR) = 0x20;
	
	timer_init();
	return 0;
}


/******************************
 Routine: misc_init_r
 Description:  Misc. init
******************************/
int misc_init_r (void)
{
	char temp[20], *env=0;
	int i = 0;
	int clk = 0;

	clk = ((REG(PLL2_PLLM) + 1) * 24) / ((REG(PLL2_PREDIV) & 0x1F) + 1); 

	printf ("ARM Clock :- %dMHz\n", ( ( ((REG(PLL1_PLLM) + 1)*24 )/(2*(7 + 1)*((REG(SYSTEM_MISC) & 0x2)?2:1 )))) );
	printf ("DDR Clock :- %dMHz\n", (clk/2));

	/* set GIO5 output, imager reset  */

	*((volatile unsigned int *) GIO_SET_DATA01) |= (1<<5); // output High

	return (0);
}

/******************************
 Routine: dram_init
 Description:  Memory Info
******************************/
int dram_init (void)
{
        DECLARE_GLOBAL_DATA_PTR;

	      gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	      gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

        return 0;
}


