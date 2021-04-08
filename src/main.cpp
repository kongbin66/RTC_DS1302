#include <Arduino.h>
// CONNECTIONS:
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO --> 4
// DS1302 RST/CE --> 2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

#include <ThreeWire.h>  
#include <RtcDS1302.h>

#define key 14

ThreeWire myWire(18,19,5); // IO, SCLK, CE//三线设置引脚
RtcDS1302<ThreeWire> Rtc(myWire);//创建ds1302对象


/////////////////////////////////////////////////////////////
/*函数作用是在RtcDateTime实例中提取出时间信息放在datestring[20]中
输入：RtcDateTime对象
输出：datestring[20]存放时间信息
*/
#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)//获取时间
{
    char datestring[20];//存放时间
    snprintf_P(datestring, //打印到数组中去
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),//格式
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);//打印数组到串口

}
///////////////////////////////////////////////////////////////

void setup () 
{
   
    Serial.begin(115200);//串口初始化

    Serial.print("电脑编译时的时间：");//这个时间是编译时自动提取的电脑系统中的时间。
    Serial.print(__DATE__);
    Serial.println(__TIME__);
  
    Rtc.Begin();//KB:初始化ds1302对象设置管脚状态。

    //RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);//加载编译时间
    RtcDateTime compiled= RtcDateTime(2022,1,2, 12, 00, 00); //自由设置时间
    printDateTime(compiled);//打印时间对象的值
    Serial.println();
 
    
   //下面是判断三种异常
    
    if (!Rtc.IsDateTimeValid()) 
    {
        //获取时间失败：1.可能原因第一次运行时，设备尚未运行2.设备上的电池电量不足甚至丢失
        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        //获取到设置为写保护了
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        //监测到DS1302未运行
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }
//对时间的判断更新时间
    RtcDateTime now = Rtc.GetDateTime();//建立一个现在时间
    if (now < compiled) //编译时间早于现在时间（有偏差）
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);//更新时间
    }
    else if (now > compiled) //编译时间晚于于现在时间（有偏差）
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
        Rtc.SetDateTime(compiled);//更新时间
    }
    else if (now == compiled) //编译时间和现在时间相等（这种情况极少）
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
}

void loop () 
{
    //功能主要用法：
    //1.获取时间使用时间
    RtcDateTime now = Rtc.GetDateTime();//建立一个时间对象（now）
    printDateTime(now);//在此函数中使用时间
    //2.更新时间
    // RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);//加载编译时间或
    // RtcDateTime compiled= RtcDateTime(2022,1,2, 12, 00, 00); //自由设置时间
    // Rtc.SetDateTime(compiled);
    delay(10000);
    

}


    