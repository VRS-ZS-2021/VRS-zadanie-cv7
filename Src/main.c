/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"
#include "stdio.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);


/* Function processing DMA Rx data. Counts how many capital and small letters are in sentence.
 * Result is supposed to be stored in global variable of type "letter_count_" that is defined in "main.h"
 *
 * @param1 - received sign
 */
void proccesDmaData(uint8_t sign);


/* Space for your global variables. */

	// type your global variables here:
char actual_data_buffer[256];
char data_to_send[100];
uint8_t allow = 0, lowercase_char = 0, uppercase_char = 0,calculate_sign = 0;

int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();

  /* Space for your local variables, callback registration ...*/

  	  //type your code here:
  USART2_RegisterCallback(proccesDmaData);

  //clearing buffers
  strcpy(actual_data_buffer,"");

  while (1)
  {
	  /* Periodic transmission of information about DMA Rx buffer state.
	   * Transmission frequency - 5Hz. (Staci raz za sekundu)
	   * Message format - "Buffer capacity: %d bytes, occupied memory: %d bytes, load [in %]: %f%"
	   * Example message (what I wish to see in terminal) - Buffer capacity: 1000 bytes, occupied memory: 231 bytes, load [in %]: 23.1%
	   */
  	  	  	  //type your code here:


	  //Sending info about buffer capacity status
	  uint16_t buffer_state = getBufferState();
	  float buffer_percentage = (float) (buffer_state) / (float) (DMA_USART2_BUFFER_SIZE)*100;
	  sprintf(data_to_send, "Buffer capacity: %d bytes, occupied memory: %d bytes, load [in %%]:%.2f%%\r\n", DMA_USART2_BUFFER_SIZE, buffer_state, buffer_percentage);

	  USART2_PutBuffer((uint8_t *) data_to_send, strlen(data_to_send));
	  LL_mDelay(1000);
  }
  /* USER CODE END 3 */
}


void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  
  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
}

/*
 * Implementation of function processing data received via USART.
 */
void proccesDmaData(uint8_t sign)
{
	/* Process received data */

		// type your algorithm here:
	char data_number_to_send[100];
	if(sign == '\r') return; //filtering endline character from PuTTY

	//Appending new character to "actual_data_buffer"
//	char tmp_string[2];
//	tmp_string[0] = sign;
//	tmp_string[1] = '\0';
//	strcat(actual_data_buffer,tmp_string);
//	actual_data_buffer[strlen(actual_data_buffer)] = '\0';

	if(sign == '#' && allow == 0){ //zapneme povolenie pre citanie znakov, startovaci znak je prijaty len raz
		allow = 1;
		calculate_sign = 0;
		lowercase_char = 0;
		uppercase_char = 0;
	}
	if(sign == '$' && allow == 1){ // po prijati ukoncovacieho znaku vypneme povolenie a vypiseme data
		allow = 0;
		sprintf(data_number_to_send, "Number of lowercase characters: %d , Number of uppercase characters: %d\r\n", lowercase_char,uppercase_char);
		USART2_PutBuffer((uint8_t *) data_number_to_send, strlen(data_number_to_send));
	}
	if(calculate_sign > 35){//ak sme prekrocili pocet znakov o 35, prestaneme znaky ratat a zahodime data
		allow = 0;
		calculate_sign = 0;
		lowercase_char = 0;
		uppercase_char = 0;
	}
	if (allow == 1){//ratanie malych/velkych pismen
		calculate_sign++;
		if(sign >= 'a' && sign <= 'z'){
			lowercase_char++;
		}
		if(sign >= 'A' && sign <= 'Z'){
			uppercase_char++;
		}

	}

}


void Error_Handler(void)
{

}

#ifdef  USE_FULL_ASSERT

void assert_failed(char *file, uint32_t line)
{ 

}

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
