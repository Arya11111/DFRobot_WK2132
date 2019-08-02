/*!
 * @file DFRobot_WK2132.h
 * @brief 定义 DFRobot_WK2132 类的基础结构
 * @n 这是一个IIC转双串口的库，IIC最高速率为1Mbps;
 * @n 每个子通道UART的波特率、字长、校验格式可以独立设置，最高可以提供2Mbps的通信速率;
 * @n 每个子通道可以独立设置工作在IrDA红外通信;
 * @n 每个子通道具备收/发独立的256字节FIFO(先进先出队列缓存)，且FIFO的中断可按用户需求进行编程出发点且具备超时中断功能;
 * @n 主接口IIC最大传输速率为1Mbps
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2019-07-28
 * @https://github.com/DFRobot/DFRobot_MCP23017
 */
#ifndef __DFRobot_WK2132_H
#define __DFRobot_WK2132_H

#include <Arduino.h>
#include <inttypes.h>
#include "Stream.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <Wire.h>

//若想打开调试宏可将0改为1，查看程序调试信息
#if 0
#define DBG(...) {Serial.print("["); Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#else
#define DBG(...)
#endif

/*全局寄存器*/
#define REG_WK2132_GENA   0x00   //全局控制寄存器，控制子串口时钟
#define REG_WK2132_GRST   0x01   //全局子串口复位寄存器，从软件上单独复位某个子串口
#define REG_WK2132_GMUT   0x01   //全局主串口控制寄存器，当主串口为串口时才会使用该寄存器，本程序无需配置
#define REG_WK2132_GIER   0x10   //全局中断寄存器，控制子串口总中断
#define REG_WK2132_GIFR   0x11   //全局中断标志寄存器，只读(OR)寄存器，指示某个子串口是否发生中断
/*页控制寄存器*/
#define REG_WK2132_SPAGE  0x03   //子串口页控制寄存器
/*子串口寄存器 SPAGE0*/
#define REG_WK2132_SCR    0x04   //子串口控制寄存器
#define REG_WK2132_LCR    0x05   //子串口配置寄存器
#define REG_WK2132_FCR    0x06   //子串口FIFO控制寄存器
#define REG_WK2132_SIER   0x07   //子串口中断使能寄存器
#define REG_WK2132_SIFR   0x08   //子串口中断标志寄存器
#define REG_WK2132_TFCNT  0x09   //子串口发送FIFO计数寄存器，只读(OR)寄存器
#define REG_WK2132_RFCNT  0x0A   //子串口接收FIFO计数寄存器，只读(OR)寄存器
#define REG_WK2132_FSR    0x0B   //子串口FIFO状态寄存器，只读(OR)寄存器
#define REG_WK2132_LSR    0x0C   //子串口接收状态寄存器，只读(OR)寄存器
#define REG_WK2132_FDAT   0x0D   //子串口FIFO数据寄存器
/*子串口寄存器 SPAGE1*/
#define REG_WK2132_BAUD1  0x04   //子串口波特率配置寄存器高字节
#define REG_WK2132_BAUD0  0x05   //子串口波特率配置寄存器低字节
#define REG_WK2132_PRES   0x06   //子串口波特率配置寄存器小数部分
#define REG_WK2132_RFTL   0x07   //子串口接收FIFO中断触发点配置寄存器
#define REG_WK2132_TFTL   0x08   //子串口发送FIFO中断触发点配置寄存器
#define SUBUART_CHANNEL_1    0x00    //子串口通道1
#define SUBUART_CHANNEL_2    0x01    //子串口通道2
#define SUBUART_CHANNEL_ALL  0x11    //所有子通道

#define IIC_SERIAL_RX_BUFFER_SIZE    32
#define IIC_SERIAL_TX_BUFFER_SIZE    32

//数据格式:N表示无校验位，Z表示0校验，O表示奇校验, E表示偶校验，F表示偶校验。前面一个数字表示发送数据的位数，后面一个数字表示停止位数
#define IIC_SERIAL_8N1    0x00
#define IIC_SERIAL_8N2    0x01
#define IIC_SERIAL_8Z1    0x08
#define IIC_SERIAL_8Z2    0x09
#define IIC_SERIAL_8O1    0x0A
#define IIC_SERIAL_8O2    0x0B
#define IIC_SERIAL_8E1    0x0C
#define IIC_SERIAL_8E2    0x0D
#define IIC_SERIAL_8F1    0x0E
#define IIC_SERIAL_8F2    0x0F


#ifdef ARDUINO_ARCH_NRF5
class DFRobot_IIC_Serial : public _Stream{
#else
class DFRobot_IIC_Serial : public Stream{
#endif
public:
  #define FOSC                11059851L //外部晶振频率11.059851MHz
  #define ERR_OK                0      //无错误
  #define ERR_PIN              -1      //引脚编号错误
  #define ERR_DATA_BUS         -1
  #define ERR_DATA_READ        -2      //数据总线读取失败
  #define ERR_ADDR             -3      //I2C地址错误
  #define OBJECT_REGISTER      0x00    //寄存器对象
  #define OBJECT_FIFO          0x01    //FIFO缓存对象
  #define FSR_FLAG_ERR         0X01
  


  /*
   WK2132的IIC地址结构：
     * -------------------------------------------------------------------------
     * |   b7   |   b6   |   b5   |   b4   |   b3   |   b2   |   b1   |   b0   |
     * -------------------------------------------------------------------------
     * |   0    |   A1   |   A0   |   1    |   0    |   C1   |   C0   |   0/1  |
     * -------------------------------------------------------------------------
     * |                    addrPre                 |       uart      |   type |
     * -------------------------------------------------------------------------
     *上电后，ready位默认为1，不可更改
  */
  typedef struct{
      uint8_t  type: 1; /*!< IIC地址的第0位，取值与操作的是寄存器还是FIFO缓存有关，0表示寄存器，1表示FIFO */
      uint8_t  uart: 2; /*!< IIC地址的第2/1位，取值与操作的子串口通道有关，子串口通道1:0x00，子串口通道2:0x01 */
      uint8_t  addrPre: 5; /*!< IIC地址的前5位，其中第7/6位可根据外部拨码开关A1A0选择值 */
  } __attribute__ ((packed)) sIICAddr_t;
  
  /*
   WK2132子串口中断使能寄存器SIER描述:
     * --------------------------------------------------------------------------------------------
     * |    b7    |   b6   |   b5   |   b4   |      b3     |     b2     |     b1     |     b0     |
     * --------------------------------------------------------------------------------------------
     * | FERR_IEN |          RSV             | TFEMPTY_IEN | TFTRIG_IEN | RXOVT_IEN  | RFTRIG_IEN |
     * --------------------------------------------------------------------------------------------
  */
  typedef struct{
      uint8_t rFTrig : 1; /*!< 接收FIFO触点中断使能位，1-使能，0-禁止 */
      uint8_t rxOvt : 1; /*!< 接收FIFO超时中断使能位，1-使能，0-禁止 */
      uint8_t tfTrig : 1; /*!< 发送FIFO触点中断使能位，1-使能，0-禁止 */
      uint8_t tFEmpty : 1; /*!< 发送FIFO空中断使能位，1-使能，0-禁止 */
      uint8_t rsv : 3; /*!< 保留位 */
      uint8_t fErr: 1; /*!< 接收FIFO数据错误中断使能位，1-使能，0-禁止 */
  } __attribute__ ((packed)) sSierReg_t; 
  
  /*
   WK2132子串口FIFO控制寄存器FCR描述:
     * -------------------------------------------------------------------------
     * |   b7   |   b6   |   b5   |   b4   |   b3   |   b2   |   b1   |   b0   |
     * -------------------------------------------------------------------------
     * |      TFTRIG     |      RFTRIG     |  TFEN  |  RFEN  |  TFRST |  RFRST |
     * -------------------------------------------------------------------------
  */
  typedef struct{
      uint8_t rfRst : 1; /*!< 子串口接收FIFO复位位，1-复位FIFO，0-未使能复位，写1复位，完成后自动置0 */
      uint8_t tfRst : 1; /*!< 子串口发送FIFO复位位，1-复位FIFO，0-未使能复位，写1复位，完成后自动置0 */
      uint8_t rfEn : 1; /*!< 子串口接收FIFO使能位，1-使能，0-不使能 */
      uint8_t tfEn : 1; /*!< 子串口发送FIFO使能位，1-使能，0-不使能 */
      uint8_t rfTrig : 2; /*!< 子串口接收FIFO触点设置位，00-8Byte(默认),01-16Byte,10-24Byte,11-18Byte */
      uint8_t tfTrig: 2; /*!< 子串口接收FIFO触点设置位，00-8Byte(默认),01-16Byte,10-24Byte,11-18Byte */
  } __attribute__ ((packed)) sFcrReg_t; 
  
  /*
   WK2132子串口控制寄存器SCR描述:
     * -------------------------------------------------------------------------
     * |   b7   |   b6   |   b5   |   b4   |   b3   |   b2   |   b1   |   b0   |
     * -------------------------------------------------------------------------
     * |                     RSV                    | SLEEPEN|  TXEN  |  RXEN  |
     * -------------------------------------------------------------------------
  */
  typedef struct{
      uint8_t rxEn : 1; /*!< 子串口接收使能位，1-使能，0-不使能 */
      uint8_t txEn : 1; /*!< 子串口发送使能位，1-使能，0-不使能 */
      uint8_t sleepEn : 1; /*!< 子串口休眠使能位，1-使能，0-不使能 */
      uint8_t rsv : 5; /*!< 保留位 */
  } __attribute__ ((packed)) sScrReg_t;
  /*
   WK2132子串口配置寄存器LCR描述:
     * -------------------------------------------------------------------------
     * |   b7   |   b6   |   b5   |   b4   |   b3   |   b2   |   b1   |   b0   |
     * -------------------------------------------------------------------------
     * |        RSV      |  BREAK |  IREN  |  PAEN  |      PAM        |  STPL  |
     * -------------------------------------------------------------------------
  */
  typedef struct{
      uint8_t format: 4;/*!< 子串口数据格式，由:
                           PAEN 子串口校验使能位，1-有校验位(9位数据)，0-无校验位（8位数据）), 
                           PAM 子串口校验模式选择位，当PAEN位为1时生效，00-0校验（默认），01-奇校验，10-偶校验，11-1校验
                           STPL 子串口停止位长度 0-1bit，1-2bits等设置*/
      uint8_t irEn : 1; /*!< 子串口红外使能位，0-普通模式，1-红外模式 */
      uint8_t lBreak: 1; /*!< 子串口Line-Break输出控制位，0-正常输出，1-Line-Break输出（TX强制输出0） */
      uint8_t rsv : 2; /*!< 保留位 */
  } __attribute__ ((packed)) sLcrReg_t;
  
  /*
   WK2132子串口FIFO状态寄存器FSR:
     * -------------------------------------------------------------------------
     * |   b7   |   b6   |   b5   |   b4   |   b3   |   b2   |   b1   |   b0   |
     * -------------------------------------------------------------------------
     * |  RFOE  |  RFBI  |  RFFE  |  RFPE  |  RDAT  |  TDAT  |  TFULL |  TBUSY |
     * -------------------------------------------------------------------------
  */
  typedef struct{
      uint8_t tBusy : 1; /*!< 子串口发送TX忙标志位，0-子串口发送TX空，1-子串口发送TX忙 */
      uint8_t tFull : 1; /*!< 子串口发送TX满标志位，0-子串口发送FIFO未满，1-子串口发送FIFO满 */
      uint8_t tDat : 1; /*!< 子串口发送FIFO空标志，0-子串口发送FIFO空，1-子串口发送FIFO未空 */
      uint8_t rDat : 1; /*!< 子串口接收FIFO空标志，0-子串口接收FIFO空，1-子串口接收FIFO未空 */
      uint8_t rFpe : 1; /*!< 子串口接收FIFO中数据校验错误标志位，0-无PE错误，1-有PE错误 */
      uint8_t rFfe : 1; /*!< 子串口接收FIFO中数据帧错误标志位，0-无FE错误，1-有FE错误 */
      uint8_t rFbi : 1; /*!< 子串口接收FIFO中数据有Line-Break错误，0-无Line-Break错误，1-有Line-Break错误 */
      uint8_t rFoe : 1; /*!< 子串口接收FIFO中数据溢出出错标志位，0-无OE错误，1-有OE错误 */
  } __attribute__ ((packed)) sFsrReg_t;

  
  typedef enum{
      clock = 0, /*!< 操作全局控制寄存器，控制子串口时钟 */
      rst, /*!< 操作全局子串口复位寄存器，从软件上单独复位某个子串口 */
      intrpt /*!< 操作全局中断寄存器，控制子串口总中断 */
  }eGlobalRegType_t;
  
  typedef enum{
      page0 = 0,
      page1,
      pageTotal
  }ePageNumber_t;

  typedef enum{
      eNormalMode,
      eIrDAMode
  }eCommunicationMode_t;
  
  typedef enum{
      eNormal,
      eLineBreak
  }eLineBreakOutput_t;

public:
  /**
   * @brief 构造函数
   * @param pWire I2C总线指针对象，构造设备，可传参数也可不传参数，默认Wire
   * @param subUartChannel 子串口通道号，WK2132有2个子串口，可填SUBUART_CHANNEL_1或SUBUART_CHANNEL_2
   * @param addr 8位I2C地址的前5位右移3位，取值(0x02/0x06/0x0A/0x0E)默认0x27
     @n IIC地址由构成如下图所示
     7  6  5  4  3  2  1   0
     0  A1 A0 1  0  C1 C0 0/1
     @n 第7/4/3位为固定值，分别为0/1/0
     @n 第6/5位的值由拨码开关来确定，范围为（00~11），默认为11
     @n 第2/1位的值为子串口通道，00表示子串口1，01表示子串口2
     @n 第0位表示是读写寄存器还是读写FIFO缓存，0表示读寄存器，1表示读FIFO缓存
     @n 综上所述，此串口扩展模块的IIC地址构成为0 A1 A0 1 0 C1 C0 0/1
     @n 构造函数中的addr指的是第7~3位右移3位，如下表所示：默认为0x0E
     0  0  0  0  | A1 A0 1  0
     0  0  0  0  | 1  1  1  0    0x0E
     0  0  0  0  | 1  0  1  0    0x0A
     0  0  0  0  | 0  1  1  0    0x06
     0  0  0  0  | 0  0  1  0    0x02
     @n 模块实际IIC地址与子串口通道编号和是操作寄存器还是FIFO有关
     @n IIC实际地址:_addr = (addr << 3) | SUBUART_CHANNEL_N | OBJECT_REGISTER/OBJECT_FIFO
   */
  DFRobot_IIC_Serial(TwoWire &wire = Wire, uint8_t subUartChannel = SUBUART_CHANNEL_1, uint8_t addr = 0x0E);
  ~DFRobot_IIC_Serial();

  /**
   * @brief 初始化函数，设置子串口的波特率
   * @param baud 串口波特率
   */
  void begin(long unsigned baud){begin(baud, IIC_SERIAL_8N1, eNormalMode, eNormal);};
  /**
   * @brief 初始化函数，设置子串口的波特率，数据格式
   * @param baud 串口波特率
   * @param format 子串口数据格式，可填IIC_SERIAL_8N1、IIC_SERIAL_8N2、IIC_SERIAL_8Z1
   * @n IIC_SERIAL_8Z2、IIC_SERIAL_8O1、IIC_SERIAL_8O2、IIC_SERIAL_8E1、IIC_SERIAL_8E2
   * @n IIC_SERIAL_8F1、IIC_SERIAL_8F2等参数
   */
  void begin(long unsigned baud, uint8_t format){begin(baud, format, eNormalMode, eNormal);};
   /**
   * @brief 初始化函数，设置子串口的波特率，数据格式，通信模式
   * @param baud 串口波特率
   * @param format 子串口数据格式，可填IIC_SERIAL_8N1、IIC_SERIAL_8N2、IIC_SERIAL_8Z1
   * @n IIC_SERIAL_8Z2、IIC_SERIAL_8O1、IIC_SERIAL_8O2、IIC_SERIAL_8E1、IIC_SERIAL_8E2
   * @n IIC_SERIAL_8F1、IIC_SERIAL_8F2等参数
   * @param mode 自串口通信模式，可设置为红外模式（1）或普通模式（0），可填eCommunicationMode_t的所有枚举值，或0或1
   */
  void begin(long unsigned baud, uint8_t format, eCommunicationMode_t mode){begin(baud, format, mode, eNormal);};
  void begin(long unsigned baud, uint8_t format, uint8_t mode){begin(baud, format, mode, eNormal);};
  /**
   * @brief 初始化函数，设置子串口的波特率，数据格式，通信模式，和Line-Break输出
   * @param baud 串口波特率
   * @param format 子串口数据格式，可填IIC_SERIAL_8N1、IIC_SERIAL_8N2、IIC_SERIAL_8Z1
   * @n IIC_SERIAL_8Z2、IIC_SERIAL_8O1、IIC_SERIAL_8O2、IIC_SERIAL_8E1、IIC_SERIAL_8E2
   * @n IIC_SERIAL_8F1、IIC_SERIAL_8F2等参数
   * @param mode 自串口通信模式，可设置为红外模式（1）或普通模式（0），可填eCommunicationMode_t的所有枚举值，或0或1
   * @param opt 子串口Line-Break输出控制位，可设置为正常输出（0）和Line-Break输出（1），可填eLineBreakOutput_t的所有枚举值，或0或1
   */
  void begin(long unsigned baud, uint8_t format, eCommunicationMode_t mode, eLineBreakOutput_t opt);
  void begin(long unsigned baud, uint8_t format, uint8_t mode, uint8_t opt);

  void end();
  virtual int available(void);
  virtual int peek(void);
  virtual int read(void);
  virtual void flush(void){}
  virtual size_t write(uint8_t);
  inline size_t write(unsigned long n) { return write((uint8_t)n); }
  inline size_t write(long n) { return write((uint8_t)n); }
  inline size_t write(unsigned int n) { return write((uint8_t)n); }
  inline size_t write(int n) { return write((uint8_t)n); }
  using Print::write; // pull in write(str) and write(buf, size) from Print
  operator bool() { return true; }

  //Interrupt handlers - Not intended to be called externally
  // inline void _rx_complete_irq(void);
  // void _tx_udr_empty_irq(void);
  // uint8_t readFifoCache(void* pBuf, size_t size);
  // void witeByte(void *pBuf, size_t size);
  // void writeFifo(void* pBuf, size_t size);

protected:
  /**
   * @brief 子串口参数配置
   * @param subUartChannel 子串口通道号,可填SUBUART_CHANNEL_1或SUBUART_CHANNEL_2
   */
  void subSerialConfig(uint8_t subUartChannel);

  /**
   * @brief 子串口全局寄存器使能
   * @param subUartChannel 子串口通道号，可填SUBUART_CHANNEL_1或SUBUART_CHANNEL_2
   * @type 全局寄存器类型，可填eGlobalRegType_t的所有枚举值
   */
  void subSerialGlobalRegEnable(uint8_t subUartChannel, eGlobalRegType_t type);

  /**
   * @brief 子串口全局寄存器禁止
   * @param subUartChannel 子串口通道号，可填SUBUART_CHANNEL_1或SUBUART_CHANNEL_2
   * @param type 全局寄存器类型，可填eGlobalRegType_t的所有枚举值
   */
  //void subSerialGlobalRegDisable(uint8_t subUartChannel, eGlobalRegType_t type);

  /**
   * @brief 子串口寄存器配置，如SIER、FCR、LCR寄存器的配置等等
   * @param subUartChannel 子串口通道号，可填SUBUART_CHANNEL_1或SUBUART_CHANNEL_2
   * @param page 页序号，可填ePageNumber_t的所有枚举值
   * @param reg 寄存器地址
   * @param pValue 数据的存放缓存，1个字节
   */
  void subSerialRegConfig(uint8_t subUartChannel, ePageNumber_t page, uint8_t reg, void *pValue);
  /**
   * @brief 获取全局寄存器地址
   * @param type 全局寄存器类型，可填eGlobalRegType_t的所有枚举值
   * @return 返回寄存器的地址
   */
  uint8_t getGlobalRegType(eGlobalRegType_t type);
  
  /**
   * @brief 设置子串口波特率
   * @param subUartChannel 子串口通道号，可填SUBUART_CHANNEL_1或SUBUART_CHANNEL_2
   * @param baud 波特率
   */
  void setSubSerialBaudRate(uint8_t subUartChannel, unsigned long baud);
  /**
   * @brief 设置子串口配置寄存器
   * @param format 子串口数据格式，可填IIC_SERIAL_8N1、IIC_SERIAL_8N2、IIC_SERIAL_8Z1
   * @n IIC_SERIAL_8Z2、IIC_SERIAL_8O1、IIC_SERIAL_8O2、IIC_SERIAL_8E1、IIC_SERIAL_8E2
   * @n IIC_SERIAL_8F1、IIC_SERIAL_8F2等参数
   * @param mode 自串口通信模式，可设置为红外模式或普通模式，可填eCommunicationMode_t的所有枚举值
   * @param opt 子串口Line-Break输出控制位，可设置为正常输出和Line-Break输出，可填eLineBreakOutput_t的所有枚举值
   */
  void setSubSerialConfigReg(uint8_t format, eCommunicationMode_t mode, eLineBreakOutput_t opt);

   /**
   * @brief 子串口寄存器页切换
   * @param subUartChannel 子串口通道号，可填SUBUART_CHANNEL_1或SUBUART_CHANNEL_2
   * @param page 页序号，可填ePageNumber_t的所有枚举值
   * @param type 全局寄存器类型，可填eGlobalRegType_t的所有枚举值
   */
  void subSerialPageSwitch(uint8_t subUartChannel, ePageNumber_t page);

  /**
   * @brief 更新模块的I2C地址，每次进行IIC通信都需先更新IIC地址
   * @param pre IIC地址的前5位，从DFRobot_WK2132构造函数中传入
   * @param subUartChannel 子串口通道号，可填SUBUART_CHANNEL_1或SUBUART_CHANNEL_2
   * @param obj 要操作的对象，是寄存器还是FIFO，可填OBJECT_REGISTER或OBJECT_FIFO
   * @return 返回值为8位IIC地址
   */
  uint8_t updateAddr(uint8_t pre, uint8_t subUartChannel, uint8_t obj);
  /**
   * @brief 读取FIFO状态寄存器
   * @return 返回值为sFsrReg_t结构体对象的值
   */
  sFsrReg_t readFIFOStateReg();

  /**
   * @brief 写寄存器函数
   * @param reg  寄存器地址 8bits
   * @param pBuf 要写入数据的存放缓存
   * @param size 要写入数据的长度
   */
  void writeReg(uint8_t reg, const void* pBuf, size_t size);
  /**
   * @brief 读寄存器函数
   * @param reg  寄存器地址 8bits
   * @param pBuf 要读取数据的存放缓存
   * @param size 要读取数据的长度
   * @return 返回实际读取的长度，返回0表示读取失败
   */
  uint8_t readReg(uint8_t reg, void* pBuf, size_t size);
  //void test();
  

private:
  TwoWire *_pWire;
  uint8_t _addr;
  uint8_t _cacheObject;
  uint8_t _page;
  uint8_t _subSerialChannel;
  uint8_t _rxBufferIndex;
  //uint8_t _rxBufferTail;
  uint8_t _txBufferIndex;
  //uint8_t _txBufferTail;
  unsigned char _rxBuffer[IIC_SERIAL_RX_BUFFER_SIZE];
  unsigned char _txBuffer[IIC_SERIAL_TX_BUFFER_SIZE];
};
//extern DFRobot_IIC_Serial iicSerial;
#endif