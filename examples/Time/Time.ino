#include "SD3178.h"
SD3178 rtc;
void setup()
{
  //Wire.setSCL(PB10);   //STM32 write io
  //Wire.setSDA(PB11);
  rtc.begin();
  SerialUSB.begin(115200);
  //rtc.setRTC(23,3,6,1,11,32,0);   //set time 
}

void loop()
{
  uint8_t Year, Month, Day, Week, Hour, Minute, Second;
  rtc.getRTC(Year, Month, Day, Week, Hour, Minute, Second);   //get time
  SerialUSB.print("20");
  SerialUSB.print(Year);   //年
  SerialUSB.print("-");
  SerialUSB.print(Month);  //月
  SerialUSB.print("-");
  SerialUSB.print(Day);   //日
  SerialUSB.print(" ");
  SerialUSB.print(Week);  //周
  SerialUSB.print(" ");
  SerialUSB.print(Hour);   //小时
  SerialUSB.print(":");
  SerialUSB.print(Minute);  //分钟
  SerialUSB.print(":");
  SerialUSB.print(Second);  //秒
  SerialUSB.println();
  delay(900);
}