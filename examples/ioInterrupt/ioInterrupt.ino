/*!
 * @file ioInterrupt.ino
 * @brief IO中断，将IO扩展板的某组端口（A组或B组）的某个引脚设置为中断模式,当对应组端口发生中断时，引脚INTA（对应A组端口）或INTB（对应B组端口）将输出一个高电平
 * INTA用来检测端口eGPA的引脚是否发生中断,INTB用来检测端口eGPB的引脚是否发生中断;将INTA和INTB引脚分别连接到主控的外部中断0和外部中断1
 * @n 实验现象：当主控检测到INTA或INTB引脚信号发生变化时，执行相应的中断服务函数，从而串口打印出是哪个引脚发生了中断
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @eGPAte  2019-07-18
 * @get from https://www.dfrobot.com
 * @url https://github.com/DFRobot/DFRobot_MCP23017
 */

#include <DFRobot_MCP23017.h>
/*DFRobot_MCP23017构造函数
 *参数&wire 可填TwoWire对象Wire
 *参数addr  如下I2C地址可用0x20~0x27，拨码开关A2、A1、A0与I2C地址对应关系如下所示（默认0x27）：
  * 0  0  1  0  | 0  A2 A1 A0
    0  0  1  0  | 0  1  1  1    0x27
    0  0  1  0  | 0  1  1  0    0x26
    0  0  1  0  | 0  1  0  1    0x25
    0  0  1  0  | 0  1  0  0    0x24
    0  0  1  0  | 0  0  1  1    0x23
    0  0  1  0  | 0  0  1  0    0x22
    0  0  1  0  | 0  0  0  1    0x21
    0  0  1  0  | 0  0  0  0    0x20
 */
DFRobot_MCP23017 mcp(Wire, /*addr =*/0x27);//构造函数，地址可通过拨码开关更改A2A1A0的高低电平，实现硬件更改地址，范围0x20~0x27
//DFRobot_MCP23017 mcp;//这样定义会使用默认参数， Wire  0x27(默认I2C地址)

//将2个按钮分别连接到IO扩展板端口eGPA的某个引脚(例：eGPA0)和端口eGPB的某个引脚(例：eGPB0)
//将INTA连接到UNO的外部中断0引脚上，INTB连接到UNO的外部中断1引脚上

bool intFlagA = false;//INTA中断标志
bool intFlagB = false;//INTB中断标志

/*中断服务函数，原型为 void func(int index),其中index代表那个引脚发生中断*/
void gpa0CB(int index){
  /*pinDescription函数用来将某个引脚转换为字符串描述
  参数pin 如下参数都是可用的
  eGPA0  eGPA1  eGPA2  eGPA3  eGPA4  eGPA5  eGPA6  eGPA7
   0    1    2    3    4    5    6    7
  eGPB0  eGPB1  eGPB2  eGPB3  eGPB4  eGPB5  eGPB6  eGPB7
   8    9   10   11   12   13   14   15
  */
  String description = mcp.pinDescription(index);
  Serial.print(description);Serial.println(" Interruption occurs!");
}

void gpb7CB(int index){
  String description = mcp.pinDescription(index);
  Serial.print(description);Serial.println(" Interruption occurs!");
}

void setup() {
  Serial.begin(115200);
  #ifdef ARDUINO_ARCH_MPYTHON 
  pinMode(P0, INPUT);//使用掌控外部中断,INTA连接到掌控P0引脚
  pinMode(P1, INPUT);//使用掌控外部中断,INTB连接到掌控P1引脚
  #else
  pinMode(2, INPUT);//使用UNO的外部中断0
  pinMode(3, INPUT);//使用UNO的外部中断1
  #endif

  /*在这里一致等到芯片初始化完成才能退出*/
  while(mcp.begin() != 0){
    Serial.println("Initialization of the chip failed, please confirm that the chip connection is correct!");
    delay(1000);
  }
  /*pinModeInterrupt函数用于将引脚设置中断模式，该函数会自动将引脚设置为输入模式
  参数pin 如下参数都是可用的：
  eGPA0  eGPA1  eGPA2  eGPA3  eGPA4  eGPA5  eGPA6  eGPA7
    0      1      2      3      4      5      6      7
  eGPB0  eGPB1  eGPB2  eGPB3  eGPB4  eGPB5  eGPB6  eGPB7
    8      9      10     11     12     13     14     15
  参数mode 如下参数是可用的：
  eLowLevel    eHighLevel    eRising    eFalling    eChangeLevel
  低电平中断   低电平中断    上升沿跳变 下降沿跳变  双边沿跳变中断
  参数cb 中断服务函数(带参函数)
  原型void func(int)
  */
  mcp.pinModeInterrupt(/*pin = */mcp.eGPA0, /*mode = */mcp.eHighLevel, /*cb = */gpa0CB);//数字引脚0(eGPA0)，高电平中断，当引脚0的状态为高电平时产生中断，INTA输出高电平
  mcp.pinModeInterrupt(/*pin = */mcp.eGPB7, /*mode = */mcp.eChangeLevel, /*cb = */gpb7CB);//数字引脚15(eGPB7)，双边沿跳变中断，当引脚15的状态改变时产生中断，INTB输出高电平

  #ifdef ARDUINO_ARCH_MPYTHON  //Microbit这里需要做一些单独处理吗
  /* 掌控 中断引脚与终端号码对应关系表
   * -------------------------------------------------------------------------------------
   * |                    |  DigitalPin  |        P0~P20均可作为外部中断使用             |
   * |    掌控            |--------------------------------------------------------------|
   * |                    | Interrupt No |  可用digitalPinToInterrupt(Pn) 查询中断号     |
   * |-----------------------------------------------------------------------------------|
   */
  attachInterrupt(digitalPinToInterrupt(P0)/*查询P0引脚的中断号*/,notifyA,RISING);//开启掌控P0引脚的外部中断，上升沿触发，INTA连接P0
  attachInterrupt(digitalPinToInterrupt(P1)/*查询P1引脚的中断号*/,notifyB,RISING);//开启掌控P1引脚的外部中断，上升沿触发，INTA连接P1
  #else
  /* AVR系列Arduino 中断引脚与终端号码对应关系表
   * ---------------------------------------------------------------------------------------
   * |                                        |  DigitalPin  | 2  | 3  |                   |
   * |    Uno, Nano, Mini, other 328-based    |--------------------------------------------|
   * |                                        | Interrupt No | 0  | 1  |                   |
   * |-------------------------------------------------------------------------------------|
   * |                                        |    Pin       | 2  | 3  | 21 | 20 | 19 | 18 |
   * |               Mega2560                 |--------------------------------------------|
   * |                                        | Interrupt No | 0  | 1  | 2  | 3  | 4  | 5  |
   * |-------------------------------------------------------------------------------------|
   * |                                        |    Pin       | 3  | 2  | 0  | 1  | 7  |    |
   * |    Leonardo, other 32u4-based          |--------------------------------------------|
   * |                                        | Interrupt No | 0  | 1  | 2  | 3  | 4  |    |
   * |--------------------------------------------------------------------------------------
   */
  /* microbit 中断引脚与终端号码对应关系表
   * ---------------------------------------------------------------------------------------------------------------
   * |                                                   |  DigitalPin  |    P0~P20均可作为外部中断使用            |
   * |                  microbit                         |---------------------------------------------------------|
   * |(作为外部中断时，无需用pinMode将其设置为输入模式)  | Interrupt No | 中断号即引脚数字值，如P0中断号为0，P1为1 |
   * |-------------------------------------------------------------------------------------------------------------|
   */
  attachInterrupt(/*中断号*/0,notifyA,RISING);//开启外部中断0,INTA连接至主控的数字引脚上：UNO(2),Mega2560(2),Leonardo(3),microbit(P0)
  attachInterrupt(/*中断号*/1,notifyB,RISING);//开启外部中断1,INTB连接至主控的数字引脚上：UNO(3),Mega2560(3),Leonardo(2),microbit(P1)
  #endif
}
/*中断服务函数*/
void notifyA(){
  intFlagA = true;
}
void notifyB(){
  intFlagB = true;
}

void loop() {
  if(intFlagA){
    intFlagA = false;
    /*pollInterrupts函数用于轮询某组端口是否发生中断
    参数group 如下参数都是可用的（默认值：eGPIOALL）：
     eGPIOA   eGPIOB   eGPIOALL
     A组端口  B组端口  A+B组端口
    */
    mcp.pollInterrupts(/*group = */mcp.eGPIOA);
  }
  if(intFlagB){
    intFlagB = false;
    mcp.pollInterrupts(/*group = */mcp.eGPIOB);
  }
}