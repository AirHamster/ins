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

		/* Retrieve the data from the peripheral. */
		tmp = usart_recv(USART1);
		switch (tmp){
		case '1':		
			usart_send_32(USART1, &I2C1_CR1, 2);
		break;			
	
		case '2':			
			usart_send_32(USART1, &I2C1_CR2, 1);
		break;
		case '3':		
			usart_send_32(USART1, &I2C1_SR1, 2);
		break;
		case '4':	
			usart_send_32(USART1, &I2C1_SR2, 1);
		break;
		default:
			i2c1_read(MPU_ADDRESS, MPU_WHO_AM_I, &temp2, 1);
			usart_send_32(USART1, &I2C1_SR2, 1);
		break;
	}
	}
	 //Check if we were called because of TXE. 
	if (((USART_CR1(USART1) & USART_CR1_TXEIE) != 0) &&
	    ((USART_SR(USART1) & USART_SR_TXE) != 0)) {
		/*Check the count of non-sended words*/	
		if (usart1.lenth != 0){
			/*send bytes until it will be send*/
			if (usart1.byte_counter-- != 0){
				usart_send(USART1, *usart1.data_pointer++);
			}else{
				if(--usart1.lenth !=0){
					/*Reconfig usarts pointers and byte array*/
					usart1.byte_counter = 3;
					usart1.global_pointer++;
					usart1.data1 = (*usart1.global_pointer >> 24) & 0xff;
					usart1.data2 = (*usart1.global_pointer >> 16) & 0xff;
					usart1.data3 = (*usart1.global_pointer >> 8) & 0xff;
					usart1.data4 = (*usart1.global_pointer) & 0xff;
					usart1.data_pointer = &usart1.data1;
					usart_send(USART1, *usart1.data_pointer++);
				}else{
		 			//Disable the TXE interrupt as we don't need it anymore. 
					USART_CR1(USART1) &= ~USART_CR1_TXEIE;
					usart1.busy = 0;
				}
			}
		
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
	usart1.lenth = lenth;
	usart1.byte_counter = 4;
	usart1.global_pointer = data;
	usart1.data_pointer = &usart1.data1;	//
	usart_send_blocking(USART, *usart1.data_pointer++);
	usart1.byte_counter--;
	//Enable TxE interrupt
	USART_CR1(USART) |= USART_CR1_TXEIE;
}
