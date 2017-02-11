/*
 * Some introduction here
 */

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/usart.h>
#include "include/defines.h"
#include "include/varables.h"
void clock_setup(void);
void gpio_setup(void);
void tim_setup(void);
void usart_send_first(uint16_t);
void usart_send_second(uint16_t);

void usart_send_32(uint32_t , uint32_t * , uint8_t );
void i2c1_setup(void);
void i2c1_write(uint8_t , uint8_t , uint8_t * , uint8_t );
void i2c1_read(uint8_t , uint8_t , uint8_t * , uint8_t );
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
	/*Enable clocks for I2C*/
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_USART1);

	/* Enable TIM1 clock. */
	rcc_periph_clock_enable(RCC_TIM1);

	/*Enable I2C1 clock.*/
	rcc_periph_clock_enable(RCC_I2C1);
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
