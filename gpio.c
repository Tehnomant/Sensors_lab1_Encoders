/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct {
    int32_t position;         
    int8_t  delta;            
    uint8_t state;            
    uint8_t last_state;       
    uint32_t last_tick;       
} Encoder_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ENC_DEBOUNCE_MS   2u
#define LOG_PERIOD_MS     10u
#define COUNT_PER_REV     80
/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
static Encoder_t enc = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */
static void Encoder_Init(void);
static void Encoder_Update(void);
static inline uint8_t Encoder_ReadState(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// retarget printf -> USART2
int _write(int file, char *ptr, int len)
{
  (void)file;
  HAL_UART_Transmit(&huart2, (uint8_t*)ptr, (uint16_t)len, HAL_MAX_DELAY);
  return len;
}

/* USER CODE END 0 */

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  Encoder_Init();

  printf("t_ms,pos,angle_x10\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    Encoder_Update();

    static uint32_t lastLog = 0;
    uint32_t now = HAL_GetTick();
    if (now - lastLog >= LOG_PERIOD_MS)
    {
      lastLog = now;

      int32_t angle_x10 = (enc.position * 3600) / COUNT_PER_REV;

      printf("%lu,%ld,%ld\r\n",
             (unsigned long)now,
             (long)enc.position,
             (long)angle_x10);
    }
  }
  /* USER CODE END WHILE */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
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
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // P0_Pin (PC0) = DT, P1_Pin (PC1) = CLK
  GPIO_InitStruct.Pin = P0_Pin|P1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

// PC1 = CLK, PC0 = DT
static inline uint8_t Encoder_ReadState(void)
{
    uint8_t clk = (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1) == GPIO_PIN_SET) ? 1 : 0;
    uint8_t dt  = (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0) == GPIO_PIN_SET) ? 1 : 0;
    return (uint8_t)((clk << 1) | dt);   // b1=CLK, b0=DT
}

static void Encoder_Init(void)
{
    enc.position   = 0;
    enc.delta      = 0;
    enc.state      = Encoder_ReadState();
    enc.last_state = enc.state;
    enc.last_tick  = HAL_GetTick();
}

static void Encoder_Update(void)
{
    enc.delta = 0;

    uint8_t s = Encoder_ReadState();
    if (s == enc.state) return;         
    
    enc.last_state = enc.state;
    enc.state      = s;

    uint32_t now = HAL_GetTick();
    if (now - enc.last_tick < ENC_DEBOUNCE_MS) return;

    uint8_t transition = (uint8_t)((enc.last_state << 2) | enc.state);

    switch (transition)
    {
        // CW (+1)
        case 0b0001: // 00->01
        case 0b0111: // 01->11
        case 0b1110: // 11->10
        case 0b1000: // 10->00
            enc.delta = +1;
            enc.position += 1;
            enc.last_tick = now;
            break;

        // CCW (-1)
        case 0b0010: // 00->10
        case 0b1011: // 10->11
        case 0b1101: // 11->01
        case 0b0100: // 01->00
            enc.delta = -1;
            enc.position -= 1;
            enc.last_tick = now;
            break;

        default:
            break;
    }
}

/* USER CODE END 4 */

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  (void)file;
  (void)line;
}
#endif
