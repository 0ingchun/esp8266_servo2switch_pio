#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
//系统外设
#include <DHT.h>//包含DHT头文件
#define DHTPIN 13 //D7
#define LED1   16 //D0
#define LED2   14 //D5
#define jidianqi 1  //D10

#define DHTTYPE DHT11   // 使用温度湿度模块的类型为DHT11
DHT dht(DHTPIN, DHTTYPE);    //定义dht

#define wifi_ssid "*****"   //wifi名称
#define wifi_password "******" //wifi密码
#define mqtt_server "changyuan.store"  // EMQX服务器的域名
#define publish_topic "/client2/pub"   //发布消息主题
#define subscribe_topic "/client2/sub"  //订阅消息的主题

#define BODY_FORMAT     "{\"clientId\":\"%s\",\"params\":{\"humi\":\"%d\",\"temp\":\"%d\"}}"
WiFiClient espClient;
PubSubClient client(espClient);
String  clientId="client2";
int  tempData;
int  humiData;
String state;

void setup() { 
  Serial.begin(115200);
  dht.begin();//初始化DHT传感器
  pinMode(jidianqi, OUTPUT);//定义io口为输出
   digitalWrite(jidianqi,HIGH);
   pinMode(LED2, OUTPUT);//定义io口为输出
   digitalWrite(LED2,HIGH);
   pinMode(LED1, OUTPUT);//定义io口为输出
   digitalWrite(LED1,HIGH);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); //监听服务器下发的消息
}
void loop() { 
   if (!client.connected()) {
        reconnect();
    }
    else{ 
      DHTWORK();
      Serial.print("温度:");
      Serial.print(tempData);
      Serial.print("，湿度:");
      Serial.print(humiData);
      char jsonBuf[128];//存放payload主题内容
      sprintf(jsonBuf,BODY_FORMAT,clientId.c_str(),humiData,tempData,lux,state.c_str());
      client.publish(publish_topic, jsonBuf);
      client.subscribe(subscribe_topic);
  }
  client.loop();
  delay(1000);
}
void setup_wifi() {
    delay(10);
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("."); 
    }
}
void DHTWORK(){
   float h = dht.readHumidity();//读取DHT11传感器的湿度 并赋值给h
    float t = dht.readTemperature();//读取传感器的温度   并赋值给t
    if (isnan(h) || isnan(t))//判断是否成功读取到温湿度数据
    {
        Serial.println("error");
    }else{
      tempData=(int)t;
      humiData=(int)h;
     }
}
void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("nodeMcuRFID")) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}
void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    String message;
    for (int i = 0; i < length; i++) {
        message = message + (char) payload[i];  // convert *byte to string
    }
    Serial.print(message);
    //解析json
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(message);
    String LED1State = root["LED1"];
    String LED2State = root["LED2"];
    String LED3State = root["LED3"];

    if(LED1State=="true")
    {
      //读取
      int button1Sate = digitalRead(LED1);
      if(button1Sate==1)//高电平置为低
      {
       digitalWrite(LED1,HIGH);
       delay(200);
       digitalWrite(LED1, LOW);
       }else{//低电平置为高
       digitalWrite(LED1,LOW);
       delay(200);
       digitalWrite(LED1,HIGH);
    }    
    }
    if(LED2State=="true")
    {
      //读取
      int button2Sate = digitalRead(LED2);
      if(button2Sate==1)//高电平置为低
      {
       digitalWrite(LED2,HIGH);
       delay(200);
       digitalWrite(LED2, LOW);
       }else{//低电平置为高
       digitalWrite(LED2,LOW);
       delay(200);
       digitalWrite(LED2,HIGH);
       }    
    }
    if(LED3State=="true")
    {
      //读取
      int button3Sate = digitalRead(jidianqi);
      if(button3Sate==1)//高电平置为低
      {
       digitalWrite(jidianqi,HIGH);
       delay(200);
       digitalWrite(jidianqi,LOW);
       }else{//低电平置为高
       digitalWrite(jidianqi,LOW);
       delay(200);
       digitalWrite(jidianqi,HIGH);
       }    
    }
    Serial.println(); 
}

