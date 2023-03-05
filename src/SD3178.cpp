#include "SD3178.h"

SD3178::SD3178(/* args */)
{
}

SD3178::~SD3178()
{
}

void SD3178::begin()
{
    Wire.begin();
}
//实时时钟
//读取
void SD3178::getRTC(uint8_t &Year,uint8_t &Month,uint8_t &Day,uint8_t &Week,uint8_t &Hour,uint8_t &Minute,uint8_t &Second)
{
    uint8_t addr = 0x00;
    uint8_t temp[7];
    i2c_read(addr, 7, temp); //获取数据
    Second = DeBCD(temp[0]);
    Minute = DeBCD(temp[1]);

    //小时处理

    Hour = DeBCD(temp[2]);
    Week = DeBCD(temp[3]);
    Day = DeBCD(temp[4]);
    Month = DeBCD(temp[5]);
    Year = DeBCD(temp[6]);


}
/*
uint8_t getSecond();
uint8_t getMinute();
uint8_t getHour();
uint8_t getWeek();
uint8_t getDay();
uint8_t getMonth();
uint8_t getYear();
*/
//写入
void setRTC(uint8_t &Year,uint8_t &Month,uint8_t &Day,uint8_t &Week,uint8_t &Hour,uint8_t &Minute,uint8_t &Second)
{

}



//I2C可写锁
void SD3178::enableI2cWrite(bool mode)
{
    //WRTC1 : 0X10 7  
    //WRTC2 : 0X0f 2
    //WRTC3 : 0X0f 7

    if(mode == true)
    {
        //写使能(先1再23)
        //先读取寄存器原值
        uint8_t temp[2];
        i2c_read(0X0f, sizeof(temp), temp);  //读取2个寄存器的原值 0x0f 0x10

        //修改寄存器值,再写入
        bitWrite(temp[1],7,1);  //修改WRTC1
        i2c_write(0x10,1,&temp[1]);  //写入数据

        //再修改 WRTC2 WRTC3
        bitWrite(temp[0],2,1);
        bitWrite(temp[0],7,1);
        i2c_write(0x0f,1,&temp[0]);  //写入数据
    }
    else
    {
        //写禁止(先23再1)

        //读取寄存器原值
        uint8_t temp[2];
        i2c_read(0X0f, sizeof(temp), temp);  //读取2个寄存器的原值 0x0f 0x10

        //设置 WRTC2 WRTC3
        bitWrite(temp[0],2,0);
        bitWrite(temp[0],7,0);
        i2c_write(0x0f,1,&temp[0]);  //写入数据

        //再设置 WRTC1
        bitWrite(temp[1],7,0);  //修改WRTC1
        i2c_write(0x10,1,&temp[1]);  //写入数据

    }
}



//



// 温度获取
int8_t SD3178::temperature()
{
    uint8_t addr = 0x16;
    uint8_t temp[1];
    i2c_read(addr, 1, temp);
    return (int8_t)temp[0];
}

// 电池电压
int16_t SD3178::batVal()
{
    uint8_t addr = 0x1A;
    uint8_t temp[2];
    i2c_read(addr, 2, temp);
    int16_t bat_val = (bitRead(temp[0], 7) << 8) + temp[1];
    return bat_val *10;  //mV output
}

// 底层

//i2c读取
void SD3178::i2c_read(uint8_t addr, uint8_t len, uint8_t *Data)
{
    // 超时逻辑
    uint32_t startTime = millis();
    uint32_t timeOut = 200;
    // 先写入逻辑地址
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(addr);
    Wire.endTransmission(false); // 重新起始信号
    // 获取返回数据
    Wire.requestFrom(I2C_ADDR, len);
    uint8_t datalen = 0;     // 获取计数器
    while (Wire.available()) // slave may send less than requested
    {
        *(Data + datalen) = Wire.read();
        datalen++;
        if (millis() - startTime > timeOut) // 超时管理
        {
            Wire.endTransmission(); // 结束总线
            return;
        }
    }
    Wire.endTransmission(); // 结束总线
}
//I2C写入
void SD3178::i2c_write(uint8_t addr,uint8_t len,uint8_t *Data)
{

    Wire.beginTransmission(I2C_ADDR);   //设备地址
    Wire.write(addr);   //寄存器地址
    for(uint8_t number =0;number<len;number++)  //写入
    {
        Wire.write(*(Data + number));
    }
    Wire.endTransmission(); // 结束
}


//解码
int SD3178::DeBCD( int bcd)  
{
  return (bcd + (bcd / 10) * 6);
}

//编码
int SD3178::EnBCD(int decimal)
{
  return (decimal - (decimal >> 4) * 6);
}