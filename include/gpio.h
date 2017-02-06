void gpio_setup(void)
{

	/* Enable led as output */
	gpio_set_mode(LED1_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL, LED1_PIN);
	/* Toggle LED to indicate compare event. */
	gpio_clear(LED1_PORT, LED1_PIN);
}
