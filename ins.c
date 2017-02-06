/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2011 Piotr Esden-Tempski <piotr@esden.net>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/usart.h>

struct iic
{
	unsigned int busy:1;
	unsigned int r:1;
	unsigned int w:1;
	unsigned int rs:1;
	uint8_t address;
	uint8_t reg_addr; 
	uint8_t lenth;
	uint32_t *data_pointer;
};
	struct iic i2c;
#define MPU_ADDRESS 0x36 //0b11010010
#define MPU_WHO_AM_I 0x75
#define COMPARE_STEP 20 
uint8_t	upcount = 1;
uint8_t temp;
uint16_t compare_time = 30000;
#define LED1_PORT GPIOC
#define LED1_PIN GPIO13
#define DEBUGVAL TIM1_DIER 
void clock_setup(void);
void gpio_setup(void);
void tim_setup(void);
void usart_send_first(uint16_t);
void usart_send_second(uint16_t);

void i2c1_setup(void);
void i2c1_write(uint8_t , uint8_t , uint32_t * , uint8_t );
void i2c1_read(uint8_t , uint8_t , uint32_t * , uint8_t );
#include "include/gpio.h"
#include "include/usart.h"
#include "include/i2c.h"
#include "include/timers.h"



void clock_setup(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
	
	/* Enable GPIOA clock (for LED GPIOs). */
	rcc_periph_clock_enable(RCC_GPIOC);

	/* Enable clocks for GPIO port A (for GPIO_USART1_TX) and USART1. */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_USART1);

	/* Enable TIM1 clock. */
	rcc_periph_clock_enable(RCC_TIM1);

	/*Enable I2C1 clock.*/
	/*rcc_periph_clock_enable(RCC_I2C1);*/
}

int main(void)
{
	/*i2c.w = 1;*/
	clock_setup();
	gpio_setup();
	tim_setup();
	usart_setup();
	i2c1_setup();	
	
	while (1);

	return 0;
}
