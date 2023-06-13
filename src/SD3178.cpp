#include "SD3178.h"

SD3178::SD3178(/* args */)
{
}

SD3178::~SD3178()
{
}

void SD3178::begin()
{
	
	_Wire = &Wire; 
    _Wire->begin();
}

void SD3178::begin(TwoWire &_wire)
{
	_Wire = &_wire; 
    _Wire->begin();
}
// 实时时钟
// 读取
void SD3178::readRTC(uint8_t &Year, uint8_t &Month, uint8_t &Day, uint8_t &Week, uint8_t &Hour, uint8_t &Minute, uint8_t &Second)
{
    uint8_t addr = 0x00;
    uint8_t temp[7];
    i2c_read(addr, 7, temp); // 获取数据
    Second = DeBCD(temp[0]);
    Minute = DeBCD(temp[1]);
    // 小时处理 
    uint8_t _Hour = temp[2];   //读取适合,先进行修改,再解码传出
    bitWrite(_Hour,7,0);   //删除24小时标记位
    Hour = DeBCD(_Hour);
    
    Week = DeBCD(temp[3]);
    Day = DeBCD(temp[4]);
    Month = DeBCD(temp[5]);
    Year = DeBCD(temp[6]);
}


// 写入
bool SD3178::writeRTC(uint8_t Year, uint8_t Month, uint8_t Day, uint8_t Week, uint8_t Hour, uint8_t Minute, uint8_t Second)
{
    // 写使能
    enableI2cWrite(true);  // 写入使能
    uint8_t write_Data[7]; // 写入数据缓存
    write_Data[0] = EnBCD(Second);
    write_Data[1] = EnBCD(Minute);
    uint8_t _Hour = EnBCD(Hour); //输入时先进行编码,再修改寄存器位置
    bitWrite(_Hour,7,1);   //在小时的最高位上写1 表示24小时制
    write_Data[2] = _Hour;
    write_Data[3] = EnBCD(Week);
    write_Data[4] = EnBCD(Day);
    write_Data[5] = EnBCD(Month);
    write_Data[6] = EnBCD(Year);

    i2c_write(0x00, sizeof(write_Data), write_Data); // 写寄存器
    // 写禁止
    enableI2cWrite(false); // 写入禁止
    return true;
}


//写用户内存
bool SD3178::writeRam(uint8_t addr,uint8_t *Data,uint8_t len)
{
    if(len>70)  //防止数据越界
    {
        len =70;
    }
    if(addr>70)
    {
        addr = 70;
    }
    uint8_t start_addr = 0x2c;  //起始内存地址

    // 写使能
    enableI2cWrite(true);  // 写入使能
    i2c_write(start_addr + addr, len, Data); // 写寄存器

    // 写禁止
    enableI2cWrite(false); // 写入禁止
    
    return true;
}

//读用户内存
void SD3178::readRam(uint8_t addr,uint8_t *Data,uint8_t len)
{
    if(len>70)  //防止数据越界
    {
        len =70;
    }
    if(addr>70)
    {
        addr = 70;
    }
    uint8_t start_addr = 0x2c;  //起始内存地址
    i2c_read(start_addr + addr, len, Data);
}





// I2C可写锁
void SD3178::enableI2cWrite(bool mode)
{
    // WRTC1 : 0X10 7
    // WRTC2 : 0X0f 2
    // WRTC3 : 0X0f 7

    if (mode == true)
    {
        // 写使能(先1再23)
        // 先读取寄存器原值
        uint8_t temp[2];
        i2c_read(0X0f, sizeof(temp), temp); // 读取2个寄存器的原值 0x0f 0x10

        // 修改寄存器值,再写入
        bitWrite(temp[1], 7, 1);      // 修改WRTC1
        i2c_write(0x10, 1, &temp[1]); // 写入数据

        // 再修改 WRTC2 WRTC3
        bitWrite(temp[0], 2, 1);
        bitWrite(temp[0], 7, 1);
        i2c_write(0x0f, 1, &temp[0]); // 写入数据
    }
    else
    {
        // 写禁止(先23再1)

        // 读取寄存器原值
        uint8_t temp[2];
        i2c_read(0X0f, sizeof(temp), temp); // 读取2个寄存器的原值 0x0f 0x10

        // 设置 WRTC2 WRTC3
        bitWrite(temp[0], 2, 0);
        bitWrite(temp[0], 7, 0);
        i2c_write(0x0f, 1, &temp[0]); // 写入数据

        // 再设置 WRTC1
        bitWrite(temp[1], 7, 0);      // 修改WRTC1
        i2c_write(0x10, 1, &temp[1]); // 写入数据
    }
}

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
    return bat_val * 10; // mV output
}



//历史低温值
int8_t SD3178::lowTemperature()
{
    uint8_t addr = 0x1E;
    uint8_t temp[1];
    i2c_read(addr, 1, temp);
    return (int8_t)temp[0];
}


//历史高温值
int8_t SD3178::hihgTemperature()
{
    uint8_t addr = 0x1F;
    uint8_t temp[1];
    i2c_read(addr, 1, temp);
    return (int8_t)temp[0];
}

//读取历史低温时间
void SD3178::readLowTemperatureRTC(uint8_t &Year, uint8_t &Month, uint8_t &Day, uint8_t &Week, uint8_t &Hour, uint8_t &Minute)
{
    uint8_t addr = 0x20;
    uint8_t temp[6];
    i2c_read(addr, 6, temp); // 获取数据
    Minute = DeBCD(temp[0]);
    // 小时处理 
    uint8_t _Hour = temp[1];   //读取适合,先进行修改,再解码传出
    bitWrite(_Hour,7,0);   //删除24小时标记位
    Hour = DeBCD(_Hour);
    
    Week = DeBCD(temp[2]);
    Day = DeBCD(temp[3]);
    Month = DeBCD(temp[4]);
    Year = DeBCD(temp[5]);
}

//历史高温时间
void SD3178::readHighTemperatureRTC(uint8_t &Year, uint8_t &Month, uint8_t &Day, uint8_t &Week, uint8_t &Hour, uint8_t &Minute)
{
    uint8_t addr = 0x26;
    uint8_t temp[6];
    i2c_read(addr, 6, temp); // 获取数据
    Minute = DeBCD(temp[0]);
    // 小时处理 
    uint8_t _Hour = temp[1];   //读取适合,先进行修改,再解码传出
    bitWrite(_Hour,7,0);   //删除24小时标记位
    Hour = DeBCD(_Hour);
    
    Week = DeBCD(temp[2]);
    Day = DeBCD(temp[3]);
    Month = DeBCD(temp[4]);
    Year = DeBCD(temp[5]);
}













// ===============底层=======================

// i2c读取
void SD3178::i2c_read(uint8_t addr, uint8_t len, uint8_t *Data)
{
    // 超时逻辑
    uint32_t startTime = millis();
    uint32_t timeOut = 200;
    // 先写入逻辑地址
    _Wire->beginTransmission(I2C_ADDR);
    _Wire->write(addr);
    _Wire->endTransmission(false); // 重新起始信号
    // 获取返回数据
    _Wire->requestFrom(I2C_ADDR, len);
    uint8_t datalen = 0;     // 获取计数器
    while (_Wire->available()) // slave may send less than requested
    {
        *(Data + datalen) = _Wire->read();
        datalen++;
        if (millis() - startTime > timeOut) // 超时管理
        {
            _Wire->endTransmission(); // 结束总线
            return;
        }
    }
    _Wire->endTransmission(); // 结束总线
}
// I2C写入
void SD3178::i2c_write(uint8_t addr, uint8_t len, uint8_t *Data)
{

    _Wire->beginTransmission(I2C_ADDR);                // 设备地址
    _Wire->write(addr);                                // 寄存器地址
    for (uint8_t number = 0; number < len; number++) // 写入
    {
        _Wire->write(*(Data + number));
    }
    _Wire->endTransmission(); // 结束
}

// 编码
int SD3178::EnBCD(int bcd)
{
    return (bcd + (bcd / 10) * 6);
}
// 解码
int SD3178::DeBCD(int decimal)
{
    return (decimal - (decimal >> 4) * 6);
}