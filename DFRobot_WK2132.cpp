/*!
 * @file DFRobot_WK2132.h
 * @brief 定义 DFRobot_WK2132 类的基础结构,基础方法的实现
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2019-07-16
 * @https://github.com/DFRobot/DFRobot_WK2132
 */
#include <Arduino.h>
#include <DFRobot_WK2132.h>

DFRobot_WK2132::DFRobot_WK2132(TwoWire &wire, uint8_t addr){
  _pWire = &wire;
  _addr = addr << 3;
  memset(rxBuffer1, 0, sizeof(rxBuffer1));
  memset(txBuffer1, 0, sizeof(txBuffer1));
  memset(rxBuffer2, 0, sizeof(rxBuffer2));
  memset(txBuffer2, 0, sizeof(txBuffer2));
}

DFRobot_WK2132::~DFRobot_WK2132(){
  
}

int DFRobot_WK2132::begin(void){
  _addr = updateAddr(_addr, SUBUART_CHANNEL_1, OBJECT_REGISTER);
  uint8_t val = 0xff;
  if(readReg(REG_WK2132_GENA, &val, 1) != 1){
      DBG("READ BYTE SIZE ERROR!");
      return ERR_DATA_READ;
  }
  if(val != 0x00){
      DBG("");
      return ERR_DATA_BUS;
  }
}

void DFRobot_WK2132::setSubSerial1(){
  /*使能子串口1的时钟*/
  uint8_t val = 0;
  readReg(REG_WK2132_GENA, &val, 1);
  subSerialClockEnable(SUBUART_CHANNEL_1);
  
}

void DFRobot_WK2132::setSubSerial2(){
  
}

 void subSerialRST(uint8_t uartN){
  if(uartN > SUBUART_CHANNEL_ALL)
  {
      DBG("SUBSERIAL CHANNEL NUMBER ERROR!");
      return;
  }
  if(readReg(REG_WK2132_GENA, &val, 1) != 1){
      DBG("READ BYTE SIZE ERROR!");
      return;//ERR_DATA_READ;
  }
  switch(uartN){
      case SUBUART_CHANNEL_1:
                             val |= 0x01;
                             break;
      case SUBUART_CHANNEL_2:
                             val |= 0x02;
                             break;
      default:
              val |= 0x03;
              break;
  }
  writeReg(REG_WK2132_GENA, &val, 1);
 }

uint8_t DFRobot_WK2132::updateAddr(uint8_t pre, uint8_t uartN, uint8_t type){
  sIICAddr_t addr ={.addrPre = pre >> 3, .uart = uartN, .type = type};
  DBG((uint8_t)addr, HEX);
  return *(uint8_t)&addr;
}

void subSerialRegEnable(uint8_t uartN, eGlobalRegType_t type){
  if(uartN > SUBUART_CHANNEL_ALL)
  {
      DBG("SUBSERIAL CHANNEL NUMBER ERROR!");
      return;
  }
  uint8_t val = 0;
  _addr = updateAddr(_addr, SUBUART_CHANNEL_1, OBJECT_REGISTER)
  if(readReg(REG_WK2132_GENA, &val, 1) != 1){
      DBG("READ BYTE SIZE ERROR!");
      return;//ERR_DATA_READ;
  }
  switch(uartN){
      case SUBUART_CHANNEL_1:
                             val |= 0x01;
                             break;
      case SUBUART_CHANNEL_2:
                             val |= 0x02;
                             break;
      default:
              val |= 0x03;
              break;
  }
  writeReg(REG_WK2132_GENA, &val, 1);
}

void DFRobot_WK2132::subSerialRegDisable(uint8_t uartN, eGlobalRegType_t type){
  if(uartN > SUBUART_CHANNEL_ALL)
  {
      DBG("SUBSERIAL CHANNEL NUMBER ERROR!");
      return;
  }
  uint8_t val = 0;
  uint8_t regAddr = getGlobalRegType(type);
  switch(uartN){
      case SUBUART_CHANNEL_1:
                             val &= 0xfE;
                             break;
      case SUBUART_CHANNEL_2:
                             val &= 0xFD;
                             break;
      default:
              val &= 0xFC;
              break;
  }
  writeReg(REG_WK2132_GENA, &val, 1);
}

uint8_t DFRobot_WK2132::getGlobalRegType(eGlobalRegType_t type){
  if((type < clock) || (type > intrpt)){
	  DBG("Global Reg Type Error!");
	  return;
  }
  uint8_t regAddr = 0;
  switch(type){
	  case clock:
	             regAddr = REG_WK2132_GENA;
				 break;
	  case rst:
	             regAddr = REG_WK2132_GRST;
				 break;
	  default:
	          regAddr = REG_WK2132_GIER;
			  break;
  }
  return regAddr;
}

void DFRobot_WK2132::writeReg(uint8_t reg, const void* pBuf, size_t size){
  if(pBuf == NULL){
      DBG("pBuf ERROR!! : null pointer");
  }
  uint8_t * _pBuf = (uint8_t *)pBuf;
  _pWire->beginTransmission(_addr);
  _pWire->write(&reg, 1);

  for(uint16_t i = 0; i < size; i++){
    _pWire->write(_pBuf[i]);
  }
  _pWire->endTransmission();
}

uint8_t DFRobot_WK2132::readReg(uint8_t reg, void* pBuf, size_t size){
  if(pBuf == NULL){
    DBG("pBuf ERROR!! : null pointer");
  }
  uint8_t * _pBuf = (uint8_t *)pBuf;
  _pWire->beginTransmission(_addr);
  _pWire->write(&reg, 1);
  if( _pWire->endTransmission() != 0){
      return 0;
  }
  _pWire->requestFrom(_addr, (uint8_t) size);
  for(uint16_t i = 0; i < size; i++){
    _pBuf[i] = _pWire->read();
  }
  _pWire->endTransmission();
  return size;
}
