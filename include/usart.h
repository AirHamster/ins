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
	uint8_t tmp;
	
	 //Check if we were called because of RXNE. 
	if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) &&
	    ((USART_SR(USART1) & USART_SR_RXNE) != 0)) {

		/* Indicate that we got data. */
		gpio_toggle(GPIOC, LED1_PIN);
		i2c1_read(MPU_ADDRESS, MPU_WHO_AM_I, &temp2, 1);
		/* Retrieve the data from the peripheral. */
		tmp = usart_recv(USART1);
		usart_send_32(USART1, &temp, 1);
	}

	 //Check if we were called because of TXE. 
	if (((USART_CR1(USART1) & USART_CR1_TXEIE) != 0) &&
	    ((USART_SR(USART1) & USART_SR_TXE) != 0)) {
		
		if (usart1.lenth-- != 0){
			usart_send(USART1, *usart1.data_pointer++);
		}else{
		 	//Disable the TXE interrupt as we don't need it anymore. 
			USART_CR1(USART1) &= ~USART_CR1_TXEIE;
			usart1.busy = 0;
		}
	}
}

void usart_send_32(uint32_t USART, uint32_t *data, uint8_t lenth)
{
	while (usart1.busy);
	usart1.busy = 1;	
	//Divide 32bit to 8bit
	usart1.data1 = (*data >> 24) & 0xff;
	usart1.data2 = (*data >> 16) & 0xff;
	usart1.data3 = (*data >> 8) & 0xff;
	usart1.data4 = (*data) & 0xff;
	usart1.lenth = lenth * 4;
	usart1.data_pointer = &usart1.data1;	//
	usart_send_blocking(USART, *usart1.data_pointer++);
	usart1.lenth--;
	//Enable TxE interrupt
	USART_CR1(USART) |= USART_CR1_TXEIE;
}
