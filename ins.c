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

#include <libopencm3/stm32/usart.h>
#define COMPARE_STEP 50
uint8_t	upcount = 1;
uint16_t compare_time = 30000;

#define LED1_PORT GPIOC
#define LED1_PIN GPIO13
void clock_setup(void);
void gpio_setup(void);
void tim_setup(void);
/*void usart_setup(void);*/



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
}

static void usart_setup(void)
{
	/* Enable the USART1 interrupt. */
	nvic_enable_irq(NVIC_USART1_IRQ);

	/* Setup GPIO pin GPIO_USART1_RE_TX on GPIO port B for transmit. */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

	/* Setup GPIO pin GPIO_USART1_RE_RX on GPIO port B for receive. */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
		      GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);

	/* Setup UART parameters. */
	usart_set_baudrate(USART1, 9600);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART1, USART_MODE_TX_RX);

	/* Enable USART1 Receive interrupt. */
	USART_CR1(USART1) |= USART_CR1_RXNEIE;

	/* Finally enable the USART. */
	usart_enable(USART1);
}
void gpio_setup(void)
{

	/* Enable led as output */
	gpio_set_mode(LED1_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL, LED1_PIN);
	/*gpio_set(LED1_PORT, LED1_PIN);*/
	/* Toggle LED to indicate compare event. */
	/*gpio_clear(LED1_PORT, LED1_PIN);*/
}

void tim_setup(void)
{

	/* Reset TIM1 peripheral to defaults. */
	rcc_periph_reset_pulse(RST_TIM1);
	timer_set_period(TIM1, 60000);
	/* Enable TIM1 interrupt. */
	/*nvic_enable_irq(NVIC_TIM1_CC_IRQ);*/

	/*nvic_enable_irq(NVIC_TIM1_UP_IRQ);	*/
	

	/* Timer global mode:
	 * - No divider
	 * - Alignment edge
	 * - Direction up
	 * (These are actually default values after reset above, so this call
	 * is strictly unnecessary, but demos the api for alternative settings)
	 */
	timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT,
		TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	/*
	 * Please take note that the clock source for STM32 timers
	 * might not be the raw APB1/APB2 clocks.  In various conditions they
	 * are doubled.  See the Reference Manual for full details!
	 * In our case, TIM2 on APB1 is running at double frequency, so this
	 * sets the prescaler to have the timer run at 5kHz
	 */
	timer_set_prescaler(TIM1, 128);

	/* Disable preload. */
	timer_disable_preload(TIM1);
	timer_continuous_mode(TIM1);
	timer_enable_oc_preload(TIM1,TIM_OC1);

	/* Set the initual output compare value for OC1. */
	timer_set_oc_value(TIM1, TIM_OC1, 30000); 

	/* Enable TIM1 interrupt. */
	/*nvic_enable_irq(NVIC_TIM1_CC_IRQ);*/
	nvic_enable_irq(NVIC_TIM1_UP_IRQ);
	/*Enable timer 0 overflow intt */
	timer_enable_irq(TIM1, (TIM_DIER_UIE));
	/*timer_enable_irq(TIM1, (TIM_DIER_CC1IE));*/
	/* Counter enable. */
	timer_enable_counter(TIM1);

	/* Enable Channel 1 compare interrupt to recalculate compare values */
	/*
	 *timer_enable_irq(TIM1, TIM_DIER_CC1IE);
	 */
}

void tim1_up_isr(void)
{
	
	/* Clear update interrupt flag. */
	timer_clear_flag(TIM1, TIM_SR_UIF);

	/* Toggle LED to indicate compare event. */
	gpio_toggle(LED1_PORT, LED1_PIN);
}
void tim1_trg_com_isr(void)
{

		/* Clear compare interrupt flag. */
		timer_clear_flag(TIM1, TIM_SR_CC1IF);
		
		
		/*gpio_toggle(LED1_PORT, LED1_PIN);*/
		/* Set the initual output compare value for OC1. */
		/*
		 *timer_set_oc_value(TIM1, TIM_OC1, 0); 
		 */

		/* Calculate and set the next compare value. */
	/*	if (upcount ==1){
		compare_time += COMPARE_STEP;
		}else{
		compare_time -= COMPARE_STEP;
		}

		if (compare_time == 1200){
			upcount = 0;
		}
		if (compare_time == 0){
			upcount = 1;
		}*/
		//
		/*timer_set_oc_value(TIM1, TIM_OC1, compare_time);*/

		/* Toggle LED to indicate compare event. */
		/*gpio_clear(LED1_PORT, LED1_PIN);*/
}

void usart1_isr(void)
{
	static uint8_t data = 'A';

	/* Check if we were called because of RXNE. */
	if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) &&
	    ((USART_SR(USART1) & USART_SR_RXNE) != 0)) {

		/* Indicate that we got data. */
		gpio_toggle(GPIOC, LED1_PIN);

		/* Retrieve the data from the peripheral. */
		data = usart_recv(USART1);

		/* Enable transmit interrupt so it sends back the data. */
		USART_CR1(USART1) |= USART_CR1_TXEIE;
	}

	/* Check if we were called because of TXE. */
	if (((USART_CR1(USART1) & USART_CR1_TXEIE) != 0) &&
	    ((USART_SR(USART1) & USART_SR_TXE) != 0)) {

		/* Indicate that we are sending out data. */
		// gpio_toggle(GPIOA, GPIO7);

		/* Put data into the transmit register. */
		usart_send(USART1, 0xffff);

		/* Disable the TXE interrupt as we don't need it anymore. */
		USART_CR1(USART1) &= ~USART_CR1_TXEIE;
	}
}
int main(void)
{
	clock_setup();
	gpio_setup();
	tim_setup();
	usart_setup();
	
	/*gpio_toggle(LED1_PORT, LED1_PIN);*/
	while (1) {
		;
	}

	return 0;
}
