#include <ESP8266MQTTClient.h>
#include <ESP8266WiFi.h>
MQTTClient mqtt;
//用继电器GPIO0来控制，低电平激活蓝色灯
#define JDQ 0
 
void setup() {
  //设置波特率
  Serial.begin(115200);
  //设置继电器GPIO输出
  pinMode(JDQ, OUTPUT);
  //判断是否已经连上，这个是断电重连关键
  if (!autoConfig())
  {
    Serial.println("Start module");
    //智能配网
    smartConfig();
  }
 
  //mqtt相关内容，详情请看往期mqtt教程，这里需要加载一下zip文件，看我的其他博客关于mqtt的教程
  //topic, data, data is continuing
  //接收mqtt消息
  mqtt.onData([](String topic, String data, bool cont) {
    Serial.printf("Data received, topic: %s, data: %s\r\n", topic.c_str(), data.c_str());
    //简单的逻辑处理，1开0关
    if (data == "0") {
      digitalWrite(JDQ, HIGH);
    } else if (data == "1") {
      digitalWrite(JDQ, LOW);
    } 
  });
 
  //订阅之后发送一条命令，可有可无
  mqtt.onSubscribe([](int sub_id) {
    Serial.printf("Subscribe topic id: %d ok\r\n", sub_id);
    mqtt.publish("esp826601", "GONEPOO", 0, 0);
  });
 
  //mqtt连接
  mqtt.onConnect([]() {
    Serial.printf("\r\nMQTT: Connected\r\n");
    Serial.printf("Subscribe id: %d\r\n", mqtt.subscribe("esp826601", 0));
  });
 
  //连接mqtt服务器，连不上的，自己改一下
  mqtt.begin("mqtts://user:pass@mosquito.org:8883#clientId");
}
 
//自动断电重连关键部分
bool autoConfig()
{
  WiFi.begin();
  for (int i = 0; i < 20; i++)
  {
    int wstatus = WiFi.status();
    if (wstatus == WL_CONNECTED)
    {
      Serial.println("AutoConfig Success");
      Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      WiFi.printDiag(Serial);
      return true;
    }
    else
    {
      Serial.print("AutoConfig Waiting......");
      Serial.println(wstatus);
      delay(1000);
    }
  }
  Serial.println("AutoConfig Faild!" );
  return false;
}
 
/**
   一键配网关键代码
*/
void smartConfig()
{
  WiFi.mode(WIFI_STA);
  Serial.println("\r\nWait for Smartconfig");
  // 等待配网
  WiFi.beginSmartConfig();
 
  while (1)
  {
    Serial.print(".");
    delay(500);
    if (WiFi.smartConfigDone())
    {
      Serial.println("SmartConfig Success");
      Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      WiFi.setAutoConnect(true);  // 设置自动连接
      break;
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: " );
  delay(300);
  Serial.println(WiFi.localIP());
}
 
void loop() {
  mqtt.handle();
}