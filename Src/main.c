/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"

/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
int infoState = 0;							//state of information throughout the code
char input[100];							//input string used to store buffered data
char lastRecString[100];
char idx, data[2], buffer[100], complete;	//used to configure callback function
char buffer_i2c[1];							//variable used to record the received data from arduino (I2C protocol)
int y_diff = 0;
char input_spi[100];

int MAX_Y = 1250;				//Maximum motor orientation on Y-axis (PWM CCR value)
int MIN_Y = 1100;				//Minimum motor orientation on Y-axis (PWM CCR value)
int MAX_X = 700;				//Maximum motor orientation on X-axis (PWM CCR value)
int MIN_X = 300;				//Minimum motor orientation on X-axis (PWM CCR value)
int mid_x = 320;				//Center x-coordinate for camera frame
int mid_y = 240;				//Center y-coordinate for camera frame
int delta_max_x = 40;			//x-range of static range region (where motors x-axis remain static)
int delta_max_y = 50;			//y-range of static range region (where motors y-axis remain static)
int motor_input_x = 500;		//variable used to determine x-axis motors orientation (PWM CCR value)
int motor_input_y = 1250;		//variable used to determine y-axis motors orientation (PWM CCR value)
int motor_center_x = 500;		//Motor "center" x-coordinate based on the product application (PWM CCR value)
int motor_center_y = 1250;		//Motor "center" y-coordinate based on the product application (PWM CCR value)
int flag_automan = -1;			//flag (integers number) indicate the touchscreen response
int flag_message1 = 0;
int flag_message2 = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                                

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/*
 * UART communication protocol functions
 * (communication with OpenCV)
 */
void transmitString(char* ch){
	//infoState = 1;
	HAL_UART_Transmit(&huart1, (uint8_t *)ch, strlen(ch), 50);
	return;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart){
	uint8_t i;
	if (huart -> Instance == USART1){
		if(idx == 0){
			//clear the buffer before receiving incoming signal
			for(i = 0; i < 100; i++){
				buffer[i] = 0;			
			}
		}
		//start receiving data when enter is pressed 
		if(data[0] != 10 && data[0] != 13){
			buffer[idx++] = data[0]; //transferring data
		}
		else{
			idx = 0;
			complete = 1; //update transfer complete flag to be on (=1)
		}

		HAL_UART_Receive_IT(&huart1, (uint8_t *) data, 1);
	}
}

void receiveString(){
	//HAL_UART_RxCpltCallback(&huart1);
	if(complete){
	  sprintf(input,"%s\n",buffer);	//use sprintf to add the new line character
  	  char* input = buffer;
	  transmitString(input);		//transmit the received signal to the output
  	  complete = 0;					//reset the complete transfer flag
  	  infoState = 1;				//raised the receiving signal flag
	  
  	}
	return;
}

void scan_input(int* x_coord, int* y_coord){
		  if(complete){
		  	sprintf(input,"%s", buffer);
		  	char sep[2]=",";
		  	char* translate_input;


		  	char output[100];

			translate_input = strtok(input, sep);
//			sprintf(output, "Hello from board. this is x input: %s\n", translate_input);
			*x_coord = atoi(translate_input);
//			transmitString(output);		//transmit the received signal to the output

			translate_input = strtok(NULL, sep);

//			sprintf(output, "Hello from board. this is y input: %s\n", translate_input);
			*y_coord = atoi(translate_input);
			complete = 0;					//reset the complete transfer flag

	  }
}

void motor_x(int ccr_value){//max = 700 (highest x coord), min = 300 (lowest y coord)
	if(ccr_value >= MIN_X && ccr_value <= MAX_X){
		TIM1->CCR4 =ccr_value;//300 is the one edge 1200 is the other edge (x-axis, bottom motor)
	}
}

void motor_y(int ccr_value){//max = 1250(lowest y coord) main = 1100 (highest y coord)
	if(ccr_value >= MIN_Y && ccr_value <= MAX_Y){
		TIM1->CCR1 =ccr_value;//300 is the one edge 1200 is the other edge (y-axis, top motor)
	}
}

/*
 * LCD touch screen responsive functions. Touchscreen
 * Input Capture speed configured in these functions.
 */
void motors_reset(){
	  motor_x(500); //default position of x
	  motor_y(1200); //default position of y
//	  motor_input_x = 500; //just for testing
//	  motor_input_y = 1100;
}


void back_to_orig(){
	if(motor_input_x < motor_center_x){
		motor_input_x += 5;

	}else if(motor_input_x > motor_center_x){
		motor_input_x -= 5;
	}

	if(motor_input_y < motor_center_y){
		motor_input_y += 5;
	}
	motor_x(motor_input_x);
	motor_y(motor_input_y);
	HAL_Delay(40);
}


void up(){
	motor_input_y -= 5;
	motor_y(motor_input_y);
	HAL_Delay(50);
}

void down(){
	motor_input_y += 5;
	motor_y(motor_input_y);
	HAL_Delay(50);
}

void left(){
	motor_input_x -= 5;
	motor_x(motor_input_x);
	HAL_Delay(50);
}

void right(){
	motor_input_x += 5;
	motor_x(motor_input_x);
	HAL_Delay(50);
}

/*
 * SPI functions for Hitachi LCD
 */
void transmit_SPI(char* ch){
	HAL_GPIO_WritePin(GPIOA, slave_select_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, (uint8_t *) ch, strlen(ch), HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GPIOA, slave_select_Pin, GPIO_PIN_SET);
	HAL_Delay(20);
}

void clear_screen(){
	int i;
	for(i = 0; i < 100; i++){
		input_spi[i] = 0;
	}
	input_spi[0] = 0xfe;
	transmit_SPI(input_spi);
	input_spi[0] = 0x51;
	transmit_SPI(input_spi);

}


void displayOn(){
	HAL_Delay(2);
	input_spi[0] = 0xfe;
	transmit_SPI(input_spi);
	input_spi[0] = 0x41;
	transmit_SPI(input_spi);

}

void enable_blink(){
	input_spi[0] = 0xfe;
	transmit_SPI(input_spi);
	input_spi[0] = 0x4B;
	transmit_SPI(input_spi);

}

void second_line(){
	input_spi[0] = 0xfe;
	transmit_SPI(input_spi);
	input_spi[0] = 0x45;
	transmit_SPI(input_spi);
	input_spi[0] = 0x40;
	transmit_SPI(input_spi);
}

void first_line(){
	input_spi[0] = 0xfe;
	transmit_SPI(input_spi);
	input_spi[0] = 0x45;
	transmit_SPI(input_spi);
	input_spi[0] = 0x00;
	transmit_SPI(input_spi);
}

void move_display_left(){
	input_spi[0] = 0xfe;
	transmit_SPI(input_spi);
	input_spi[0] = 0x55;
	transmit_SPI(input_spi);
}

void move_display_right(){
	input_spi[0] = 0xfe;
	transmit_SPI(input_spi);
	input_spi[0] = 0x56;
	transmit_SPI(input_spi);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart1, (uint8_t *) data, 1);
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_PWM_Init(&htim1);//pwm
  TIM1->BDTR |=0x8000;
  TIM1->CCER |= 0X1;
  TIM1->CCER |= 0X1000;

  /*
   * hardware initialization
   */
//  motors_reset();
//  int i;
//  for(i = 0; i < 100; i++){
//    input_spi[i] = 0;
//  }

//  back_to_orig();
  motors_reset();
  clear_screen();
  displayOn();
  transmit_SPI("Hello Human");
  second_line();
  transmit_SPI("I can see you");
  flag_message1 = 1;

//  clear_screen();
//  transmit_SPI("  <-- <--");
//  second_line();
//  transmit_SPI(" control ...");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  while (1)
  {


  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	  /*
	   * Flag definition based on touch screen
	   */


	HAL_I2C_Master_Receive(&hi2c1, 0x08 << 1,buffer_i2c, 1, 100);
	if(buffer_i2c[0] == '2'){
		flag_automan = 2;
	}else if(buffer_i2c[0] == '1'){
		flag_automan = 1;
	}else if(buffer_i2c[0] == '3'){
		flag_automan = 3;
	}else if(buffer_i2c[0] == '4'){
		flag_automan = 4;
	}else if(buffer_i2c[0] == '5'){
		flag_automan = 5;
	}else if(buffer_i2c[0] == '6'){
		flag_automan = 6;
	}else if(buffer_i2c[0] == '7'){
		flag_automan = 7;
	}else if(buffer_i2c[0] == '8'){
		flag_automan = 8;
	}else if(buffer_i2c[0] == '0'){
		flag_automan = 0;
	}


	/*
	 * Motors automatic mode algorithms
	 * based on openCV response
	 */
	if(flag_automan == 0 || flag_automan == 1){
		volatile int x_coord = -1;
		volatile int y_coord = -1;
		scan_input(&x_coord, &y_coord);

		int diff_x = x_coord - mid_x;
		int diff_y = y_coord - mid_y;

		if(diff_x < (-1 * delta_max_x) && x_coord != -1){
			motor_input_x += 5;
		}else if(diff_x > delta_max_x && x_coord != -1){
			motor_input_x -= 5;
		}

		if(diff_y < (-1 * delta_max_y) && y_coord != -1){
			motor_input_y -= 5;
		}else if(diff_y > delta_max_y && y_coord != -1){
			motor_input_y += 5;
		}

		motor_x(motor_input_x);
		motor_y(motor_input_y);
	}

	/*
	 * Motors manual mode algorithms
	 * based on touch screen response
	 * Note that the touch screen
	 * capture speed is configured by each
	 * up, down, left, right functions
	 */
	if(flag_automan == 6){
		up();
	}
	if(flag_automan == 7){
		down();
	}
	if(flag_automan == 5){
		right();
	}
	if(flag_automan == 4){
		left();
	}
	if(flag_automan == 8){
//		back_to_orig();
		motors_reset();
	}


//	if(motor_input_x == MAX_X || motor_input_x == MIN_X || motor_input_y == MIN_Y){
//		clear_screen();
//		transmit_SPI("Oops, you are at");
//		second_line();
//		transmit_SPI("the limit");
//	}else{
		if(flag_automan == 1 && flag_message1 == 0){
			clear_screen();
			transmit_SPI("Hello Human");
			second_line();
			transmit_SPI("I can see you");
			flag_message1 = 1;
			flag_message2 = 0;
		}else if(flag_automan == 2 && flag_message2 == 0){
			clear_screen();
			transmit_SPI("You're in");
			second_line();
			transmit_SPI("control ...");
			flag_message1 = 0;
			flag_message2 = 1;
		}
//	}


  }


  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Analogue filter 
    */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Digital filter 
    */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM1 init function */
static void MX_TIM1_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 20;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 19999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_MspPostInit(&htim1);

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(slave_select_GPIO_Port, slave_select_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LD4_Pin|LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : slave_select_Pin */
  GPIO_InitStruct.Pin = slave_select_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(slave_select_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
