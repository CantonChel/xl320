/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// 舵机位置信息结构体
typedef struct {
    uint8_t id;
    uint16_t length;
    uint8_t inst;
    uint8_t error;
    uint32_t position;
    uint8_t crc1;
    uint8_t crc2;
} XPositionInfo;

typedef struct {
    uint8_t id;
    uint16_t length;
    uint8_t inst;
    uint8_t error;
    uint16_t position;
    uint8_t crc1;
    uint8_t crc2;
} X320PositionInfo;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/*                  ֡ͷ                  id      length       instruct    address     parameters               crc          */
u8 torqueFrame[]={  0xFF,0xFF,0xFD,0x00,  0x00,   0x06,0x00,   0x03,      0x40,0x00,   0x01,                   0x00,0x00};
u8 xl430PFrame[]={  0xFF,0xFF,0xFD,0x00,  0x00,   0x09,0x00,   0x03,      0x74,0x00,   0xAD,0x09,0x00,0x00,    0x65,0x6D};
u8 xl430PGFrame[]={ 0xFF,0xFF,0xFD,0x00,  0x00,   0x07,0x00,   0x03,      0x54,0x00,   0x80,0x02,              0x65,0x6D};
u8 ledFrame[]={     0xFF,0xFF,0xFD,0x00,  0x00,   0x06,0x00,   0x03,      0x19,0x00,   0x01,                   0x00,0x00};
u8 xl320PFrame[]={  0xFF,0xFF,0xFD,0x00,  0x00,   0x07,0x00,   0x03,      0x1E,0x00,   0xAD,0x09,              0x65,0x6D};
u8 xl320PGFrame[]={ 0xFF,0xFF,0xFD,0x00,  0x00,   0x06,0x00,   0x03,      0x1D,0x00,   0x20,                   0x65,0x6D};


u8 xl430PRead[]={   0xFF,0xFF,0xFD,0x00,  0x00,   0x07,0x00,   0x02,      0x84,0x00,   0x04,0x00,              0x1D,0x15};
u8 xl320PRead[]={   0xFF,0xFF,0xFD,0x00,  0x00,   0x07,0x00,   0x02,      0x25,0x00,   0x02,0x00,              0x21,0xB5};

XPositionInfo *xl430_1=0;
XPositionInfo *xl430_2=0;
X320PositionInfo *xl320_1=0;
u8 xl430_1_data[11];
u8 xl430_2_data[11];
u8 xl320_1_data[9];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void dynamixel_set_direction(uint8_t tx_mode);
void dynamixel_transmit_packet(uint8_t* packet, uint16_t length);
uint16_t update_crc(uint16_t crc_accum, uint8_t *data_blk_ptr, uint16_t data_blk_size);
void dynamixel_write_byte(uint8_t id, uint16_t address, uint8_t value);
void dynamixel_enable_torque(uint8_t id, uint8_t enable);
void dynamixel_set_led(uint8_t id, uint8_t color);
void dynamixel_set_goal_position(uint8_t id, uint16_t position);

//xl系列舵机驱动
void xlSeriesStart(void);
void xl320SendPosition(u8 id,u16 position);
void xlSeriesSendFrame(USART_TypeDef * pUSARTx,u8 * frame,u8 length);
void xlSeriesTorque(u8 id,u8 on,u8 address);
void xlSeriesLed(u8 id,u8 on,u8 address);
unsigned short updateCRC(uint16_t crc_accum, uint8_t *data_blk_ptr, uint16_t data_blk_size);
void debugFrame(u8 *frame,u8 length);
void xl320SendPGain(u8 id,u8 pGain);
void xl320ReadPosition(u8 id);
void xlPowerOff(u8 isOn);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

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
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();

// 初始化方向控制引脚为接收模式
dynamixel_set_direction(0);
HAL_Delay(100); // 等待舵机上电稳定

// 使能舵机扭矩
dynamixel_enable_torque(SERVO_ID, 1);
HAL_Delay(100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  // 任务1: 转到位置 0 ，亮蓝灯
	      dynamixel_set_led(SERVO_ID, LED_BLUE);
	      dynamixel_set_goal_position(SERVO_ID, 0);
	      HAL_Delay(1500); // 等待1.5秒让舵机转动到位

	      // 任务2: 转到位置 1023 (大约300度)，亮红灯
	      dynamixel_set_led(SERVO_ID, LED_RED);
	      dynamixel_set_goal_position(SERVO_ID, 1023);
	      HAL_Delay(1500);

	      // 任务3: 转到中间位置 512，亮绿灯
	      dynamixel_set_led(SERVO_ID, LED_GREEN);
	      dynamixel_set_goal_position(SERVO_ID, 512);
	      HAL_Delay(1500);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
#define DIR1_PORT GPIOA
#define DIR1      GPIO_PIN_1

// 设置数据方向
void dynamixel_set_direction(uint8_t tx_mode) {
    if (tx_mode) {
        HAL_GPIO_WritePin(DIR1_GPIO_Port, DIR1_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(DIR1_GPIO_Port, DIR1_Pin, GPIO_PIN_RESET);
    }
}
// 发送数据包
void dynamixel_transmit_packet(uint8_t* packet, uint16_t length) {
    dynamixel_set_direction(1); // 切换到发送模式
    HAL_UART_Transmit(&huart2, packet, length, 100); // 发送数据
    while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY);
    dynamixel_set_direction(0); // 切换回接收模式
}


unsigned short update_crc(unsigned short crc_accum, unsigned char *data_blk_ptr, unsigned short data_blk_size)
{
    unsigned short i, j;
    unsigned short crc_table[256] = {
        0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
        0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
        0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
        0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
        0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
        0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
        0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
        0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
        0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
        0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
        0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
        0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
        0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
        0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
        0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
        0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
        0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
        0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
        0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
        0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
        0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
        0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
        0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
        0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
        0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
        0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
        0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
        0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
        0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
        0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
        0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
        0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
    };

    for(j = 0; j < data_blk_size; j++)
    {
        i = ((unsigned short)(crc_accum >> 8) ^ data_blk_ptr[j]) & 0xFF;
        crc_accum = (crc_accum << 8) ^ crc_table[i];
    }

    return crc_accum;
}

// 构造并发送一个"写"指令
void dynamixel_write_packet(uint8_t id, uint16_t address, uint8_t* data, uint16_t data_len) {
    uint8_t packet[64]; // 缓冲区
    uint16_t packet_len = data_len + 5; // 指令(1) + 地址(2) + 数据(N) + CRC(2)
    uint16_t total_len = data_len + 12; // 完整数据包长度

    packet_len = data_len + 3; // Length = Instruction(1) + Parameters(N)

    // 完整数据包的长度
    total_len = packet_len + 7; // Length + 7 (Headers, Reserved, ID, CRC)

    // 1. 构建包头和ID
    packet[0] = 0xFF;
    packet[1] = 0xFF;
    packet[2] = 0xFD;
    packet[3] = 0x00;
    packet[4] = id;
    // 2. 长度 (小端模式)
    packet[5] = packet_len & 0xFF;
    packet[6] = (packet_len >> 8) & 0xFF;
    // 3. 指令
    packet[7] = 0x03; // WRITE 指令
    // 4. 参数
    packet[8] = address & 0xFF; // 地址 (小端模式)
    packet[9] = (address >> 8) & 0xFF;
    for(int i=0; i < data_len; i++) {
        packet[10+i] = data[i];
    }
    // 5. CRC
    uint16_t crc = update_crc(0, packet, total_len - 2);
    packet[total_len - 2] = crc & 0xFF; // CRC (小端模式)
    packet[total_len - 1] = (crc >> 8) & 0xFF;

    // 6. 发送
    dynamixel_transmit_packet(packet, total_len);
}


// 使能/失能扭矩
void dynamixel_enable_torque(uint8_t id, uint8_t enable) {
    // XL-320 控制表地址: Torque Enable = 24
    dynamixel_write_packet(id, 24, &enable, 1);
}

// 设置LED颜色
void dynamixel_set_led(uint8_t id, uint8_t color) {
    // XL-320 控制表地址: LED = 25
    dynamixel_write_packet(id, 25, &color, 1);
}

// 设置目标位置
void dynamixel_set_goal_position(uint8_t id, uint16_t position) {
    // XL-320 控制表地址: Goal Position = 30 (2 bytes)
    uint8_t data[2];
    data[0] = position & 0xFF;
    data[1] = (position >> 8) & 0xFF;
    dynamixel_write_packet(id, 30, data, 2);
}

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
#ifdef USE_FULL_ASSERT
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
