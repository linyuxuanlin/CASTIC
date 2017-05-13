#define CINEMA_DATA_ADDRESS   "channel/widget9_0/data/cinema_data" 
#define CINEMA_SWITCH_ADDRESS   "channel/widget9_0/cmd/control" 
#define LIGHT_DATA_ADDRESS   "channel/widget9_0/data/light_data" 
#define BAIYECHUANG_DATA_ADDRESS   "channel/widget9_0/data/baiyechaung_data" 
#define CHAZUO_DATA_ADDRESS   "channel/widget9_0/data/chazuo_data" 
void setup() 
{
	IntoRobot.subscribe(CINEMA_SWITCH_ADDRESS, NULL, cinema_switch_function);

	SerialUSB.begin(115200);
	SerialUSB.println("hello");
}

void loop() 
{
// put your main code here, to run repeatedly.

}
void cinema_switch_function(uint8_t *payload, uint32_t len) //云端按钮
{
	uint8_t cinema_switch;
	//解析云平台下发的控制命令
	aJsonClass aJson;
	aJsonObject *root = aJson.parse((char *)payload);
	if(root == NULL)
	{
		aJson.deleteItem(root);
		return;
	}
	aJsonObject *_switch = aJson.getObjectItem(root, "cinema_switch");
	if(_switch != NULL)
	{
		cinema_switch = atoi(_switch->valuestring);
		if(cinema_switch)
		{
			SerialUSB.println("cinema on");
			IntoRobot.publish(CINEMA_SWITCH_ADDRESS,"影院模式开");
			IntoRobot.publish(LIGHT_DATA_ADDRESS,"灯泡已关");
			IntoRobot.publish(BAIYECHUANG_DATA_ADDRESS,"百叶窗已关");
			IntoRobot.publish(CHAZUO_DATA_ADDRESS,"空气净化器已开");
		
		}
		else
		{
			SerialUSB.println("cinema off");
		    IntoRobot.publish(CINEMA_SWITCH_ADDRESS,"影院模式关");
			IntoRobot.publish(LIGHT_DATA_ADDRESS,"灯泡开");
			IntoRobot.publish(BAIYECHUANG_DATA_ADDRESS,"百叶窗自动");
			IntoRobot.publish(CHAZUO_DATA_ADDRESS,"空气净化器自动");
			
		}
	}
	aJson.deleteItem(root);
}