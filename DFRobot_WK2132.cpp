/*!
 * @file DFRobot_IIC_Serial.h
 * @brief 定义 DFRobot_IIC_Serial 类的基础结构,基础方法的实现
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2019-07-16
 * @https://github.com/DFRobot/DFRobot_IIC_Serial
 */
#include <Arduino.h>
#include <DFRobot_WK2132.h>

//DFRobot_IIC_Serial iicSerial;
DFRobot_IIC_Serial::DFRobot_IIC_Serial(TwoWire &wire,  uint8_t subUartChannel, uint8_t addr){
  _pWire = &wire;
  _addr = addr << 3;
  _cacheObject = 0xff;
  _page = 0xff;
  _subSerialChannel = subUartChannel;
  _rxBufferIndex = 0;
 // _rxBufferTail = 0;
  _txBufferIndex = 0;
 // _txBufferTail = 0;
  memset(_rxBuffer, 0, sizeof(_rxBuffer));
  memset(_txBuffer, 0, sizeof(_txBuffer));
}

DFRobot_IIC_Serial::~DFRobot_IIC_Serial(){
  
}

void DFRobot_IIC_Serial::begin(long unsigned baud, uint8_t format, eCommunicationMode_t mode, eLineBreakOutput_t opt){
  _pWire->begin();
  _addr = updateAddr(_addr, SUBUART_CHANNEL_1, OBJECT_REGISTER);
  uint8_t val = 0;
  if(readReg(REG_WK2132_GENA, &val, 1) != 1){
      DBG("READ BYTE SIZE ERROR!");
      return; //ERR_DATA_READ;
  }
  if((val >> 6) != 0x02){
      DBG("");
      return;// ERR_DATA_BUS;
  }
  subSerialConfig(_subSerialChannel);
  DBG("OK");
  setSubSerialBaudRate(_subSerialChannel, baud);
  setSubSerialConfigReg(format, mode, opt);
}
void DFRobot_IIC_Serial::begin(long unsigned baud, uint8_t format, uint8_t mode, uint8_t opt){
  begin(baud, format, (eCommunicationMode_t)mode, (eLineBreakOutput_t)opt);
}

void DFRobot_IIC_Serial::end(){
  //复位
}

int DFRobot_IIC_Serial::available(void){
  int index = 0;
  uint8_t val = 0;
  sFsrReg_t fsr;
 // DBG(_subSerialChannel);
  if(readReg(REG_WK2132_RFCNT, &val, 1) != 1){
      DBG("READ BYTE SIZE ERROR!");
      return -1;
  }
  index = (int)val;
  if(index == 0){
      fsr = readFIFOStateReg();
      if(fsr.rDat == 1){
          index = 256;
      }
  }
  return index;
}

int DFRobot_IIC_Serial::peek(void){
  
  return -1;
}

int DFRobot_IIC_Serial::read(void){
  sFsrReg_t fsr;
  uint8_t val;
  fsr = readFIFOStateReg();
  if(fsr.rDat == 0){
      DBG("FIFO Empty!");
      return -1;
  }
  readReg(REG_WK2132_FDAT, &val, 1);
  return (int)val;
}

size_t DFRobot_IIC_Serial::write(uint8_t value){
  sFsrReg_t fsr;
  fsr = readFIFOStateReg();
  if(fsr.tFull == 1){
      DBG("FIFO full!");
      return -1;
  }
  writeReg(REG_WK2132_FDAT, &value, 1);
  return 1;
}


void DFRobot_IIC_Serial::subSerialConfig(uint8_t subUartChannel){
  DBG("子串口时钟使能");
  subSerialGlobalRegEnable(subUartChannel, clock);
  DBG("软件复位子串口");
  subSerialGlobalRegEnable(subUartChannel, rst);
  DBG("子串口全局中断使能");
  subSerialGlobalRegEnable(subUartChannel, intrpt);
  
  DBG("子串口中断配置");
  sSierReg_t sier = {.rFTrig = 0x01, .rxOvt = 0x01, .tfTrig = 0x01, .tFEmpty = 0x01, .rsv = 0x00, .fErr = 0x01};
  subSerialRegConfig(subUartChannel, page0, REG_WK2132_SIER, &sier);
  DBG("使能发送/接收FIFO");
  sFcrReg_t fcr = {.rfRst = 0x01, .tfRst = 0x00, .rfEn = 0x01, .tfEn = 0x01, .rfTrig = 0x00, .tfTrig = 0x00};
  subSerialRegConfig(subUartChannel, page0, REG_WK2132_FCR, &fcr);
  DBG("子串口接收/发送使能");
  sScrReg_t scr = {.rxEn = 0x01, .txEn = 0x01, .sleepEn = 0x00, .rsv = 0x00 };
  subSerialRegConfig(subUartChannel, page0, REG_WK2132_SCR, &scr);
}

void DFRobot_IIC_Serial::subSerialGlobalRegEnable(uint8_t subUartChannel, eGlobalRegType_t type){
  if(subUartChannel > SUBUART_CHANNEL_ALL)
  {
      DBG("SUBSERIAL CHANNEL NUMBER ERROR!");
      return;
  }
  uint8_t val = 0;
  uint8_t regAddr = getGlobalRegType(type);
  _addr = updateAddr(_addr, SUBUART_CHANNEL_1, OBJECT_REGISTER);
  DBG("reg");DBG(regAddr, HEX);
  if(readReg(regAddr, &val, 1) != 1){
        DBG("READ BYTE SIZE ERROR!");
      return;//ERR_DATA_READ;
  }
  DBG("before:");DBG(val, HEX);
  switch(subUartChannel){
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
  writeReg(regAddr, &val, 1);
  readReg(regAddr, &val, 1);
  DBG("after:");DBG(val, HEX);
}

/*void DFRobot_IIC_Serial::subSerialGlobalRegDisable(uint8_t subUartChannel, eGlobalRegType_t type){
  if(subUartChannel > SUBUART_CHANNEL_ALL)
  {
      DBG("SUBSERIAL CHANNEL NUMBER ERROR!");
      return;
  }
  uint8_t val = 0;
  uint8_t regAddr = getGlobalRegType(type);
  _addr = updateAddr(_addr, subUartChannel, OBJECT_REGISTER);
  if(readReg(regAddr, &val, 1) != 1){
      DBG("READ BYTE SIZE ERROR!");
      return;//ERR_DATA_READ;
  }
  switch(subUartChannel){
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
  writeReg(regAddr, &val, 1);
  readReg(regAddr, &val, 1);
  DBG(val, HEX);
}
*/

void DFRobot_IIC_Serial::subSerialPageSwitch(uint8_t subUartChannel, ePageNumber_t page){
  if((_page == page) || (page < page0) || (page >= pageTotal)){
      return;
  }
  uint8_t val = 0;
  _addr = updateAddr(_addr, subUartChannel, OBJECT_REGISTER);
  if(readReg(REG_WK2132_SPAGE, &val, 1) != 1){
      DBG("READ BYTE SIZE ERROR!");
      return;//ERR_DATA_READ;
  }
  switch(page){
      case page0:
                 val &= 0xFE;
                 break;
      case page1:
                 val |= 0x01;
                 break;
      default:
              break;
  }
  DBG("before: "); DBG(val);
  writeReg(REG_WK2132_SPAGE, &val, 1);
  _page = page;
  readReg(REG_WK2132_SPAGE, &val, 1);
  DBG("after: ");DBG(val, HEX);
}

void DFRobot_IIC_Serial::subSerialRegConfig(uint8_t subUartChannel, ePageNumber_t page, uint8_t reg, void *pValue){
  _addr = updateAddr(_addr, subUartChannel, OBJECT_REGISTER);
  subSerialPageSwitch(subUartChannel, page);
  uint8_t val = 0;
  readReg(reg, &val, 1);
  DBG("before: "); DBG(val);
  val |= *(uint8_t *)pValue;
  writeReg(reg, &val, 1);
  readReg(reg, &val, 1);
  DBG("after: ");DBG(val, HEX);
}

uint8_t DFRobot_IIC_Serial::getGlobalRegType(eGlobalRegType_t type){
  if((type < clock) || (type > intrpt)){
      DBG("Global Reg Type Error!");
      return 0;
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

void DFRobot_IIC_Serial::setSubSerialBaudRate(uint8_t subUartChannel, unsigned long baud){
  uint8_t scr = 0x00,clear = 0x00;
  _addr = updateAddr(_addr, subUartChannel, OBJECT_REGISTER);
  subSerialPageSwitch(subUartChannel, page0);
  readReg(REG_WK2132_SCR, &scr, 1);
  subSerialRegConfig(subUartChannel, page0, REG_WK2132_SCR, &clear);
  uint8_t baud1 = 0,baud0 = 0, baudPres = 0;
  uint16_t valIntger  = FOSC/(baud * 16) - 1;
  uint16_t valDecimal = (FOSC%(baud * 16))/(baud * 16); 
  baud1 = (uint8_t)(valIntger >> 8);
  baud0 = (uint8_t)(valIntger & 0x00ff);
  while(valDecimal > 0x0A){
      valDecimal /= 0x0A;
  }
  baudPres = (uint8_t)(valDecimal);
  subSerialRegConfig(subUartChannel, page1, REG_WK2132_BAUD1, &baud1);
  subSerialRegConfig(subUartChannel, page1, REG_WK2132_BAUD0, &baud0);
  subSerialRegConfig(subUartChannel, page1, REG_WK2132_PRES, &baudPres);
  subSerialRegConfig(subUartChannel, page0, REG_WK2132_SCR, &scr);

  readReg(REG_WK2132_BAUD1, &baud1, 1);
  readReg(REG_WK2132_BAUD0, &baud0, 1);
  readReg(REG_WK2132_PRES, &baudPres, 1);
  DBG(baud1, HEX);
  DBG(baud0, HEX);
  DBG(baudPres, HEX);
}

void DFRobot_IIC_Serial::setSubSerialConfigReg(uint8_t format, eCommunicationMode_t mode, eLineBreakOutput_t opt){
  uint8_t _mode = (uint8_t)mode;
  uint8_t _opt = (uint8_t)opt;
  uint8_t val = 0;
  subSerialPageSwitch(_subSerialChannel, page0);
  _addr = updateAddr(_addr, _subSerialChannel, OBJECT_REGISTER);
  if(readReg(REG_WK2132_LCR, &val, 1) != 1){
      DBG("数据字节读取错误！");
      return;
  }
  DBG("before: "); DBG(val, HEX);
  sLcrReg_t lcr = *((sLcrReg_t *)(&val));
  lcr.format = format;
  lcr.irEn = _mode;
  lcr.lBreak = _opt;
  val = *(uint8_t *)&lcr;
  writeReg(REG_WK2132_LCR, &val, 1);
  readReg(REG_WK2132_LCR, &val, 1);
  DBG("after: "); DBG(val, HEX);
}

uint8_t DFRobot_IIC_Serial::updateAddr(uint8_t pre, uint8_t subUartChannel, uint8_t obj){
  if(((pre&0x01) == obj) && (((pre >> 1)&0x03)== subUartChannel)){
      return pre;
  }
  sIICAddr_t addr ={.type = obj, .uart = subUartChannel, .addrPre = (uint8_t)((int)pre >> 3)};
  DBG(*(uint8_t *)&addr, HEX);
  _cacheObject = obj;
  return *(uint8_t *)&addr;
}

DFRobot_IIC_Serial::sFsrReg_t DFRobot_IIC_Serial::readFIFOStateReg(){
  sFsrReg_t fsr;
  readReg(REG_WK2132_FSR, &fsr, sizeof(fsr));
  return fsr;
}

void DFRobot_IIC_Serial::writeReg(uint8_t reg, const void* pBuf, size_t size){
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

uint8_t DFRobot_IIC_Serial::readReg(uint8_t reg, void* pBuf, size_t size){
  if(pBuf == NULL){
    DBG("pBuf ERROR!! : null pointer");
  }
  uint8_t * _pBuf = (uint8_t *)pBuf;
  _addr &= 0xFE;
  _pWire->beginTransmission(_addr);
  _pWire->write(&reg, 1);
  if(_pWire->endTransmission() != 0){
      return 0;
  }
  _pWire->requestFrom(_addr, (uint8_t) size);
  for(uint16_t i = 0; i < size; i++){
    _pBuf[i] = (char)_pWire->read();
  }
  return size;
}
// uint8_t DFRobot_IIC_Serial::readFifoCache(void* pBuf, size_t size){
   // if(pBuf == NULL){
    // DBG("pBuf ERROR!! : null pointer");
  // }
  // uint8_t * _pBuf = (uint8_t *)pBuf;
  // _addr |=0x01;
  // _pWire->beginTransmission(_addr);
  // _pWire->endTransmission();
  // _pWire->requestFrom(_addr, (uint8_t) size);
  // for(uint16_t i = 0; i < size; i++){DBG();
    // _pBuf[i] = _pWire->read();DBG(_pBuf[i], HEX);
  // }
  // return size;
// }
// void DFRobot_IIC_Serial::witeByte(void *pBuf, size_t size){
  // if(pBuf == NULL){
    // DBG("pBuf ERROR!! : null pointer");
  // }
  // uint8_t * _pBuf = (uint8_t *)pBuf;
  // _addr &= 0xFE;
  // uint8_t val = 0;
 //readReg( REG_WK2132_RFCNT, &val, 1);
  // DBG("rf_before:");DBG(val,HEX);
 // readReg( REG_WK2132_TFCNT, &val, 1);
  // DBG("tx_before:");DBG(val,HEX);
  // uint8_t reg = REG_WK2132_FDAT;
  // for(uint16_t i = 0; i < size; i++){
    // _pWire->beginTransmission(_addr);
    // _pWire->write(&reg, 1);
    // _pWire->write(_pBuf[i]);
    // _pWire->endTransmission();
  // }
    //readReg( REG_WK2132_RFCNT, &val, 1);
    // DBG("rf_after:");DBG(val,HEX);
    //readReg( REG_WK2132_TFCNT, &val, 1);
    // DBG("tx_after:");DBG(val,HEX);
// }
// void DFRobot_IIC_Serial::writeFifo(void* pBuf, size_t size){
  // if(pBuf == NULL){
    // DBG("pBuf ERROR!! : null pointer");
  // }
  // uint8_t val = 0;
  // readReg(REG_WK2132_TFCNT, &val, 1);
  // DBG("tx_before:");DBG(val);
  // uint8_t * _pBuf = (uint8_t *)pBuf;
  // _addr |=0x01;
  // _pWire->beginTransmission(_addr);
  // for(uint16_t i = 0; i < 43; i++){
    // _pWire->write(_pBuf[i]);
  // }
  // _pWire->endTransmission();
  // readReg( REG_WK2132_TFCNT, &val, 1);
  // DBG("tx_after:");DBG(val);
// }

// void DFRobot_IIC_Serial::test(){
    
    
// }