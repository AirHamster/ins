void i2c1_ev_isr(void)
{
	usart_send(USART1, 'I');
	/* If writing proceed*/
	if (i2c.w== 1){
	/*Start send*/
	if ((I2C_SR1(I2C1) & I2C_SR1_SB) != 0){
		i2c_send_data(I2C1, i2c.address);
		return;
		}
	 /*When address send, send register addr*/
	if ((I2C_SR1(I2C1) & I2C_SR1_ADDR) != 0){
		I2C_SR2(I2C1);
		i2c_send_data(I2C1, (i2c.reg_addr & 0xfe));	//
		//i2c_enable_ack(I2C1);
		return;
		}
	/*Send data while lenth is valid*/
	if ((I2C_SR1(I2C1) & I2C_SR1_BTF) != 0){
		i2c_send_data(I2C1, *i2c.data_pointer);
		i2c.data_pointer++;
		//i2c_enable_ack(I2C1);
		if (i2c.lenth-- == 0){
			i2c_send_stop(I2C1);
			i2c.busy = 0;
			return;
			}
		}
	}
	/*If reading needed	*/
	else if (i2c.r == 1){
	if (i2c.rs == 0){

	/*Start send*/
	if ((I2C_SR1(I2C1) & I2C_SR1_SB) != 0){
		i2c_send_data(I2C1, (i2c.address & 0xfe));
		usart_send(USART1, 's');
		return;
		}
	 
	 /*When address send, send register addr*/
	if ((I2C_SR1(I2C1) & I2C_SR1_ADDR) != 0){
		I2C_SR2(I2C1);
		i2c_send_data(I2C1, i2c.reg_addr);
		return;
		}
	/*When reg addr send, repeat start	*/
	if ((I2C_SR1(I2C1) & I2C_SR1_BTF) != 0){
		i2c_send_start(I2C1);
		i2c.rs = 1;	//repeated start send
		return;
		}
	}
	/*We set i2c.rs, so now can read data from i2c*/
	else{
		if((I2C_SR1(I2C1) & I2C_SR1_SB) != 0){
			i2c_send_data(I2C1, (i2c.address | 0x01));
			return;
			}
		if ((I2C_SR1(I2C1) & I2C_SR1_ADDR) != 0){
			i2c_enable_ack(I2C1);
			return;
			}
		if ((I2C_SR1(I2C1) & I2C_SR1_BTF) != 0){
			*i2c.data_pointer = i2c_get_data(I2C1);
			i2c.data_pointer++;
			
			if (--i2c.lenth == 1){
				i2c_nack_current(I2C1);
				return;
				}
			}	
		if ((I2C_SR1(I2C1) & I2C_SR1_RxNE) !=0) {
			i2c_send_stop(I2C1);
			i2c.busy = 0;
			}	
	}
}
}

void i2c1_er_isr(void)
{
	/*TODO errors handling*/
	usart_send(USART1, 'e');
	
}
void i2c1_setup(void)
{
	i2c_peripheral_disable(I2C1);
	//I2c on APB1 - 36MHz is maximum frequency
	usart_send_blocking(USART1, I2C1_CR2);
	i2c_set_clock_frequency(I2C1, I2C_CR2_FREQ_36MHZ); 
	i2c_set_standard_mode(I2C1);
	i2c_enable_interrupt(I2C1, I2C_CR2_ITERREN);
	i2c_set_ccr(I2C1, 360);	//36MHz/100kHz
	i2c_set_trise(I2C1, 36);
	nvic_enable_irq(NVIC_I2C1_EV_IRQ);
	nvic_enable_irq(NVIC_I2C1_ER_IRQ);
	i2c_enable_interrupt(I2C1, I2C_CR2_ITEVTEN | I2C_CR2_ITEVTEN);
	usart_send_blocking(USART1, I2C1_CR2>>8);

	i2c_peripheral_enable(I2C1);
}

void i2c1_write(uint8_t address, uint8_t reg_addr, uint8_t *data, uint8_t lenth)
{
	i2c.busy = 1;
	i2c.r = 0;
	i2c.w = 1;
	i2c.reg_addr = reg_addr;
	i2c.address = address;
	i2c.data_pointer = data;
	i2c.lenth = lenth;
	i2c_send_start(I2C1);
}

void i2c1_read(uint8_t address, uint8_t reg_addr, uint8_t *data, uint8_t lenth)
{
	i2c.busy = 1;
	i2c.r = 1;
	i2c.w = 0;
	i2c.reg_addr = reg_addr;
	i2c.address = address;
	i2c.data_pointer = data;
	i2c.lenth = lenth;
	i2c_send_start(I2C1);
}
