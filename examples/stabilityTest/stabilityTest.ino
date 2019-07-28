/*!
 * @file stabilityTest.ino
 * @brief 综合测试demo，测试扩展板的输入、输出、中断等功能。
 * @n 将扩展板B组端口上的引脚eGPB1设置为输出模式，并接上LED灯,取名LEDB1
 * @n 将扩展板A组端口上的引脚eGPA7设置为输出模式，并接上LED灯，取名LEDA7
 * @n 将扩展板A组端口上的引脚eGPA0设置为输入模式，并接上Button，取名按钮A0
 * @n 将扩展板A组端口上的引脚eGPA1设置为下降沿中断，并接上按钮，取名按钮A1，并将INTA中断信号引脚连接到主控的外部中断0（这里以UNO为例）
 * @n 将扩展板B组端口上的引脚eGPB7设置为上升沿中断，并接上按钮，取名按钮B7，并将INTB中断信号引脚连接到主控的外部中断1（这里以UNO为例）
 * @n 当检测到按钮A0被按下时，LEDB1亮，松开时，LEDB1灭
 * @n 当检测到A组端口发生中断时，LEDA7亮
 * @n 当检测到B组端口发生中断时，LEDA7灭
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2019-07-18
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
DFRobot_MCP23017 mcp(Wire, 0x27);//构造函数，地址可通过拨码开关更改A2A1A0的高低电平，实现硬件更改地址，范围0x20~0x27
//DFRobot_MCP23017 mcp;//这样定义会使用默认参数， Wire  0x27(默认I2C地址)

/*中断服务函数，原型为 void func(int index),index：表示发生中断的引脚编号*/
void gpioA1(int index){
  /*pinDescription函数用来将某个引脚转换为字符串描述
  参数pin 如下参数都是可用的
  eGPA0  eGPA1  eGPA2  eGPA3  eGPA4  eGPA5  eGPA6  eGPA7
    0      1      2      3      4      5      6      7
  eGPB0  eGPB1  eGPB2  eGPB3  eGPB4  eGPB5  eGPB6  eGPB7
    8      9      10     11     12     13     14     15
  */
  String description = mcp.pinDescription(/*pin = */index);
  Serial.print(description);
  Serial.println(" Interruption occurs!");
  Serial.println("LEDA7 turn on!");
  mcp.digitalWrite(/*pin = */mcp.eGPB1, /*level = */HIGH);
}
void gpioB7(int index){
  String description = mcp.pinDescription(/*pin = */index);
  Serial.print(description);
  Serial.println(" Interruption occurs!");
  Serial.println("LEDA7 turn off!");
  mcp.digitalWrite(/*pin = */mcp.eGPB1, /*level = */LOW);
}

bool intFlagA = false;//INTA中断标志
bool intFlagB = false;//INTB中断标志

void setup() {
  Serial.begin(115200);
  
  /*在这里一致等到芯片初始化完成才能退出*/
  while(mcp.begin() != 0){
    Serial.println("Initialization of the chip failed, please confirm that the chip connection is correct!");
    delay(1000);
  }
  #ifdef ARDUINO_ARCH_MPYTHON 
  pinMode(P0, INPUT);//使用掌控外部中断,INTA连接到掌控P0引脚
  pinMode(P1, INPUT);//使用掌控外部中断,INTB连接到掌控P1引脚
  #else
  pinMode(2, INPUT);//使用UNO的外部中断0
  pinMode(3, INPUT);//使用UNO的外部中断1
  #endif
  
  /*pinMode函数用于这是模块引脚模式
  参数pin 如下参数都是可用的：
  eGPA0  eGPA1  eGPA2  eGPA3  eGPA4  eGPA5  eGPA6  eGPA7
   0       1      2      3      4      5      6      7
  eGPB0  eGPB1  eGPB2  eGPB3  eGPB4  eGPB5  eGPB6  eGPB7
   8       9      10     11     12     13     14     15
  参数mode 如下参数是可用的：可设置成输入(INPUT)、输出(OUTPUT)、上拉输入(INPUT_PULLUP)模式
  */
  mcp.pinMode(/*pin = */mcp.eGPA7, /*mode = */OUTPUT);
  mcp.pinMode(/*pin = */mcp.eGPB1, /*mode = */OUTPUT);
  mcp.pinMode(/*pin = */mcp.eGPA0, /*mode = */INPUT);

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
  mcp.pinModeInterrupt(/*p = */mcp.eGPA1, /*mode = */mcp.eFalling, /*cb = */gpioA1);//数字引脚1(eGPA1)，下降沿中断，当引脚1的状态从高电平到低电平变化时产生中断，INTA输出高电平
  mcp.pinModeInterrupt(/*p = */mcp.eGPB7, /*mode = */mcp.eRising, /*cb = */gpioB7);//数字引脚15(eGPB7)，上升沿中断，当引脚15的状态从低电平到高电平变化时产生中断，INTB输出高电平
  #ifdef ARDUINO_ARCH_MPYTHON  //Microbit这里需要做一些单独处理吗
  /* 掌控 中断引脚与终端号码对应关系表
   * -------------------------------------------------------------------------------------
   * |                    |  DigitalPin  |        P0~P20均可作为外部中断使用             |
   * |    掌控            |--------------------------------------------------------------|
   * |                    | Interrupt No |  可用digitalPinToInterrupt(Pn) 查询中断号     |
   * |-----------------------------------------------------------------------------------|
   */
  attachInterrupt(digitalPinToInterrupt(P0)/*查询P0引脚的中断号*/,notifyA,RISING);//开启掌控P0引脚的外部中断，上升沿触发，INTA连接P0
  attachInterrupt(digitalPinToInterrupt(P1)/*查询P1引脚的中断号*/,notifyB,RISING);//开启掌控P1引脚的外部中断，上升沿触发，INTB连接P1
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
  /*digitalRead函数用于读取某个数字引脚的高低电平,在使用该函数之前，必须先将引脚设置为输入模式
  参数pin 如下参数都是可用的：
  eGPA0  eGPA1  eGPA2  eGPA3  eGPA4  eGPA5  eGPA6  eGPA7
   0       1      2      3      4      5      6      7
  eGPB0  eGPB1  eGPB2  eGPB3  eGPB4  eGPB5  eGPB6  eGPB7
   8       9      10     11     12     13     14     15
  */
  uint8_t value = mcp.digitalRead(/*pin = */mcp.eGPA0);
  if(value){
      /*digitalWrite函数用于将引脚输出高电平(HIGH)或低电平(LOW),在使用该函数之前，必须先将引脚设置为输出模式
      可以直接指定扩展板的引脚，参数pin如下参数都是可用的：
      eGPA0  eGPA1  eGPA2  eGPA3  eGPA4  eGPA5  eGPA6  eGPA7
       0      1       2      3      4      5      6      7
      eGPB0  eGPB1  eGPB2  eGPB3  eGPB4  eGPB5  eGPB6  eGPB7
        8      9     10     11      12     13    14     15
      */
      mcp.digitalWrite(/*pin = */mcp.eGPB1, /*level = */HIGH);
      delay(200);
  }else{
      mcp.digitalWrite(/*pin = */mcp.eGPB1, /*level = */LOW);
  }
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