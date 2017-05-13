#define HUMIDITY_address "channel/widget7_0/data/humidity"
#define BUTTONS_address "channel/widget7_0/cmd/control"
#define AUTOSTATE_address "channel/widget7_0/data/autostate"
int humidity_pin=A0;//土壤湿度传感器
int relayPin = D0;//继电器
int autostate;//自动状态
int jiaoshui_state;//手动浇水状态
int x = 50 ;//湿度阈值
int humidity;//湿度值
void setup()
{
	pinMode(relayPin, OUTPUT);	//继电器设为输出
	pinMode(humidity_pin,INPUT);	//湿度传感器设为输入
	digitalWrite(relayPin, LOW);	//继电器初始化,先设为断开
	IntoRobot.subscribe(BUTTONS_address, NULL, buttons_function);	//说明按钮控制
	IntoRobot.subscribe(HUMIDITY_address, NULL, HUMIDITY_print_function);	//说明湿度显示
	SerialUSB.begin(115200);
	SerialUSB.println("hello");
}
void loop()
{
	//addwater_function();
	humidity_read_function();
	IntoRobot.publish(HUMIDITY_address,humidity);	//输出湿度
	if(autostate==1)//自动模式
	{
		IntoRobot.publish(AUTOSTATE_address,"自动");
		if(humidity<=x)
		{
			auto_water_function();
		}
		else
		{
			digitalWrite(relayPin, LOW);
		}
	}
	if(autostate==0)//手动模式
	{
		IntoRobot.publish(AUTOSTATE_address,"手动");
		if(jiaoshui_state==1)
		{
			manual_water_function();
		}
		else
		{
		}
	}
	delay(200);
}
void humidity_read_function()//读取传感器湿度
{
	//float humidity =analogRead(humidity_pin)* (3.3 / 1023.0*100); 
	humidity=map(analogRead(humidity_pin),1023,245,0,100);
}
void manual_water_function()//手动浇水
{
	digitalWrite(relayPin, HIGH);
	delay(1000);
	digitalWrite(relayPin, LOW);
}
void auto_water_function()//自动浇水
{
	digitalWrite(relayPin, HIGH);
	delay(700);
	digitalWrite(relayPin, LOW);
}
void HUMIDITY_print_function(uint8_t *payload, uint32_t len) //云端湿度显示
{
}
void buttons_function(uint8_t *payload, uint32_t len)//自动&浇水按钮
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
	aJsonObject *_switch = aJson.getObjectItem(root, "autobutton");
	if(_switch != NULL)
	{
		SwitchKey = atoi(_switch->valuestring);
		if(SwitchKey)
		{
			SerialUSB.println("auto on");
			autostate=1;
		}
		else
		{
			SerialUSB.println("auto off");
			autostate=0;
		}
	}
	aJsonObject *_switch2 = aJson.getObjectItem(root, "waterbutton");
	if(_switch2 != NULL)
	{
		SwitchKey2 = atoi(_switch2->valuestring);
		if(SwitchKey2)
		{
			SerialUSB.println("water on");
			jiaoshui_state=1;
		}
		else
		{
			SerialUSB.println("water off");
			jiaoshui_state=0;
		}
	}
	else
	{
	}
	aJson.deleteItem(root);
}
/*
void addwater_function()//自动补水
{
    读出水位();
	if(过低)
	{
		总闸打开();
		delay(200);
		关闭();
	}
	读水位();
}
*/