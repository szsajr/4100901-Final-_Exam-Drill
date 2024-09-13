/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "status.h"
#include <string.h>

#define OLED_ADDRESS 0x3C
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
int state = 0; //ESTADO DEL SISTEMA
uint8_t claveIngresada[10];
uint8_t claveCorrecta[10] = {1,0,8,7,7,0,5,98,4,4}; //DOCUMENTO
uint8_t cindex = 0;
uint8_t claveVerificada = 0;

//UART MENSAJES
char successMsg[] = "Success\r\n";
char errorMsg[] = "Error\r\n";

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
uint8_t detectarTecla(uint16_t GPIO_Pin);  // Prototipo de la función detectarTecla
//void mostrarError(void);  // Prototipo de la función mostrarError
//void mostrarExito(void);  // Prototipo de la función mostrarExito
void actualizarDisplay(uint8_t *clave, uint8_t size);  // Prototipo de la función actualizarDisplay
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
  // to using printf
  HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 10);
  return len;
}



uint8_t detectarTecla(uint16_t GPIO_Pin) {
    uint8_t tecla = 0xFF;  // Valor por defecto para error/no tecla detectada

    // Suponiendo que las filas son controladas por pines GPIO y las columnas son las que generan interrupciones
    // Primero se activan las filas secuencialmente para detectar la columna correspondiente.

    // Activar ROW_1
    HAL_GPIO_WritePin(ROW_1_GPIO_Port, ROW_1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ROW_2_GPIO_Port, ROW_2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW_3_GPIO_Port, ROW_3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW_4_GPIO_Port, ROW_4_Pin, GPIO_PIN_SET);

    // Verificar qué columna generó la interrupción
    if (GPIO_Pin == COL_1_Pin) {
        tecla = 1;
    } else if (GPIO_Pin == COL_2_Pin) {
        tecla = 2;
    } else if (GPIO_Pin == COL_3_Pin) {
        tecla = 3;
    } else if (GPIO_Pin == COL_4_Pin) {
        tecla = 10;  // Aquí se puede asignar a una función especial, como '*'
    }

    // Activar ROW_2
    HAL_GPIO_WritePin(ROW_1_GPIO_Port, ROW_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW_2_GPIO_Port, ROW_2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ROW_3_GPIO_Port, ROW_3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW_4_GPIO_Port, ROW_4_Pin, GPIO_PIN_SET);

    // Verificar qué columna generó la interrupción
    if (GPIO_Pin == COL_1_Pin) {
        tecla = 4;
    } else if (GPIO_Pin == COL_2_Pin) {
        tecla = 5;
    } else if (GPIO_Pin == COL_3_Pin) {
        tecla = 6;
    } else if (GPIO_Pin == COL_4_Pin) {
        tecla = 11;  // Aquí se puede asignar a una función especial, como '0'
    }

    // Activar ROW_3
    HAL_GPIO_WritePin(ROW_1_GPIO_Port, ROW_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW_2_GPIO_Port, ROW_2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW_3_GPIO_Port, ROW_3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ROW_4_GPIO_Port, ROW_4_Pin, GPIO_PIN_SET);

    // Verificar qué columna generó la interrupción
    if (GPIO_Pin == COL_1_Pin) {
        tecla = 7;
    } else if (GPIO_Pin == COL_2_Pin) {
        tecla = 8;
    } else if (GPIO_Pin == COL_3_Pin) {
        tecla = 9;
    } else if (GPIO_Pin == COL_4_Pin) {
        tecla = 12;  // Aquí se puede asignar a una función especial, como '#'
    }

    // Activar ROW_4
    HAL_GPIO_WritePin(ROW_1_GPIO_Port, ROW_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW_2_GPIO_Port, ROW_2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW_3_GPIO_Port, ROW_3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW_4_GPIO_Port, ROW_4_Pin, GPIO_PIN_RESET);

    // Verificar qué columna generó la interrupción
    if (GPIO_Pin == COL_1_Pin) {
        tecla = '*';  // Función especial
    } else if (GPIO_Pin == COL_2_Pin) {
        tecla = 0;
    } else if (GPIO_Pin == COL_3_Pin) {
        tecla = '#';  // Función especial
    } else if (GPIO_Pin == COL_4_Pin) {
        tecla = 13;  // Otra función especial si es necesario
    }

    // Restaurar el estado inicial de las filas (todas en alto)
    HAL_GPIO_WritePin(ROW_1_GPIO_Port, ROW_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW_2_GPIO_Port, ROW_2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW_3_GPIO_Port, ROW_3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW_4_GPIO_Port, ROW_4_Pin, GPIO_PIN_SET);

    return tecla;  // Devolver la tecla presionada
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == COL_1_Pin || GPIO_Pin == COL_2_Pin || GPIO_Pin == COL_3_Pin || GPIO_Pin == COL_4_Pin) {
        // Detecta y actualiza la clave ingresada
        if (cindex < 10) {  // Asegúrate de no superar el tamaño del buffer
            uint8_t tecla = detectarTecla(GPIO_Pin);
            if (tecla == '*' && cindex > 0) {
                // Reinicia la secuencia
                cindex = 0;
                claveIngresada[cindex] = '\0'; // Limpiar clave ingresada
                actualizarDisplay(claveIngresada, cindex);
            } else if (tecla == '#' && cindex > 0) {
                // Verifica la clave
                claveVerificada = 1;
                actualizarDisplay(claveIngresada, cindex);
            } else if (tecla >= 0 && tecla <= 9 && cindex < 10) {
                // Solo almacena teclas numéricas y controla el índice
                claveIngresada[cindex++] = tecla;
                actualizarDisplay(claveIngresada, cindex);
            }
        }
    }
}

void verificarClave() {
    if (claveVerificada) {
        int correct = 1;
        for (int i = 0; i < 10; i++) {
            if (claveIngresada[i] != claveCorrecta[i]) {
                correct = 0;
                break;
            }
        }
        if (correct) {
            LED_SetState(LED_ON);
            HAL_UART_Transmit(&huart2, (const uint8_t*)successMsg, sizeof(successMsg) - 1, 100);
            HAL_Delay(3000); // Mantén el LED encendido por 3 segundos
            LED_SetState(LED_OFF); // Apaga el LED después de 3 segundos
        } else {
            LED_SetState(LED_BLINK); // Parpadea el LED en caso de error
            HAL_UART_Transmit(&huart2, (const uint8_t*)errorMsg, sizeof(errorMsg) - 1, 100);
        }
        claveVerificada = 0; // Restablece el estado de la verificación
        cindex = 0; // Reinicia el índice de la clave ingresada
        memset(claveIngresada, 0, sizeof(claveIngresada)); // Limpia el buffer de clave
    }
}




void actualizarDisplay(uint8_t *clave, uint8_t size) {
    if (claveVerificada) {
        // Mostrar mensaje de éxito o error
        if (size > 0) {
            HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, clave, size, HAL_MAX_DELAY);
        }
        if (clave[0] == successMsg) {
            HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, (uint8_t *)successMsg, sizeof(successMsg) - 1, HAL_MAX_DELAY);
        } else {
            HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, (uint8_t *)errorMsg, sizeof(errorMsg) - 1, HAL_MAX_DELAY);
        }
    } else {
        // Mostrar la clave ingresada
        HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, clave, size, HAL_MAX_DELAY);
    }
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
//HAL_Init();
//LED_Init(GPIOA, GPIO_PIN_5);
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  printf("Starting...\r\n");
  while (1)
  {
    /* USER CODE END WHILE */
	  if (claveVerificada) {
				 verificarClave();  // Verificar si ya se ingresó toda la clave
				 claveVerificada = 0;  // Reset de flag para evitar verificar en cada ciclo
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10909CEC;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SYSTEM_LED_GPIO_Port, SYSTEM_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ROW_1_GPIO_Port, ROW_1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ROW_2_Pin|ROW_4_Pin|ROW_3_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : SYSTEM_LED_Pin ROW_1_Pin */
  GPIO_InitStruct.Pin = SYSTEM_LED_Pin|ROW_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : COL_1_Pin */
  GPIO_InitStruct.Pin = COL_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(COL_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : COL_4_Pin */
  GPIO_InitStruct.Pin = COL_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(COL_4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : COL_2_Pin COL_3_Pin */
  GPIO_InitStruct.Pin = COL_2_Pin|COL_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : ROW_2_Pin ROW_4_Pin ROW_3_Pin */
  GPIO_InitStruct.Pin = ROW_2_Pin|ROW_4_Pin|ROW_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
