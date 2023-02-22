/* ###################################################################
**     Filename    : main.c
**     Project     : Mini_Project_UART
**     Processor   : MK64FN1M0VLL12
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2022-11-22, 13:52, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "Pins1.h"
#include "SM1.h"
#include "CsIO1.h"
#include "IO1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"
/* User includes (#include below this line is not maintained by Processor Expert) */
#include "MK64F12.h"
#include <stdint.h>
#include <stdio.h>

// Global Static Variables
const unsigned long Delay = 0x10000; 	  // Delay Value
const int LEDsequence[8] = {0x000004, 0x000008, 0x000200, 0x000400, 0x000800, 0x040000, 0x080000, 0x100000};
static unsigned char currSeq = 0;		  // Used to indicate where in the array LEDsequence
static unsigned char directionSwitch = 0; // Used to change the direction of LED's shifting up or down
static unsigned char playerAScore = 0;	  // Keeps track of player A's score
static unsigned char playerBScore = 0;    // Keeps track of player B's score
unsigned char write[512];
/*lint -save  -e970 Disable MISRA rule (6.3) checking. */

void software_delay(unsigned long delay)
{
	while (delay > 0) delay--;
}
// LED_BitShifter function is controls shifting the GPIO Pins on port D
// left or right to light up the 8 LEDS for game play. Shifts up to 8, then
// cycles back down to 1 and back and forth.
void LED_BitShifter(void)
{
	if(currSeq == 7)
	{
		directionSwitch = 1;
	}

	else if(currSeq == 0)
	{
		directionSwitch = 0;
	}

	if(directionSwitch == 0)
		GPIOB_PDOR = LEDsequence[++currSeq];
	else
		GPIOB_PDOR = LEDsequence[--currSeq];
}

// ButtonPressCheck checks to see if either button is pressed and
// the LED is in the correct location.
// LED will flash 5 times when timed correctly and will add
// 1 point to respective players score
void ButtonPressCheck(void)
{
	unsigned char Abutton = GPIOA_PDIR & 2; // Port A Pin 1
	unsigned char Bbutton = GPIOA_PDIR & 4; // Port A Pin 2

	if(Abutton && currSeq == 7)
	{
		playerAScore++;
		for(int i = 0; i < 5; ++i)
		{
			GPIOB_PDOR = 0x00;
			software_delay(Delay << 1);
			GPIOB_PDOR = LEDsequence[currSeq];
			software_delay(Delay << 1);
		}
	}
	else if (Bbutton && currSeq == 0)
	{
		playerBScore++;
		for(int i = 0; i < 5; ++i)
		{
			GPIOB_PDOR = 0x00;
			software_delay(Delay << 1);
			GPIOB_PDOR = LEDsequence[currSeq];
			software_delay(Delay << 1);
		}
	}
}

int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  // Configure Clock Gating for Ports A and B;
  	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK; /*Enable Port A Clock Gate Control*/
  	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK; /*Enable Port B Clock Gate Control*/

  	/*Port A will be used for button input*/
  	// Configure Port A pins 1 and 2 for GPIO
  	PORTA_GPCLR = 0x00060100;
  	// Configure Port A pins 1 and 2 for INPUT
  	GPIOA_PDDR = 0x00000000;

  	/*Port B will be used for LED output*/
  	// Configure Port B pins 2,3,9,8,10,11 for GPIO
  	PORTB_GPCLR = 0x0E0C0100;
  	// Configure Port B pins 18, 19,20 for GPIO
  	PORTB_GPCHR = 0x001C0100;
  	// Configure Port B Pins 2 and 3 for OUTPUT;
  	GPIOB_PDDR = 0x001C0E0C;
  	// Setting LED's To initial starting position
  	GPIOB_PDOR = LEDsequence[currSeq];

  	int len;
  	LDD_TDeviceData *SM1_DeviceData;
  	SM1_DeviceData = SM1_Init(NULL);

  while(1)
  	{
  		LED_BitShifter();
  		software_delay(Delay);
  		ButtonPressCheck();
  		software_delay(Delay);
  		if(playerAScore == 7 || playerBScore == 7)
  		{
  			len = sprintf(write,"WINNER!!!\n");
  			SM1_SendBlock(SM1_DeviceData, &write, len);
  			software_delay(Delay);
  			for(int i = 0; i < 5; ++i)
  			{
  				software_delay(Delay << 4);
  			}
  			playerAScore = 0; playerBScore = 0;
  		}
  		else
  			len = sprintf(write,"A:%d    B:%d\n", playerAScore, playerBScore);

  		SM1_SendBlock(SM1_DeviceData, &write, len);
  		software_delay(Delay);
  	}
  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
