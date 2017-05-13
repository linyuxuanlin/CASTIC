#include <I2Cdev/I2Cdev.h>
#define PM_address "channel/widget2_0/data/dustval"
#define RECIEVE_address "channel/widget2_0/data/test"
#define BUTTONS_address    "channel/widget2_0/cmd/control"  
#define JUDGE_address    "channel/widget2_0/data/judge"  
int voutPin = A0;//灰尘传感器
int sensorled = D0;//灰尘传感器的LED检测
int relayPin = D1;//继电器
int test_led = D6;//测试的LED
int samplingTime = 280;//灰尘传感器取样时间
int deltaTime = 40;//灰尘传感器中断时间
int sleepTime = 9680;//让灰尘传感器休息一会儿
float voMeasured = 0;//采集数据
float calcVoltage = 0;//采集数据
int dustval ;//PM值
int x = 900 ;//闸值,超过:继电器吸合,反之
int a = 3 ;//继电器状态
void setup()
{
	IntoRobot.publish(RECIEVE_address,"hello");
	IntoRobot.subscribe(PM_address, NULL, PM_print_function);//描述PM值显示
    pinMode(relayPin, OUTPUT);	//继电器设为输出
	pinMode(test_led, OUTPUT);	//测试的LED设为输出
	pinMode(sensorled, OUTPUT);	//灰尘传感器的LED初始化
	digitalWrite(relayPin, LOW);	//继电器初始化,先设为断开
	digitalWrite(test_led, LOW);
	SerialUSB.begin(115200);
	IntoRobot.subscribe(BUTTONS_address, NULL, buttons_function);
	SerialUSB.println("hello");
}
void loop()
{
	PM_read_function();	//读取PM值
	//IntoRobot.publish(RECIEVE_address,"hello");
	IntoRobot.publish(PM_address,dustval);	//输出PM值 
	//relay_auto_function();	//继电器自动模式
	if(a==3){
	    relay_auto_function();
	    SerialUSB.println("auto");
	    IntoRobot.publish(RECIEVE_address,"自动模式");
	}
	if(a==2){
	    digitalWrite(relayPin, HIGH);
	    SerialUSB.println("relay on");
	    IntoRobot.publish(RECIEVE_address,"常开");
	}
	if(a==0){
	    digitalWrite(relayPin, LOW);
	    SerialUSB.println("relay off");
	    IntoRobot.publish(RECIEVE_address,"关闭");
	}
	if(dustval>900){
	    IntoRobot.publish(JUDGE_address,"PM浓度超标");
	}
	else{
	    IntoRobot.publish(JUDGE_address,"PM浓度正常");
	}
	delay(500);
}
void PM_read_function() //读取PM值
{
	digitalWrite(sensorled, LOW);
	delayMicroseconds(samplingTime);
	voMeasured = analogRead(voutPin);
	delayMicroseconds(deltaTime);
	digitalWrite(sensorled, HIGH);	//关LED
	delayMicroseconds(sleepTime);
	calcVoltage = (float)voMeasured * (5.0 / 1024.0);
	dustval = 1000 * 0.17 * calcVoltage - 0.1;
}
void relay_auto_function() //继电器自动模式
{
	if (dustval > x) //ON
	{
		digitalWrite(relayPin, HIGH);
	}
	else //OFF
	{
		digitalWrite(relayPin, LOW);
	}
	delay(1000);
}
void PM_print_function(uint8_t *payload, uint32_t len) //云端PM显示
{
}
void buttons_function(uint8_t *payload, uint32_t len)
{
	uint8_t SwitchKey;
	uint8_t SwitchKey2;
	aJsonClass aJson;
	aJsonObject *root = aJson.parse((char *)payload);
	if(root == NULL)
	{
		aJson.deleteItem(root);
		return;
	}
	aJsonObject *_switch = aJson.getObjectItem(root, "onbutton");
	if(_switch != NULL)
	{
		SwitchKey = atoi(_switch->valuestring);
	
		if(SwitchKey)
		{
			SerialUSB.println("onbutton");
			a=1;
		}
		else
		{
			SerialUSB.println("offbutton");
			a=0;
		}
	}
	aJsonObject *_switch2 = aJson.getObjectItem(root, "modebutton");
	if(_switch2 != NULL)
	{
		SwitchKey2 = atoi(_switch2->valuestring);
		if(a==1)
		{
			if(SwitchKey2)
			{
				SerialUSB.println("mode on");
				a=3;
			}
			else
			{
				SerialUSB.println("mode off");
				a=2;
			}
		}
	}
	else
	{
	}
	aJson.deleteItem(root);
}