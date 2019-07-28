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
#define REG_WK2132_BAUD1  0x05   //子串口波特率配置寄存器低字节
#define REG_WK2132_PRES   0x06   //子串口波特率配置寄存器小数部分
#define REG_WK2132_RFTL   0x07   //子串口接收FIFO中断触发点配置寄存器
#define REG_WK2132_TFTL   0x08   //子串口发送FIFO中断触发点配置寄存器

#define WK2132_BUFSIZE    256

//typedef void(*MCP23017_INT_CB)(int index);

class DFRobot_WK2132 : public Stream
{
public:
  #define ERR_OK                0      //无错误
  #define ERR_PIN              -1      //引脚编号错误
  #define ERR_DATA_BUS         -1
  #define ERR_DATA_READ        -2      //数据总线读取失败
  #define ERR_ADDR             -3      //I2C地址错误
  #define SUBUART_CHANNEL_1    0x00    //子串口通道1
  #define SUBUART_CHANNEL_2    0x01    //子串口通道2
  #define SUBUART_CHANNEL_ALL  0x02    //所有子通道
  #define OBJECT_REGISTER       0x00    //寄存器对象
  #define OBJECT_FIFO           0x01    //FIFO缓存对象
  #define BUFSIZE             

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
  
  typedef enum{
      clock = 0, /*!< 操作全局控制寄存器，控制子串口时钟 */
	  rst, /*!< 操作全局子串口复位寄存器，从软件上单独复位某个子串口 */
	  intrpt /*!< 操作全局中断寄存器，控制子串口总中断 */
  }eGlobalRegType_t

public:
  /**
   * @brief 构造函数
   * @param pWire I2C总线指针对象，构造设备，可传参数也可不传参数，默认Wire
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
	 @n IIC实际地址:_addr = (addr << 3) | (getPortUart() << 1) | isFIFO()
   */
  DFRobot_WK2132(TwoWire &wire = Wire, uint8_t addr = 0x0E);
  ~DFRobot_WK2132();
  
  /**
   * @brief 初始化函数
   * @return 返回0表示初始化成功，返回其他值表示初始化失败
   */
  void begin(unsigned long);
  void setSubSerial1();
  void setSubSerial2();
  /**
   * @brief 子串口软件复位
   * @param uartN 子串口通道号，可填SUBUART_CHANNEL_1或SUBUART_CHANNEL_2或SUBUART_CHANNEL_ALL
   */
  void subSerialRST(uint8_t uartN);

protected:
  /**
   * @brief 更新模块的I2C地址，每次进行IIC通信都需先更新IIC地址
   * @param pre IIC地址的前5位，从DFRobot_WK2132构造函数中传入
   * @param uartN 子串口通道号，可填SUBUART_CHANNEL_1或SUBUART_CHANNEL_2
   * @param type 要操作的对象，是寄存器还是FIFO，可填OBJECT_REGISTER或OBJECT_FIFO
   * @return 返回值为8位IIC地址
   */
  uint8_t updateAddr(uint8_t pre, uint8_t uartN, uint8_t type);
  /**
   * @brief 子串口时钟使能
   * @param uartN 子串口通道号，可填SUBUART_CHANNEL_1或SUBUART_CHANNEL_2
   */
  void subSerialRegEnable(uint8_t uartN, eGlobalRegType_t type);

  /**
   * @brief 子串口时钟失能
   * @param uartN 子串口通道号，可填SUBUART_CHANNEL_1或SUBUART_CHANNEL_2
   * @param type 全局寄存器类型，可填eGlobalRegType_t的所有枚举值
   */
  void subSerialRegDisable(uint8_t uartN, eGlobalRegType_t type);
  uint8_t getGlobalRegType(eGlobalRegType_t type);
  //uint8_t modifySubSerialbit(eGlobalRegType_t type, );
  
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

private:
  TwoWire *_pWire;
  uint8_t _addr;
  uint8_t rxBuffer1[WK2132_BUFSIZE];
  uint8_t txBuffer1[WK2132_BUFSIZE];
  uint8_t rxBuffer2[WK2132_BUFSIZE];
  uint8_t txBuffer2[WK2132_BUFSIZE];
};
#endif