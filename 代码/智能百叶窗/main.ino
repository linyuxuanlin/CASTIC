#include <WidgetServo/WidgetServo.h> //调用舵机的库文件
#include <DHT11/dht11.h> //调用温湿度传感器的库文件
#define SMARTLIGHT_CMD_SWITCH    "channel/widget3_0/cmd/control"   //开关命令 
#define SMARTLIGHT_DATA_STATUS   "channel/widget3_0/data/light"    //开关状态
#define AUTO_STATUS   "channel/widget3_0/data/autostate"   //自动 
#define LEDPIN       D6 //定义测试灯泡控制引脚
#define UP           D1 //上升按钮
#define DOWN         D2 //下降按钮
#define AUTOBUTTON   D3 //自动按钮
#define water        D4 //雨水传感器
Servo myservo_A0;//定义舵机类
DHT11 senser_humiture_D0 = DHT11(D0);
int a=0;//控制开合的变量
int gate = 62;//湿度闸值
int middle = 87;//舵机中位值
boolean  buttonstate =0;//判断“自动”按钮状态的布尔变量
int updelay =1150 ;// 1/4上升时间 
int downdelay =1100 ; // 1/4上升时间 

void setup()
{
	pinMode(UP, INPUT_PULLUP);
	pinMode(DOWN, INPUT_PULLUP);
	pinMode(water,INPUT);
	pinMode(AUTOBUTTON, INPUT_PULLUP);
	myservo_A0.attach(D5);
	pinMode(LEDPIN, OUTPUT);
	IntoRobot.subscribe("channel/widget3_0/cmd/control", NULL, widget3_0Cb);
	IntoRobot.subscribe(SMARTLIGHT_CMD_SWITCH, NULL, smartLightSwitchCb);
	myservo_A0.write(middle);
	SerialUSB.begin(115200);
}
void loop()
{
	IntoRobot.publish("channel/widget3_0/data/humidity",senser_humiture_D0.ReadHumidity());
	if( a== 4)
	{
		IntoRobot.publish("channel/widget3_0/data/string","状态：关");
	}
	if( a== 0)
	{
		IntoRobot.publish("channel/widget3_0/data/string","状态：开");
	}
	if(digitalRead(AUTOBUTTON) == LOW)
	{
		buttonstate = !buttonstate;
		delay(200);
	}
	if (buttonstate==1 )
	{
		//自动模式
		digitalWrite(LEDPIN,HIGH);
		automode();
		delay(10);
		IntoRobot.publish(AUTO_STATUS,"自动模式");
	}
	if(buttonstate==0  )
	{
		//手动模式
        IntoRobot.publish(AUTO_STATUS,"手动模式");
		digitalWrite(LEDPIN,LOW);
		manual();
		delay(10);
	}
	SerialUSB.println(a);
}


void widget3_0Cb(uint8_t *payload, uint32_t len) //云端湿度显示
{
}
void ICACHE_FLASH_ATTR smartLightSwitchCb(uint8_t *payload, uint32_t len) //云端按钮
{
	uint8_t SwitchKey;
	//解析云平台下发的控制命令
	aJsonClass aJson;
	aJsonObject *root = aJson.parse((char *)payload);
	if(root == NULL)
	{
		aJson.deleteItem(root);
		return;
	}
	aJsonObject *_switch = aJson.getObjectItem(root, "switch1");
	if(_switch != NULL)
	{
		SwitchKey = atoi(_switch->valuestring);
		if(SwitchKey)
		{
			SerialUSB.println("on");
			IntoRobot.publish(SMARTLIGHT_DATA_STATUS,"1");
			down();
			down();
			down();
			down();
		}
		else
		{
			SerialUSB.println("off");
			IntoRobot.publish(SMARTLIGHT_DATA_STATUS,"0");
			up();
			up();
			up();
			up();
		}
	}
	aJson.deleteItem(root);
}
void ICACHE_FLASH_ATTR manual() //手动模式
{
	if(digitalRead(UP) == LOW)
	{
		up();
	}
	if(digitalRead(DOWN) == LOW)
	{
		down();
	}
}
void  ICACHE_FLASH_ATTR automode() //自动模式
{
	if (senser_humiture_D0.ReadHumidity()>gate && digitalRead(water==0))
	{
		down();
	}
	if (senser_humiture_D0.ReadHumidity()<(gate)&& digitalRead(water==1))
	{
		up();
	}
	else
	{
	}
	delay(10);
}
void  ICACHE_FLASH_ATTR up() //上升的函数
{
	if (a<4)
	{
		a++;
		myservo_A0.write(164);
		delay (updelay);
		myservo_A0.write(middle);
	}
}
void  ICACHE_FLASH_ATTR down() //下降的函数
{
	if (a>0)
	{
		a--;
		myservo_A0.write(0);
		delay (downdelay);
		myservo_A0.write(middle);
	}
}