/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include <stdbool.h>
#include "web.h"
#include <stdio.h>
#include <stdlib.h>

/* Private includes ----------------------------------------------------------*/

void SystemClock_Config(void);

void GPIO_Init(void);

void SPI_Init(void);

void ADC_Init(void);

void Wiz_Init(void);

void Check_Init(void);

void Transmit_Data(uint16_t data_size, uint8_t data[]);

void Receive_Data(uint16_t data_size, uint8_t dataRx[], uint8_t dataTx[]);

void Socket_Init(void);

void Socket_Connect(void);

void w5500_receive(void);

void w5500_send(uint8_t tosend[], uint16_t size);

uint8_t Button_State(void);

void POST_response(uint8_t output);

void Start_ADC(void);

/* Registers ------------------------------------------------------------------*/


/* GPIO (Base 0x4002 0000) */

uint32_t* PORTA_MODER = (uint32_t*) (0x40020000);

uint32_t* PORTA_SPEED = (uint32_t*) (0x40020008);

uint32_t* PORTA_PULL = (uint32_t*) (0x4002000C);

uint32_t* PORTB_MODER = (uint32_t*) (0x40020400);

uint32_t* PORTC_MODER = (uint32_t*) (0x40020800);

uint32_t* PORTC_PULL = (uint32_t*) (0x4002080C);

uint32_t* RCCAHB1_GPIO = (uint32_t*) (0x40023830); 	//GPIO clock enable

uint32_t* PORTA_AF = (uint32_t*) (0x40020020);		//GPIO alternate function r.


/* SPI (Base 0x4001 3000) */

uint32_t* APB2ENR = (uint32_t*) (0x40023844); 		//SPI clock (RCC registers, bus AHB1)

uint32_t* SPI_CR = (uint32_t*) (0x40013000);


/* Button (PC13) */

uint16_t* PORTC_IDR = (uint16_t*) (0x40020810);		//button state


/* ADC1 */

uint32_t* ADC_CR1 = (uint32_t*) (0x40012004);		//ADC_CR1 for selecting channels

uint32_t* ADC_CR2 = (uint32_t*) (0x40012008);		//ADC_CR2 for enabling and starting ADC, continuousness

uint32_t* ADC_SMPR1 = (uint32_t*) (0x4001200C);		//ADC_SMPR1

uint32_t* ADC_SQR3 = (uint32_t*) (0x40012034);		//CHANNEL SELECT


/* -----------------------------------------------------------------------------*/



uint8_t received[100];
uint8_t rcv[10];
uint16_t b = 0;
uint8_t a = 0;
uint8_t cnt = 0;

uint8_t ptr[2] = {0};

uint16_t* button = (uint16_t*)(0x40020810);
uint16_t* adc = (uint16_t*) (0x4001204C);;
uint8_t updated = 0;
uint8_t counter = 0;


char html_content_res[256]; // Allocate memory for the HTML content
uint16_t response_len = 0;
char* button_value;
char* adc_value[6];
uint16_t sum = 0;
uint16_t average_ADC = 0;


int main(void)
{
	HAL_Init();
	SystemClock_Config();
	GPIO_Init();
	SPI_Init();
	ADC_Init();

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_Delay(1);

	Wiz_Init();


	Socket_Init();

	//uint8_t tr[] = {0};

	////RESET det i inicializacija


	while (1)
	{

		Start_ADC();
		Socket_Connect();

	}

}

uint8_t Button_State(void){
	updated = (*button) >> 13;
	updated &= 1;
	return updated;
}

void Start_ADC(void){

	*ADC_CR2 |= 0x40000000; //starting
}

void POST_response(uint8_t output){

	button_value = (Button_State() == 1) ? "Not pressed" : "Pressed";
	if (counter == 8){
		average_ADC = (sum >> 3);
		counter = 0;
		sum = 0;
	}
	else{
		sum += *adc & 0x0FFF;
		counter++;
	}
	sprintf(adc_value, "%u", average_ADC);
	response_len = strlen("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"parameter1\": \"\", \"parameter2\": \"\"}") +
	               strlen(button_value) + strlen(adc_value);
	snprintf((char*)html_content_res, response_len + 1, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"parameter1\": \"%s\", \"parameter2\": \"%s\"}", button_value, adc_value);

	}


void w5500_receive(void){

	uint8_t tx[] = {0x00, 0x26, 0x08, 0x00, 0x00};
	uint8_t rec[2048] = {0};
	uint8_t rx[2] = {0};
	Receive_Data(2, rx, tx);

	if (rx[0] != 0x00 && rx[1] != 0x00){

		uint16_t len = rx[0] * 256 + rx[1];
		tx[3] = rx[0]; tx[4] = rx[1]; tx[1] = 0x28;

		Receive_Data(2, ptr, tx);

		tx[0] = ptr[0]; tx[1] = ptr[1]; tx[2] = 0x18;

		Receive_Data(len, rec, tx);

		tx[0] = 0; tx[1] = 0x28; tx[2] = 0x08;
		Receive_Data(2, rx, tx);
		uint16_t current_len = rx[0]*256 + rx[1];
		uint16_t final_len = current_len + len;
		tx[1] = 0x28; tx[2] = 0x0C; tx[3] = (final_len>>8); tx[4] = final_len % 256;
		Transmit_Data(5, tx);

		tx[1] = 0x01; tx[2] = 0x0C; tx[3] = 0x40;
		Transmit_Data(4, tx);

		uint8_t text1[] = "GET", text2[] = "POST";
		uint8_t i = 0, j = 0;
		while(i < 3){
			if (text1[i] != rec[i])
				break;
			i++;}

		if (i < 3){
			while(j < 4){
				if (text2[j] != rec[j])
					break;
				j++;}}

		uint8_t final[] = index;
		uint16_t l = 0;

		if (i == 3){
			l = strlen(final);
			w5500_send(final, l);
		}

		if (j == 4){
			POST_response(1);
			char* html_content = html_content_res;
			l = strlen(html_content);
			w5500_send(html_content, l);
		}

	}

}

void w5500_send(uint8_t tosend[], uint16_t len){

	uint8_t tx[5] = {0};
	uint8_t formatted[len+3];
	uint16_t j;
	for (j = 0; j < len; j++)
		formatted[j + 3] = tosend[j];
	uint8_t rx[2];

	uint16_t i = 0;

	while (i < len){

		tx[0] = 0; tx[1] = 0x20; tx[2] = 0x08;
		Receive_Data(2, rx, tx);
		uint16_t free = rx[0] * 256 + rx[1];

		rx[0] = 0; rx[1] = 0;
		tx[0] = 0; tx[1] = 0x22; tx[2] = 0x08;
		Receive_Data(2, rx, tx);
		formatted[0] = rx[0]; formatted[1] = rx[1]; formatted[2] = 0x14;

		uint16_t tmpl;
		if (free < len){
			Transmit_Data(free+3, formatted);
			i += free;
			tmpl = free;
		}

		else{
			Transmit_Data(len+3, formatted);
			i += len;
			tmpl = len;
		}

		tx[0] = 0; tx[1] = 0x24; tx[2] = 0x08;
		Receive_Data(2, rx, tx);
		uint16_t current_len = rx[0]*256 + rx[1];
		uint16_t final_len = current_len + tmpl;
		tx[2] = 0x0C; tx[3] = (final_len>>8); tx[4] = final_len % 256;
		Transmit_Data(5, tx);

		tx[1] = 0x01; tx[3] = 0x20;
		Transmit_Data(4, tx);

		uint16_t ptrrx[2] = {0};
		uint16_t ptrtx[2] = {1};

		while (ptrrx[0] != ptrtx[0] || ptrrx[1] != ptrtx[1]){
			tx[0] = 0; tx[1] = 0x22; tx[2] = 0x08;
			Receive_Data(2, rx, tx);
			ptrrx[0] = rx[0]; ptrrx[1] = rx[1];

			tx[0] = 0; tx[1] = 0x24; tx[2] = 0x08;
			Receive_Data(2, rx, tx);
			ptrtx[0] = rx[0]; ptrtx[1] = rx[1];
		}

	}

	tx[0] = 0; tx[1] = 0x01; tx[2] = 0x0C; tx[3] = 0x08;
	Transmit_Data(4, tx);

}

void Socket_Init(void){

	uint8_t transmit[5];
	uint8_t rec[1] = {0};
	transmit[1] = 0x03;
	transmit[2] = 0x08;
	Receive_Data(1, rec, transmit);

	while (rec[0] != 0x13){

		//tcp mode
		transmit[1] = 0x00;
		transmit[2] = 0x0C;
		transmit[3] = 0x01;
		Transmit_Data(4, transmit);
		uint8_t tx_data22[3];
		tx_data22[0] = 0x00;  // read operation
		tx_data22[1] = 0x00;
		tx_data22[2] = 0x08;
		Receive_Data(1, rec, tx_data22);

		//port
		transmit[1] = 0x04;
		transmit[2] = 0x0C;
		transmit[3] = 0x00;
		transmit[4] = 0x50;
//		transmit[1] = 0x04;
//		transmit[2] = 0x0C;
//		transmit[3] = 0x1f;
//		transmit[4] = 0x90;
		Transmit_Data(5, transmit);

		//open
		transmit[1] = 0x01;
		transmit[2] = 0x0C;
		transmit[3] = 0x01;
		Transmit_Data(4, transmit);

		//check if initialized
		transmit[1] = 0x03;
		transmit[2] = 0x08;
		Receive_Data(1, rec, transmit);


		if (rec[0] == 0){
			//close
			transmit[0] = 0x00;
			transmit[1] = 0x01;
			transmit[2] = 0x0C;
			transmit[3] = 0x10;
			Transmit_Data(4, transmit);
		}

	}

}

void Socket_Connect(void){

	uint8_t transmit[5] = {0};
	uint8_t rec[1] = {0};
	transmit[1] = 0x03;
	transmit[2] = 0x08;
	Receive_Data(1, rec, transmit);

	while (rec[0] != 0x14 && rec[0] != 0x17){

		//listen
		transmit[0] = 0x00;
		transmit[1] = 0x01;
		transmit[2] = 0x0C;
		transmit[3] = 0x02;
		Transmit_Data(4, transmit);

		//check if listening
		transmit[1] = 0x03;
		transmit[2] = 0x08;
		Receive_Data(1, rec, transmit);


		if (rec[0] == 0 || rec[0] == 0x1C){
			//close
			transmit[0] = 0x00;
			transmit[1] = 0x01;
			transmit[2] = 0x0C;
			transmit[3] = 0x10;
			Transmit_Data(4, transmit);
			Socket_Init();
		}

	}

	while (rec[0] != 0x17){
		transmit[1] = 0x03;
		transmit[2] = 0x08;
		Receive_Data(1, rec, transmit);

	}

	if (rec[0] == 0x17)
		w5500_receive();



}

void Transmit_Data(uint16_t data_size, uint8_t data[]){
	GPIOB->BSRR |= 1 << (6 + 16);
	for (int j = 0; j < data_size; j++){
		SPI1->DR = data[j];
		while(!(SPI1->SR & SPI_SR_TXE));
	}
	while(SPI1->SR & SPI_SR_BSY);
	GPIOB->BSRR |= 1 << 6;
}

void Receive_Data(uint16_t data_size, uint8_t dataRx[], uint8_t dataTx[]){

	uint16_t j = 0;

	if((SPI1->SR & SPI_SR_RXNE)!=0)
		a = SPI1->DR;

	GPIOB->BSRR |= 1 << (6 + 16);
	while (j < 3){
		while(!(SPI1->SR & SPI_SR_TXE));
		SPI1->DR = dataTx[j];
		while((SPI1->SR & SPI_SR_RXNE)==0);
		rcv[j++] = SPI1->DR;
	}
	while((SPI1->SR & SPI_SR_BSY));

	//a = SPI1->DR;

	j = 0;
	while (j < data_size)//(SPI1->SR & SPI_SR_RXNE)
		{
		SPI1->DR = 0x00;

		while((SPI1->SR & SPI_SR_RXNE)==0);
		dataRx[j++] = SPI1->DR;}

	GPIOB->BSRR |= 1 << 6;
}


void Wiz_Init(void)
{
	uint8_t init_data[21];
	uint8_t socket_data[5];
	uint8_t arp_config[4];

	arp_config[0] = 0x00;
	arp_config[1] = 0x00;
	arp_config[2] = 0x04;
	arp_config[3] = 0x01;

	socket_data[0] = 0x00;
	socket_data[1] = 0x1E;
	socket_data[2] = 0x08;
	socket_data[3] = 0x02;
	socket_data[4] = 0x02;

	init_data[0] = 0x00;
	init_data[1] = 0x01;
	init_data[2] = 0x04;

	//gw
	init_data[3] = 0xC0;
	init_data[4] = 0xA8;
	init_data[5] = 0x37;
	init_data[6] = 0x1;

	//subr
	init_data[7] = 0xFF;
	init_data[8] = 0xFF;
	init_data[9] = 0xFF;
	init_data[10] = 0x00;

	//mac
	init_data[11] = 0x00;
	init_data[12] = 0x08;
	init_data[13] = 0xDC;
	init_data[14] = 0x01;
	init_data[15] = 0x02;
	init_data[16] = 0x03;

	//sipr
	init_data[17] = 0xC0;
	init_data[18] = 0xA8;
	init_data[19] = 0x37;
	init_data[20] = 0x0A;


//	GPIOB->BSRR |= 1 << (6 + 16);
//	for (int j = 0; j < 21; j++){
//		SPI1->DR = init_data[j];
//		while(!(SPI1->SR & SPI_SR_TXE));
//	}
//	while(SPI1->SR & SPI_SR_BSY);
//	GPIOB->BSRR |= 1 << 6;

	Transmit_Data(4, arp_config);

	Transmit_Data(21, init_data);
	for (uint8_t count = 0; count < 8; count++) {
		socket_data[2] = (0x0C | (count<<5));
		Transmit_Data(5, socket_data);
	}

}

void Check_Init(void)
{
	uint8_t tx_data[4];
	//uint8_t rx_data[4];

	tx_data[0] = 0x00;  // read operation
	tx_data[1] = 0x01;
	tx_data[2] = 0x00;
	tx_data[3] = 0x00;// dummy byte for response

	int j = 0;

	if((SPI1->SR & SPI_SR_RXNE)!=0)
		a = SPI1->DR;

	GPIOB->BSRR |= 1 << (6 + 16);
	while (j < 3){
		while(!(SPI1->SR & SPI_SR_TXE));
		SPI1->DR = tx_data[j];
		while((SPI1->SR & SPI_SR_RXNE)==0);
		rcv[j++] = SPI1->DR;
	}
	while((SPI1->SR & SPI_SR_BSY));

	a = SPI1->DR;

	j = 0;
	while (j < 4)//(SPI1->SR & SPI_SR_RXNE)
		{
		j++;
		SPI1->DR = tx_data[3];

		while((SPI1->SR & SPI_SR_RXNE)==0);
		received[b++] = SPI1->DR;}

	GPIOB->BSRR |= 1 << 6;

}

void GPIO_Init(void){


	*RCCAHB1_GPIO |= 7; //PORTA|PORTB|PORTC clock enable

	*PORTA_MODER &= ~0x0000A800;

	*PORTA_MODER |= 0x0000A800;

	*PORTA_PULL = 0x64000000;

	*PORTA_AF |= 0x55500000;

	*PORTA_SPEED |= 0xFC00;

	*PORTB_MODER &= ~0x3000;

	*PORTB_MODER |= 0x1000;

	*PORTC_MODER |= 0x4300;

	*PORTC_PULL |= 0x04000000;

}


void ADC_Init(void){

	*APB2ENR |= (1 << 8);

	*ADC_CR1 = 0x01000000; 	//ADC1_14 (PC4)

	*ADC_SMPR1 = (2 << 12); //sampling to 28 cycles

	*ADC_CR2 = 0x00000001; 	//enabling

	*ADC_SQR3 = 0xE;



}

void SPI_Init(void){

	*APB2ENR |= (1 << 12);

	*SPI_CR |=  0x308;

	*SPI_CR |=  0x44;

}



void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
