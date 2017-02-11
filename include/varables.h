struct iic
{
	uint8_t address;
	uint8_t reg_addr; 
	uint8_t lenth;
	uint8_t *data_pointer;
	unsigned int busy:1;
	unsigned int r:1;
	unsigned int w:1;
	unsigned int rs:1;
};
struct usart
{
	uint8_t *data_pointer;
	uint8_t data1;
	uint8_t data2;
	uint8_t data3;
	uint8_t data4;
	uint8_t lenth;
	uint8_t byte_number;
	unsigned int busy:1;
};
	struct iic i2c;
	struct usart usart1;
uint8_t	upcount = 1;
uint32_t temp = 0x12345678;
uint8_t temp2;
uint16_t compare_time = 30000;
