#include "xlSeries.h"
#include "bsp_SysTick.h"
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


static void delay(u16 count){
	while(count!=0){
	    count--;
	}
}

void xlSeriesStart(){

  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(servoDirClk, ENABLE);

  //GPIOF9,F10?????
  GPIO_InitStructure.GPIO_Pin = servoDirPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(servoDirPort, &GPIO_InitStructure);
	
	xlSeriesLed(0x01,0x01,XL430Led);
	Delay_ms(100);
	xlSeriesLed(0x02,0x01,XL430Led);
	Delay_ms(100);
	xlSeriesLed(0x03,0x01,XL320Led);
	Delay_ms(100);
	
	
	xlSeriesTorque(0x01,0x01,XL430Torque);
	Delay_ms(100);
	xlSeriesTorque(0x02,0x01,XL430Torque);
	Delay_ms(100);
	xlSeriesTorque(0x03,0x01,XL320Torque);
	Delay_ms(100);
	
	
	xl430SendPosition(0x01,180*11.375);
	Delay_ms(100);
	xl430SendPosition(0x02,180*11.375);
	Delay_ms(100);
	xl320SendPosition(0x03,150*3.41);
	Delay_ms(100);
	
	GPIO_SetBits(servoDirPort,servoDirPin);

}
void xlSeriesLed(u8 id,u8 on,u8 address){
	   u16 crc;
	   GPIO_SetBits(servoDirPort,servoDirPin);
     ledFrame[4]=id;
	   ledFrame[8]=address;
	   ledFrame[10]=on;
     crc=updateCRC(0,ledFrame,11);
	   ledFrame[11]=(u8)(crc&0xff);
	   ledFrame[12]=(u8)((crc>>8)&0xff);
	   debugFrame(ledFrame,13);
	   xlSeriesSendFrame(USART2,ledFrame,13);
	   Delay_ms(1);
	   GPIO_ResetBits(servoDirPort,servoDirPin);

}
void xl430SendPosition(u8 id,u16 position){
	   u16 crc;
	   GPIO_SetBits(servoDirPort,servoDirPin);
     xl430PFrame[4]=id;
	   xl430PFrame[10]=(u8)(position&0xFF);
	   xl430PFrame[11]=(u8)((position>>8)&0xFF);
	   xl430PFrame[12]=0x00;
	   xl430PFrame[13]=0x00;
	   crc=updateCRC(0,xl430PFrame,14);
	   xl430PFrame[14]=(u8)(crc&0xff);
	   xl430PFrame[15]=(u8)((crc>>8)&0xff);
	   //debugFrame(xl430PFrame,16);
	   xlSeriesSendFrame(USART2,xl430PFrame,16);
     Delay_ms(1);
	   GPIO_ResetBits(servoDirPort,servoDirPin);
}

void xl320SendPosition(u8 id,u16 position){
	
	  // printf("position:%d\r\n",position);
	   u16 crc;
	   GPIO_SetBits(servoDirPort,servoDirPin);
     xl320PFrame[4]=id;
	   xl320PFrame[10]=(u8)(position&0xFF);
	   xl320PFrame[11]=(u8)((position>>8)&0xFF);
	   crc=updateCRC(0,xl320PFrame,12);
	   xl320PFrame[12]=(u8)(crc&0xff);
	   xl320PFrame[13]=(u8)((crc>>8)&0xff);
	   //debugFrame(xl320PFrame,14);
	   xlSeriesSendFrame(USART2,xl320PFrame,14);
     Delay_ms(1);
	   GPIO_ResetBits(servoDirPort,servoDirPin);
}

void xlSeriesTorque(u8 id,u8 on,u8 address){
	   u16 crc;
	   GPIO_SetBits(servoDirPort,servoDirPin);
     torqueFrame[4]=id;
	   torqueFrame[8]=address;
	   torqueFrame[10]=on;
	   crc=updateCRC(0,torqueFrame,11);
	   torqueFrame[11]=(u8)(crc&0xff);
	   torqueFrame[12]=(u8)((crc>>8)&0xff);
	   debugFrame(torqueFrame,13);
	   xlSeriesSendFrame(USART2,torqueFrame,13);
     Delay_ms(1);
	   GPIO_ResetBits(servoDirPort,servoDirPin);
	   
}


void xl430ReadPosition(u8 id){
     u16 crc;
	   GPIO_SetBits(servoDirPort,servoDirPin);
     xl430PRead[4]=id;
	   crc=updateCRC(0,xl430PRead,12);
	   xl430PRead[12]=(u8)(crc&0xff);
	   xl430PRead[13]=(u8)((crc>>8)&0xff);
	   //debugFrame(xl430PRead,14);
	   xlSeriesSendFrame(USART2,xl430PRead,14);
	   //Delay_us(9);
	   delay(90);
	   GPIO_ResetBits(servoDirPort,servoDirPin);
}

void xl320ReadPosition(u8 id){
     u16 crc;
	   GPIO_SetBits(servoDirPort,servoDirPin);
     xl320PRead[4]=id;

	   crc=updateCRC(0,xl320PRead,12);
	   xl320PRead[12]=(u8)(crc&0xff);
	   xl320PRead[13]=(u8)((crc>>8)&0xff);
	   //debugFrame(xl320PRead,13);
	   xlSeriesSendFrame(USART2,xl320PRead,14);
    // Delay_us(9);
	   delay(90);
	   GPIO_ResetBits(servoDirPort,servoDirPin);

}


void xlSeriesSendFrame(USART_TypeDef * pUSARTx,u8 * frame,u8 length){
	for(int i=0;i<length;i++){
		
		  USART_SendData(pUSARTx,frame[i]);
		  while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
	}
}

void xl430SendPGain(u8 id,u16 pGain){
     u16 crc;
	   GPIO_SetBits(servoDirPort,servoDirPin);
     xl430PGFrame[4]=id;
	   xl430PGFrame[10]=(u8)(pGain&0xFF);
	   xl430PGFrame[11]=(u8)((pGain>>8)&0xFF);
	   crc=updateCRC(0,xl430PGFrame,12);
	   xl430PGFrame[12]=(u8)(crc&0xff);
	   xl430PGFrame[13]=(u8)((crc>>8)&0xff);
	   debugFrame(xl430PGFrame,14);
	   xlSeriesSendFrame(USART2,xl430PGFrame,14);
     Delay_ms(2);
	   GPIO_ResetBits(servoDirPort,servoDirPin);

}
void xl320SendPGain(u8 id,u8 pGain){
     u16 crc;
	   GPIO_SetBits(servoDirPort,servoDirPin);
     xl320PGFrame[4]=id;
	   xl320PGFrame[10]=pGain;
	   crc=updateCRC(0,xl320PGFrame,11);
	   xl320PGFrame[11]=(u8)(crc&0xff);
	   xl320PGFrame[12]=(u8)((crc>>8)&0xff);
	   debugFrame(xl320PFrame,13);
	   xlSeriesSendFrame(USART2,xl320PFrame,13);
     Delay_ms(2);
	   GPIO_ResetBits(servoDirPort,servoDirPin);

}

void xlPowerOff(u8 isOn){
	if(isOn){
		
			xlSeriesTorque(0x01,0x00,XL430Torque);
		  xlSeriesTorque(0x02,0x00,XL430Torque);
		  xlSeriesTorque(0x03,0x00,XL320Torque);
		 
		  xlSeriesLed(0x01,0x00,XL430Led);
			xlSeriesLed(0x02,0x00,XL430Led);
			xlSeriesLed(0x03,0x00,XL320Led);

	}else{

		
			xlSeriesTorque(0x01,0x01,XL430Torque);
			xlSeriesTorque(0x02,0x01,XL430Torque);
			xlSeriesTorque(0x03,0x01,XL320Torque);
		 
			xlSeriesLed(0x01,0x01,XL430Led);
			xlSeriesLed(0x02,0x01,XL430Led);
			xlSeriesLed(0x03,0x01,XL320Led); 
	}


}
unsigned short updateCRC(uint16_t crc_accum, uint8_t *data_blk_ptr, uint16_t data_blk_size)
{
  uint16_t i, j;
  static const uint16_t crc_table[256] = { 0x0000,
    0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
    0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027,
    0x0022, 0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D,
    0x8077, 0x0072, 0x0050, 0x8055, 0x805F, 0x005A, 0x804B,
    0x004E, 0x0044, 0x8041, 0x80C3, 0x00C6, 0x00CC, 0x80C9,
    0x00D8, 0x80DD, 0x80D7, 0x00D2, 0x00F0, 0x80F5, 0x80FF,
    0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1, 0x00A0, 0x80A5,
    0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1, 0x8093,
    0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
    0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197,
    0x0192, 0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE,
    0x01A4, 0x81A1, 0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB,
    0x01FE, 0x01F4, 0x81F1, 0x81D3, 0x01D6, 0x01DC, 0x81D9,
    0x01C8, 0x81CD, 0x81C7, 0x01C2, 0x0140, 0x8145, 0x814F,
    0x014A, 0x815B, 0x015E, 0x0154, 0x8151, 0x8173, 0x0176,
    0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162, 0x8123,
    0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
    0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104,
    0x8101, 0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D,
    0x8317, 0x0312, 0x0330, 0x8335, 0x833F, 0x033A, 0x832B,
    0x032E, 0x0324, 0x8321, 0x0360, 0x8365, 0x836F, 0x036A,
    0x837B, 0x037E, 0x0374, 0x8371, 0x8353, 0x0356, 0x035C,
    0x8359, 0x0348, 0x834D, 0x8347, 0x0342, 0x03C0, 0x83C5,
    0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1, 0x83F3,
    0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
    0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7,
    0x03B2, 0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E,
    0x0384, 0x8381, 0x0280, 0x8285, 0x828F, 0x028A, 0x829B,
    0x029E, 0x0294, 0x8291, 0x82B3, 0x02B6, 0x02BC, 0x82B9,
    0x02A8, 0x82AD, 0x82A7, 0x02A2, 0x82E3, 0x02E6, 0x02EC,
    0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2, 0x02D0, 0x82D5,
    0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1, 0x8243,
    0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
    0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264,
    0x8261, 0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E,
    0x0234, 0x8231, 0x8213, 0x0216, 0x021C, 0x8219, 0x0208,
    0x820D, 0x8207, 0x0202 };

  for (j = 0; j < data_blk_size; j++)
  {
    i = ((uint16_t)(crc_accum >> 8) ^ *data_blk_ptr++) & 0xFF;
    crc_accum = (crc_accum << 8) ^ crc_table[i];
  }

  return crc_accum;
}

void debugFrame(u8 *frame,u8 length){
   //	printf("frameData: ");
    for(int i=0;i<length;i++){
		//  printf("%x ",frame[i]);
		}
//    printf("\r\n");
}
