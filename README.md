# DFRobot_MCP23017
这是一款16位数字IO口扩展板，通过IIC接口与主控通信，可以读取并设置引脚的电平值。<br>
支持8个IIC地址，一块主控上最多并联8个模块，一次最多扩展128个IO口。<br>

这里需要显示拍照图片，可以一张图片，可以多张图片（不要用SVG图）

![正反面svg效果图](https://github.com/Arya11111/DFRobot_MCP23017/blob/master/resources/images/SEN0245svg1.png)


## 产品链接（链接到英文商城）
    DFR0626：Gravity: MCP23017 IIC to 16 digital IO expansion module
   
## Table of Contents

* [Summary](#summary)
* [Installation](#installation)
* [Methods](#methods)
* [Compatibility](#compatibility)
* [History](#history)
* [Credits](#credits)

## Summary

1. 设置引脚模式：输入模式、输出模式、上拉输入模式；<br>
2. 读取并设置引脚电平值；<br>
3. 支持5种中断方式：高电平中断、低电平中断、上升沿中断、下降沿中断、双边沿跳变中断；<br>
4. 支持2路中断信号输出：当端口A的某个引脚发生中断时，INTA引脚输出高电平，当端口B的某个引脚发生中断，INTB引脚输出高电平；<br>
5. 轮询中断：可通过轮询中断函数，检测引脚是否发生中断，并执行相应的中断服务函数；<br>

## Installation

To use this library, first download the library file, paste it into the \Arduino\libraries directory, then open the examples folder and run the demo in the folder.

## Methods

```C++
/**
 * @brief 初始化函数
 * @return 返回0表示初始化成功，返回其他值表示初始化失败
 */
int begin(void);

/**
 * @brief 设置引脚模式，将其配置为输入、输出或上拉输入模式
 * @param pin 引脚编号，可填ePin_t包含的所有枚举值（eGPA0-eGPB7/ 0-15）
 * @param mode 模式，可设置输入(INPUT)、输出(OUTPUT)、上拉输入(INPUT_PULLUP)模式
 * @return 返回0表示设置成功，返回其他值表示设置失败
 */
int pinMode(ePin_t pin, uint8_t mode);

/**
 * @brief 写数字引脚，在写引脚之前，需要将引脚设置为输出模式
 * @param pin 引脚编号，可填ePin_t包含的所有枚举值（eGPA0-eGPB7/ 0-15）
 * @param level 高低电平 1(HIGH)或0(LOW)
 * @return 返回0表示设置成功，返回其他值表示写入失败
 */
int digitalWrite(ePin_t pin, uint8_t level);

/**
 * @brief 读数字引脚，在读引脚之前，需要将引脚设置为输入模式
 * @param pin 引脚编号，可填ePin_t包含的所有枚举值（eGPA0-eGPB7/ 0-15）
 * @return 返回高低电平
 */
int digitalRead(ePin_t pin);

/**
 * @brief 将某个引脚设置为中断模式
 * @param pin 引脚编号，可填ePin_t包含的所有枚举值（eGPA0-eGPB7/ 0-15）
 * @param mode 中断方式：可填eInterruptMode_t包含的所有枚举值
 * @param cb 中断服务函数，由用户外部定义函数传参，原型为void func(int)
 */
void pinModeInterrupt(ePin_t pin, eInterruptMode_t mode,  MCP23017_INT_CB cb);

/**
 * @brief 轮询某组端口是否发生中断
 * @param group 端口组，可填eGPIOGrout_t包含的所有枚举值GPIO A组（eGPIOA）、GPIO B组（eGPIOB）A+B组（eGPIOALL）
 * @n 填eGPIOA，则轮询A组端口是否发生中断
 * @n 填eGPIOB，则轮询B组端口是否发生中断
 * @n 填eGPIOALL，则轮询A组和B组端口是否发生中断
 * @n 不填，默认轮询A组和B组所有端口是否发生中断
 */
void pollInterrupts(eGPIOGrout_t group=eGPIOALL);

/**
 * @brief 将引脚转为字符串描述
 * @param pin 引脚编号，可填ePin_t包含的所有枚举值（eGPA0-eGPB7/ 0-15）
 * @return 返回引脚描述字符串
 * @n 如"GPIOA0" "GPIOA1" "GPIOA2" "GPIOA3" "GPIOA4" "GPIOA5" "GPIOA6" "GPIOA7"
 * @n   "GPIOB0" "GPIOB1" "GPIOB2" "GPIOB3" "GPIOB4" "GPIOB5" "GPIOB6" "GPIOB7"
 */
String pinDescription(ePin_t pin);

/**
 * @brief 将引脚转为字符串描述
 * @param pin 引脚编号，范围0~15
 * @return 返回引脚描述字符串,
 * @n 如"GPIOA0" "GPIOA1" "GPIOA2" "GPIOA3" "GPIOA4" "GPIOA5" "GPIOA6" "GPIOA7"
 * @n   "GPIOB0" "GPIOB1" "GPIOB2" "GPIOB3" "GPIOB4" "GPIOB5" "GPIOB6" "GPIOB7"
 */
String pinDescription(int pin);
```

## Compatibility

MCU                | Work Well    | Work Wrong   | Untested    | Remarks
------------------ | :----------: | :----------: | :---------: | -----
Arduino Uno        |      √       |              |             | 
Mega2560        |      √       |              |             | 
Leonardo        |      √       |              |             | 
ESP32         |      √       |              |             | 
micro:bit        |      √       |              |             | 

## History

- data 2019-7-18
- version V1.0

## Credits

Written by(xue.peng@dfrobot.com), 2019. (Welcome to our [website](https://www.dfrobot.com/))





