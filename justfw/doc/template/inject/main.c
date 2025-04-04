
/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    /* USER CODE BEGIN 1 */

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
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_CAN1_Init();
    MX_CAN2_Init();
    MX_I2C1_Init();
    MX_SPI1_Init();
    MX_SPI2_Init();
    MX_UART5_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    MX_CRC_Init();
    MX_TIM8_Init();
    MX_UART4_Init();
    MX_TIM2_Init();
    /* USER CODE BEGIN 2 */
    Sys_Init();
    Modules_Init();

    // extern void app1_init();
    // app1_init();
    extern void Drat_Init();
    extern void Engin_Chassis_Init();

    extern void Engin_Little_Hand_Init();
    extern void test_app_init();
    // Drat_Init();
    // Engin_Chassis_Init();
    test_app_init();
    // Engin_Little_Hand_Init();

    /* USER CODE END 2 */

    /* Init scheduler */
    osKernelInitialize();

    /* Call init function for freertos objects (in cmsis_os2.c) */
    MX_FREERTOS_Init();

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
