/*!
 * @file pollInterrupt.ino
 * @brief 将IO扩展板的某个引脚设置为中断引脚，轮询端口引脚是否发生中断。
 * @n 实验现象：轮询某组端口中断，如果发生中断，则串口打印哪个引脚发生中断
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
void func(int index){
  /*pinDescription函数用来将某个引脚转换为字符串描述
  参数pin 如下参数都是可用的
  eGPA0  eGPA1  eGPA2  eGPA3  eGPA4  eGPA5  eGPA6  eGPA7
    0      1      2      3      4      5      6      7
  eGPB0  eGPB1  eGPB2  eGPB3  eGPB4  eGPB5  eGPB6  eGPB7
    8      9      10     11     12     13     14     15
  */
  String description = mcp.pinDescription(/*pin = */index);
  Serial.print(description);Serial.println(" Interruption occurs!");
}

void setup() {
  Serial.begin(115200);
  
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
  mcp.pinModeInterrupt(/*p = */mcp.eGPA0, /*mode = */mcp.eChangeLevel, /*cb = */func);//数字引脚0(eGPA0)，双边沿跳变中断，当引脚0的状态改变时产生中断，INTA输出高电平
  mcp.pinModeInterrupt(/*p = */mcp.eGPA1, /*mode = */mcp.eFalling, /*cb = */func);//数字引脚1(eGPA1)，下降沿中断，当引脚1的状态从高电平到低电平变化时产生中断，INTA输出高电平
  mcp.pinModeInterrupt(/*p = */mcp.eGPB7, /*mode = */mcp.eRising, /*cb = */func);//数字引脚15(eGPB7)，上升沿中断，当引脚15的状态从低电平到高电平变化时产生中断，INTA输出高电平
  mcp.pinModeInterrupt(/*p = */mcp.eGPB6, /*mode = */mcp.eHighLevel, /*cb = */func);//数字引脚14(eGPB6)，高电平中断，当引脚14的状态为高电平时产生中断，INTA输出高电平 
}

void loop() {
  /*pollInterrupts函数用于轮询某组端口是否发生中断
  参数group 如下参数都是可用的（默认值：eGPIOALL）：
   eGPIOA   eGPIOB   eGPIOALL
   A组端口  B组端口  A+B组端口
  */
  mcp.pollInterrupts();
 // delay(1000);
}