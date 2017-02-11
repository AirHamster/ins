
//void i2c1_read(uint8_t, uint8_t, uint8_t, uint8_t);
void tim_setup(void)
{

	/* Reset TIM1 peripheral to defaults. */
	rcc_periph_reset_pulse(RST_TIM1);
	timer_set_period(TIM1, 60000);
	

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
	timer_set_prescaler(TIM1, 0);

	/* Disable preload. */
	timer_disable_preload(TIM1);
	timer_continuous_mode(TIM1);
	timer_enable_oc_preload(TIM1,TIM_OC1);

	/* Set the initual output compare value for OC1. */
	timer_set_oc_value(TIM1, TIM_OC1, compare_time); 

	/* Enable TIM1 interrupt. */
	nvic_enable_irq(NVIC_TIM1_CC_IRQ);
	nvic_enable_irq(NVIC_TIM1_UP_IRQ);

	/*Enable timer 1 overflow and compare int */
	timer_enable_irq(TIM1, (TIM_DIER_UIE));
	timer_enable_irq(TIM1, (TIM_DIER_CC1IE));
	/* Counter enable. */
	timer_enable_counter(TIM1);

}

void tim1_up_isr(void)
{
	//i2c1_read(MPU_ADDRESS, MPU_WHO_AM_I, &temp, 1);	
	/* Clear update interrupt flag. */
	timer_clear_flag(TIM1, TIM_SR_UIF);

	//usart_send_blocking(USART1, I2C1_SR1);
	/* Toggle LED to indicate compare event. */
	gpio_set(LED1_PORT, LED1_PIN);
}
void tim1_cc_isr (void)
{
	/* Clear compare interrupt flag. */
	timer_clear_flag(TIM1, TIM_SR_CC1IF);
	//usart_send(USART1, 't');		
		
	gpio_clear(LED1_PORT, LED1_PIN);
	
		
	if (upcount ==1){
	compare_time += COMPARE_STEP;
	}else{
	compare_time -= COMPARE_STEP;
	}

	if (compare_time == 59000){
		upcount = 0;
	}
	if (compare_time == 0){
		upcount = 1;
	}
	timer_set_oc_value(TIM1, TIM_OC1, compare_time); 
	//usart_send(USART1, TIM1_CCR1);
}
