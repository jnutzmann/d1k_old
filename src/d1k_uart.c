/********************************************************************
d1k_uart.c

Copyright (c) 2014, Jonathan Nutzmann, Arlo Siemsen

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
********************************************************************/

#include "FreeRTOS.h"
#include "queue.h"
#include "d1k_uart.h"
#include "stm32f4xx_rcc.h"

static xQueueHandle uart_tx_queue;
static xQueueHandle uart_rx_queue;

#define uart_QUEUE_LENGTH  	32
#define uart_MAX_DELAY_MS 	10

void d1k_uart_init( uint32 baudrate )
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	USART_InitTypeDef usartInit;
	USART_StructInit(&usartInit);
	usartInit.USART_BaudRate = baudrate;
	usartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(USART3, &usartInit);

	// Init the NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART3, ENABLE);

	uart_tx_queue = xQueueCreate(uart_QUEUE_LENGTH, sizeof(uint8_t));
	uart_rx_queue = xQueueCreate(uart_QUEUE_LENGTH, sizeof(uint8_t));

	USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

void d1k_uart_puts( char* message, size_t len )
{
	for(int i = 0; i<len; i++)
	{
		d1k_uart_put(message[i]);
	}
}

void d1k_uart_put( char byte )
{
	xQueueSend(uart_tx_queue, &byte, uart_MAX_DELAY_MS / portTICK_RATE_MS);

	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

char d1k_uart_receive( void )
{
	char byte;

	xQueueReceive(uart_rx_queue, &byte, uart_MAX_DELAY_MS / portTICK_RATE_MS);

	return byte;
}

void USART3_IRQHandler( void )
{
	signed portBASE_TYPE higherPriTaskWoken = pdFALSE;

	if (USART_GetITStatus(USART3,USART_IT_RXNE))
	{
		uint8_t received_byte;
		received_byte = USART_ReceiveData(USART3);
		xQueueSendFromISR(uart_rx_queue, &received_byte, &higherPriTaskWoken);
	}

	if (USART_GetITStatus(USART3,USART_IT_TXE))
	{
		uint8_t byte_to_send;

		if(xQueueReceiveFromISR(uart_tx_queue, &byte_to_send, &higherPriTaskWoken))
		{
			//there is data to send
			USART_SendData(USART3, byte_to_send);
		}
		else
		{
			//no more data to send
			USART_ITConfig(USART3,USART_IT_TXE,DISABLE);
		}
	}

	portEND_SWITCHING_ISR(higherPriTaskWoken);
}


