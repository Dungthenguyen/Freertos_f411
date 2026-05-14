/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "fc_motor.h"
#include "fc_mpu6050.h"
#include "fc_pid.h"
#include "fc_rc.h"
#include "task.h"

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
DMA_HandleTypeDef hdma_i2c1_rx;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

osThreadId pidTaskHandle;
osThreadId rcTaskHandle;
/* USER CODE BEGIN PV */
volatile uint32_t dbg_tim4_irq_cnt = 0;
volatile uint32_t dbg_pid_task_cnt = 0;
volatile uint32_t dbg_rc_task_cnt = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
void StartPIDTask(void const *argument);
void StartRCTask(void const *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

  FC_Motor_Init();
  FC_MPU6050_Init();
  FC_RC_Init();

  // THÊM DÒNG NÀY — start TIM3 để PPM interrupt hoạt động
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);

  // Chờ thêm 1 giây để PPM ổn định trước khi đo
  HAL_Delay(1000);

  // ==========================================
  // ESC CALIBRATION MODE
  // Để calib: Bật TX, đẩy ga lên MAX, sau đó cấp nguồn cho mạch (hoặc nạp code
  // xong). Đợi ESC kêu bíp bíp xác nhận ga MAX, sau đó kéo ga xuống MIN. ESC sẽ
  // kêu báo hiệu calib thành công.
  // ==========================================
  if (rc_channels[2] > 1900) {
    // Nháy LED nhanh báo hiệu vào chế độ Calib ESC
    for (int i = 0; i < 10; i++) {
      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
      HAL_Delay(100);
    }

    // Gửi xung MAX 2000us cho tất cả ESC TRƯỚC KHI BẬT PWM
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, FC_MOTOR_CALIB_MAX_US);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, FC_MOTOR_CALIB_MAX_US);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, FC_MOTOR_CALIB_MAX_US);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, FC_MOTOR_CALIB_MAX_US);

    // Bắt đầu xuất xung PWM. Lúc này ESC sẽ nhận được 2000us ngay lập tức!
    FC_Motor_StartPWM();

    // Chờ người dùng kéo ga xuống thấp nhất (< 1100)
    while (rc_channels[2] > 1100) {
      HAL_Delay(50);
      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }

    // Gửi xung MIN 1000us cho tất cả ESC
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, FC_MOTOR_MIN_US);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, FC_MOTOR_MIN_US);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, FC_MOTOR_MIN_US);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, FC_MOTOR_MIN_US);

    // Chờ 3 giây để ESC nhận xung MIN và lưu cài đặt
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0); // Bật LED sáng tĩnh
    HAL_Delay(3000);
  } else {
    // Nếu không vào chế độ Calib, gửi xung 1000us và chạy PWM bình thường
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, FC_MOTOR_MIN_US);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, FC_MOTOR_MIN_US);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, FC_MOTOR_MIN_US);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, FC_MOTOR_MIN_US);
    FC_Motor_StartPWM();
  }
  // ==========================================

  // LED nhấp nháy báo hiệu đang calibrate
  // Tay cầm phải để GIỮA lúc này
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);
  HAL_Delay(300);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
  HAL_Delay(300);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);
  HAL_Delay(300);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
  HAL_Delay(300);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);
  HAL_Delay(300);

  FC_RC_CalibrateCenter(); // 1 giây đo center

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1); // LED tắt = xong

  FC_PID_Init();

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of pidTask */
  osThreadDef(pidTask, StartPIDTask, osPriorityHigh, 0, 512);
  pidTaskHandle = osThreadCreate(osThread(pidTask), NULL);

  /* definition and creation of rcTask */
  osThreadDef(rcTask, StartRCTask, osPriorityIdle, 0, 128);
  rcTaskHandle = osThreadCreate(osThread(rcTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */
  xTraceInitialize();
  vTraceEnable(TRC_START);

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */
}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void) {

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 99;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2499; // 2500us -> 400Hz cho ESC drone
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = FC_MOTOR_MIN_US;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);
}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void) {

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 99;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim3) != HAL_OK) {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
}

/**
 * @brief TIM4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM4_Init(void) {

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 99;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK) {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK) {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartPIDTask */
/**
 * @brief  Function implementing the pidTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartPIDTask */
void StartPIDTask(void const *argument) {
  /* USER CODE BEGIN 5 */
  // 1. Khởi tạo và Calib MPU khi RTOS đã chạy (osDelay hoạt động an toàn)
  FC_MPU6050_Init();
  osDelay(200);           // Chờ cảm biến ổn định
  FC_MPU6050_Calibrate(); // Tính sai số tĩnh (Gyro Offset)
  // 2. CỰC KỲ QUAN TRỌNG: Calib xong xuôi MỚI bật Timer 4
  // Timer 4 sẽ tự động gõ nhịp 1ms và tự động gọi DMA!
  HAL_TIM_Base_Start_IT(&htim4);
  /* Infinite loop */
  for (;;) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    dbg_pid_task_cnt++;
    FC_PID_Run_1ms();
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartRCTask */
/**
 * @brief Function implementing the rcTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartRCTask */
void StartRCTask(void const *argument) {
  /* USER CODE BEGIN StartRCTask */
  /* Infinite loop */
  for (;;) {
    dbg_rc_task_cnt++;
    // TODO: Kiểm tra mảng kênh PPM, thực hiện logic Arm/Disarm
    FC_RC_Process();

    osDelay(20); // Chạy RC Task ở tần số 50Hz là đủ, vì tín hiệu tay cầm cập
                 // nhật chậm
  }
  /* USER CODE END StartRCTask */
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM5 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM5) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  // Ngắt 1ms của TIM4 dành cho PID
  else if (htim->Instance == TIM4) {
    dbg_tim4_irq_cnt++; // Biến đếm debug của bạn
    FC_MPU6050_Read_DMA();
  }
  /* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
