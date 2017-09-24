//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#include <stm32f10x_gpio.h>
#include <stm32f10x_i2c.h>
#include <stm32f10x_rcc.h>
#include "i2cm.h"


void i2cm_init(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  //I2C
  I2C_Cmd(I2C1, DISABLE);
  I2C_DeInit(I2C1);
  I2C_InitTypeDef i2c_InitStruct;
  i2c_InitStruct.I2C_Mode = I2C_Mode_I2C;
  i2c_InitStruct.I2C_ClockSpeed = 10000;
  i2c_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  i2c_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
  i2c_InitStruct.I2C_Ack = I2C_Ack_Enable;
  i2c_InitStruct.I2C_OwnAddress1 = 0;
  I2C_Cmd(I2C1, ENABLE);
  I2C_Init(I2C1, &i2c_InitStruct);
  
  //GPIO
  GPIO_InitTypeDef InitStruct;
  InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
  InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  
  GPIO_Init(GPIOB, &InitStruct);
}



int8_t i2cm_Start(uint8_t slave_addr, uint8_t IsRead, uint16_t TimeOut){
  uint16_t TOcntr;
  I2C_GenerateSTART(I2C1, ENABLE);
  TOcntr = TimeOut;
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && TOcntr) {TOcntr--;}
  if (!TOcntr)
    return I2C_ERR_HWerr;
  if (IsRead){
    I2C_Send7bitAddress(I2C1, slave_addr << 1, I2C_Direction_Receiver);
    TOcntr = TimeOut;
    while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && TOcntr) {TOcntr--;}
  }
  else
  {
    I2C_Send7bitAddress(I2C1, slave_addr << 1, I2C_Direction_Transmitter);
    TOcntr = TimeOut;
    while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && TOcntr) {TOcntr--;}
  }
  
  if (!TOcntr)
      return I2C_ERR_NotConnect;
  
  return I2C_ERR_Ok;
}



int8_t i2cm_Stop(uint16_t TimeOut){
  I2C_GenerateSTOP(I2C1, ENABLE);
  uint16_t TOcntr = TimeOut;
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF) && TOcntr);
  if (!TOcntr)
    return I2C_ERR_HWerr;
  
  return I2C_ERR_Ok;
}


int8_t i2cm_WriteBuff(uint8_t *pbuf, uint16_t len, uint16_t TimeOut){
  uint16_t TOcntr;
  
  while (len--){
    I2C_SendData(I2C1, *(pbuf++));
    TOcntr = TimeOut;
    while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && TOcntr) {TOcntr--;}
    if (!TOcntr)
      return I2C_ERR_NotConnect;
  }
  
  return I2C_ERR_Ok;
}



int8_t i2cm_ReadBuffAndStop(uint8_t *pbuf, uint16_t len, uint16_t TimeOut){
  uint16_t TOcntr;
  I2C_AcknowledgeConfig(I2C1, ENABLE);

  while (len-- != 1){
    TOcntr = TimeOut;
    while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) ) && TOcntr) {TOcntr--;}
    *pbuf++ = I2C_ReceiveData(I2C1);
  }

  I2C_AcknowledgeConfig(I2C1, DISABLE);
  I2C_GenerateSTOP(I2C1,ENABLE);

  TOcntr = TimeOut;
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) ) && TOcntr) {TOcntr--;}
  *pbuf++ = I2C_ReceiveData(I2C1);
  i2cm_Stop(TimeOut);
  return I2C_ERR_Ok;
}
