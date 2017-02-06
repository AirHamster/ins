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
	usart_set_baudrate(USART1, 230400);
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

void usart1_isr(void)

{
	uint8_t data1 = (DEBUGVAL >> 24) & 0xff;
	uint8_t data2 = (DEBUGVAL >> 16) & 0xff;
	uint8_t data3 = (DEBUGVAL >> 8) & 0xff;
	uint8_t data4 = (DEBUGVAL) & 0xff;
	uint8_t tmp;
	
	if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) &&
	    ((USART_SR(USART1) & USART_SR_RXNE) != 0)) {

		/* Indicate that we got data. */
		gpio_toggle(GPIOC, LED1_PIN);

		/* Retrieve the data from the peripheral. */
		tmp = usart_recv(USART1);
		usart_send_blocking(USART1, data1);
		usart_send_blocking(USART1, data2);
		usart_send_blocking(USART1, data3);
		usart_send_blocking(USART1, data4);
		 /*Disable the TXE interrupt as we don't need it anymore. */
		/*USART_CR1(USART1) &= ~USART_CR1_TXEIE;*/
		tmp = usart_recv(USART1);
	}

	 /*Check if we were called because of TXE. */
	/*if (((USART_CR1(USART1) & USART_CR1_TXEIE) != 0) &&*/
	    /*((USART_SR(USART1) & USART_SR_TXE) != 0)) {*/
		
		/*usart_send_second(data2);*/
		/*[>USART_CR1(USART1) &= ~USART_CR1_TXEIE;<]*/
		/*[> Indicate that we are sending out data. <]*/
		/*// gpio_toggle(GPIOA, GPIO7);*/

		/*[> Put data into the transmit register. <]*/
		/*[>usart_send(USART1, 0xffff);<]*/
		/*[>usart_send_blocking(USART1, (TIM1_CR1));<]*/
		/*[>usart_send(USART1, (TIM1_CR1>>16));<]*/
		/*[>usart_send_blocking(USART1, (data2));<]*/
		/*[>usart_send(USART1, (data1));<]*/
		/*[> Disable the TXE interrupt as we don't need it anymore. <]*/
		/*[>USART_CR1(USART1) &= ~USART_CR1_TXEIE;<]*/
	/*}*/
}

void usart_send_first(uint16_t datatosend)
{
	usart_send(USART1, datatosend);
	/* Enable transmit interrupt so it sends back the data. */
	USART_CR1(USART1) |= USART_CR1_TXEIE;
}	

void usart_send_second(uint16_t datatosend)
{
	USART_CR1(USART1) &= ~USART_CR1_TXEIE;
	usart_send(USART1, datatosend);
}
