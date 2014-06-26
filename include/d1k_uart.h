/********************************************************************
d1k_uart.h

Copyright (c) 2014, Jonathan Nutzmann

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
********************************************************************/

#include "d1k.h"
#include "stm32f4xx_usart.h"

void d1k_uart_init( uint32 baudrate );

void d1k_uart_puts( char* message, size_t len );
void d1k_uart_put( char byte );
char d1k_uart_receive( void );
