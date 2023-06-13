#ifndef SD3178_H
#define SD3178_H

#include "Arduino.h"
#include <Wire.h>

class SD3178
{
private:
    /* data */
    uint8_t I2C_ADDR = 0x32;
	TwoWire *_Wire;
public:
    SD3178(/* args */);
    ~SD3178();
    void begin();
	void begin(TwoWire &_wire);

    // 核心功能
    void enableI2cWrite(bool mode);  //寄存器写入使能
    // 实时时钟
    // 读取
    void readRTC(uint8_t &Year, uint8_t &Month, uint8_t &Day, uint8_t &Week, uint8_t &Hour, uint8_t &Minute, uint8_t &Second);
    uint8_t getSecond();
    uint8_t getMinute();
    uint8_t getHour();
    uint8_t getWeek();
    uint8_t getDay();
    uint8_t getMonth();
    uint8_t getYear();

    // 写入
    bool writeRTC(uint8_t Year,uint8_t Month,uint8_t Day,uint8_t Week,uint8_t Hour,uint8_t Minute,uint8_t Second);

    // 其他功能
	void chargingEnabled(bool Enabled,uint8_t current);  //充电使能  0:10k  1:5k  2:2k
	
	
    int8_t temperature(); // 温度
    int16_t vbat();     // 电池电压
	
	
	int8_t lowTemperature(); //历史低温值
	int8_t hihgTemperature();//历史高温值
	void readLowTemperatureRTC(uint8_t &Year, uint8_t &Month, uint8_t &Day, uint8_t &Week, uint8_t &Hour, uint8_t &Minute); //历史低温时间
	void readHighTemperatureRTC(uint8_t &Year, uint8_t &Month, uint8_t &Day, uint8_t &Week, uint8_t &Hour, uint8_t &Minute);//历史高温时间
	

    //用户存储器 70字节ram 0-69
    bool writeRam(uint8_t addr,uint8_t *Data,uint8_t len);  
    void readRam(uint8_t addr,uint8_t *Data,uint8_t len);


    // 链路底层通讯
    void i2c_read(uint8_t addr, uint8_t len, uint8_t *Data);
    void i2c_write(uint8_t addr,uint8_t len,uint8_t *Data);
	void i2c_write(uint8_t addr, uint8_t Data);
    int DeBCD(int bcd);     // bcd码解码
    int EnBCD(int decimal); // bcd码编码
};

#endif