/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "csro_common.h"

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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId task01Handle;
osThreadId task02Handle;
osThreadId task03Handle;
osThreadId task04Handle;
osThreadId task05Handle;
osThreadId task06Handle;
osThreadId task07Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void task01_most_important_task(void const *argument);
void task02_cps_read_write_task(void const *argument);
void task03_hmi_wait_cmd_task(void const *argument);
void task04_aqi_read_task(void const *argument);
void task05_pwm_adc_dac_gpio_task(void const *argument);
void task06_stepper_task(void const *argument);
void task07_misc_func_task(void const *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

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
  /* definition and creation of task01 */
  osThreadDef(task01, task01_most_important_task, osPriorityAboveNormal, 0, 256);
  task01Handle = osThreadCreate(osThread(task01), NULL);

  /* definition and creation of task02 */
  osThreadDef(task02, task02_cps_read_write_task, osPriorityAboveNormal, 0, 256);
  task02Handle = osThreadCreate(osThread(task02), NULL);

  /* definition and creation of task03 */
  osThreadDef(task03, task03_hmi_wait_cmd_task, osPriorityNormal, 0, 256);
  task03Handle = osThreadCreate(osThread(task03), NULL);

  /* definition and creation of task04 */
  osThreadDef(task04, task04_aqi_read_task, osPriorityNormal, 0, 256);
  task04Handle = osThreadCreate(osThread(task04), NULL);

  /* definition and creation of task05 */
  osThreadDef(task05, task05_pwm_adc_dac_gpio_task, osPriorityBelowNormal, 0, 128);
  task05Handle = osThreadCreate(osThread(task05), NULL);

  /* definition and creation of task06 */
  osThreadDef(task06, task06_stepper_task, osPriorityBelowNormal, 0, 128);
  task06Handle = osThreadCreate(osThread(task06), NULL);

  /* definition and creation of task07 */
  osThreadDef(task07, task07_misc_func_task, osPriorityLow, 0, 128);
  task07Handle = osThreadCreate(osThread(task07), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_task01_most_important_task */
/**
  * @brief  Function implementing the task01 thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_task01_most_important_task */
void task01_most_important_task(void const *argument)
{

  /* USER CODE BEGIN task01_most_important_task */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1000);
  }
  /* USER CODE END task01_most_important_task */
}

/* USER CODE BEGIN Header_task02_cps_read_write_task */
/**
* @brief Function implementing the task02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task02_cps_read_write_task */
void task02_cps_read_write_task(void const *argument)
{
  /* USER CODE BEGIN task02_cps_read_write_task */
  csro_master_cps_init(&huart3);
  /* Infinite loop */
  for (;;)
  {
    csro_master_cps_read_write_task();
  }
  /* USER CODE END task02_cps_read_write_task */
}

/* USER CODE BEGIN Header_task03_hmi_wait_cmd_task */
/**
* @brief Function implementing the task03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task03_hmi_wait_cmd_task */
void task03_hmi_wait_cmd_task(void const *argument)
{
  /* USER CODE BEGIN task03_hmi_wait_cmd_task */
  csro_slave_hmi_init(&huart2);
  /* Infinite loop */
  for (;;)
  {
    csro_slave_hmi_wait_cmd();
  }
  /* USER CODE END task03_hmi_wait_cmd_task */
}

/* USER CODE BEGIN Header_task04_aqi_read_task */
/**
* @brief Function implementing the task04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task04_aqi_read_task */
void task04_aqi_read_task(void const *argument)
{
  /* USER CODE BEGIN task04_aqi_read_task */
  csro_master_aqi_init(&huart1);
  /* Infinite loop */
  for (;;)
  {
    osDelay(1000);
    csro_master_aqi_read_task();
  }
  /* USER CODE END task04_aqi_read_task */
}

/* USER CODE BEGIN Header_task05_pwm_adc_dac_gpio_task */
/**
* @brief Function implementing the task05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task05_pwm_adc_dac_gpio_task */
void task05_pwm_adc_dac_gpio_task(void const *argument)
{
  /* USER CODE BEGIN task05_pwm_adc_dac_gpio_task */
  Csro_Control_init();
  /* Infinite loop */
  for (;;)
  {
    osDelay(100);
    Csro_Auto_Run();
  }
  /* USER CODE END task05_pwm_adc_dac_gpio_task */
}

/* USER CODE BEGIN Header_task06_stepper_task */
/**
* @brief Function implementing the task06 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task06_stepper_task */
void task06_stepper_task(void const *argument)
{
  /* USER CODE BEGIN task06_stepper_task */
  Csro_Stepper_Set_Position(0);
  /* Infinite loop */
  for (;;)
  {
    osDelay(100);
    Csro_Stepper_Set_Position(sys_regs.holdings[4]);
  }
  /* USER CODE END task06_stepper_task */
}

/* USER CODE BEGIN Header_task07_misc_func_task */
/**
* @brief Function implementing the task07 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task07_misc_func_task */
void task07_misc_func_task(void const *argument)
{
  /* USER CODE BEGIN task07_misc_func_task */
  /* Infinite loop */
  for (;;)
  {
    osDelay(500);
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  }
  /* USER CODE END task07_misc_func_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
