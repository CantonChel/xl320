#ifndef __XL430_H
#define	__XL430_H

#include "stm32f4xx.h"
#include "usart.h"

#define XL320Led                 0x19
#define XL320Torque              0x18
#define XL320LPosition           0x1e

#define XL430Led                 0x41
#define XL430Torque              0x40
#define XL430LPosition           0x74

#define servoDirClk     RCC_AHB1Periph_GPIOC
#define servoDirPin     GPIO_Pin_3
#define servoDirPort     GPIOC

#define xl430Step                0.08789
#define xl320Step                0.29


#pragma pack (1)
typedef struct Xseries {
	u8 id;
	u16 length;
	u8 inst;
	u8 error;
	u32 position;
	u8 crc1;
	u8 crc2;
} XPositionInfo;

typedef struct X320series {
	u8 id;
	u16 length;
	u8 inst;
	u8 error;
	u16 position;
	u8 crc1;
	u8 crc2;
} X320PositionInfo;

extern XPositionInfo *xl430_1;
extern XPositionInfo *xl430_2;
extern X320PositionInfo *xl320_1;
extern u8 xl430_1_data[11];
extern u8 xl430_2_data[11];
extern u8 xl320_1_data[9];

void xlSeriesStart(void);
void xl430SendPosition(u8 id,u16 position);
void xl320SendPosition(u8 id,u16 position);
void xlSeriesSendFrame(USART_TypeDef * pUSARTx,u8 * frame,u8 length);
void xlSeriesTorque(u8 id,u8 on,u8 address);
void xlSeriesLed(u8 id,u8 on,u8 address);
unsigned short updateCRC(uint16_t crc_accum, uint8_t *data_blk_ptr, uint16_t data_blk_size);
void debugFrame(u8 *frame,u8 length);
void xl430SendPGain(u8 id,u16 pGain);
void xl320SendPGain(u8 id,u8 pGain);
void xl430ReadPosition(u8 id);
void xl320ReadPosition(u8 id);
void xlPowerOff(u8 isOn);

#endif 
