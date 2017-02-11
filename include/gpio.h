void gpio_setup(void)
{

	/* Enable led as output */
	gpio_set_mode(LED1_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL, LED1_PIN);
	//Enable i2c1 SDA and SCL pins
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO_I2C2_SCL | GPIO_I2C2_SDA);

	/* Toggle LED to indicate compare event. */
	gpio_clear(LED1_PORT, LED1_PIN);
}
