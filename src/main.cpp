#include <Arduino.h>
#include <ESP8266WiFi.h>


#include<cstring>
#include<string>

//#include "HTTPClient.h"

#include "main.hpp"
#include "ritos.h"

#include "ESP8266WebServer.h"
#include "Servo.h"
#include "PubSubClient.h"
#include "ArduinoJson.h"
#include "LiquidCrystal_I2C.h"
#include "DHTesp.h"



#define I2C_SDA D2
#define I2C_SCL D1
#define DHTPIN D0
#define DHTTYPE DHT11

LiquidCrystal_I2C lcd(0x27, 20, 4);

DHTesp dht;

const char* ssid = "White-House";
const char* password = "qazwsxedcrfv123!";
const char* mqtt_server = "192.168.2.200";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(500) //50
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  WiFi.setAutoReconnect(true); //设置当断开连接的时候自动重连
  //WiFi.persistent(true); //该方法设置将WiFi参数保存于Flash
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println();
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // String message;
  // for (int i = 0; i < length; i++) {
  //   message = message + (char) payload[i];  // convert *byte to string
  // }
  // Serial.print(message);

  //// Switch on the LED if an 1 was received as first character
  // if ((char)payload[0] == '1') {
  //   digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
  //   // but actually the LED is on; this is because
  //   // it is active low on the ESP-01)
  // } else {
  //   digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  // }

////////////////

    Serial.println("$Received message:");

    String sTopic = topic; //将topic转换为String 可以加以处理

    String sMessage = (char *)payload;        //将消息转换为String
    sMessage = sMessage.substring(0, length); //取合法长度 避免提取到旧消息
    Serial.println(sMessage); //输出消息 用于调试 可以注释掉

    //解析json
    DynamicJsonDocument jsonBuffer(512); //创建一个DynamicJsonDocument类型的doc对象,大小2048byte
    //DynamicJsonBuffer jsonBuffer;
    //JsonObject& root = jsonBuffer.JsonObject(sMessage);
    Serial.println("从sMessage解码成的DynamicJsonDocument对象doc:");
    deserializeJson(jsonBuffer, sMessage);
    for (int i = 0; i < length; i++) {
      payload[i] = NULL;
    }
    sMessage = "";
    // serializeJson(jsonBuffer, Serial);
    Serial.println("从jsonBuffer对象转换成的JsonObject类型对象root:");
    JsonObject root = jsonBuffer.as<JsonObject>();
    // serializeJson(root, Serial);
    Serial.println();
    Serial.println(jsonBuffer.memoryUsage());
    Serial.println(measureJson(jsonBuffer));
    Serial.println(measureJsonPretty(jsonBuffer));

    String LED1State = jsonBuffer["LED1"];
    String LED2State = jsonBuffer["LED2"];
    String LED3State = jsonBuffer["LED3"];

    Serial.println(LED1State);
    Serial.println(LED2State);
    Serial.println(LED3State);

    //按字符判断mqtt消息中命令的作用 可以自行定义
    if (sMessage.charAt(0) == '#')
    {         //第一位#
        if (sMessage.charAt(1) == 'D') 
        {     //第一位l
            if (sMessage.charAt(2) == '1') 
            { //第一位1
                //处理#D1
                //digitalWrite(LED, 0);  //输出低电平
                digitalWrite(LED_BUILTIN, LOW);  
            } 
            else if (sMessage.charAt(2) == '0') 
            {
                //处理#D0
                //igitalWrite(LED, 1); //初始化输出高电平
                digitalWrite(LED_BUILTIN, HIGH);  
            }
        }
        if (sMessage.charAt(1) == 'd') 
        {
            //处理#d...
        }
    }


}


String clientId = "ESP8266Client-lcdStation" + String(random(0xffff), HEX), clientUser = "user8266", clientPasswd = "88888888";

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    //String clientId = "ESP8266Client-lcdStation", clientUser = "user8266", clientPasswd = "88888888";
    //String(random(0xffff), HEX)

    // Attempt to connect
    if (client.connect(clientId.c_str(), clientUser.c_str(), clientPasswd.c_str())) {

        ESP.wdtFeed();

      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(100);
    }
  }
}



volatile int a = 1000, b = 1000;
int statemachine = 0;

Ritos tarefa1; // Thread Task One / 綫程任務1
void thread1() {
  a--;
  
  if ( a <= 0 && statemachine==0) {
    Serial.println("Interrupt thread 1!");
    lcd.backlight();
      lcd.setCursor(0,0);
  lcd.print(b);
    // Attention! Delay() cannot be used within the thread / 注意！綫程内不允許使用Delay()
    a = 1500; // 1500ms. You can be used in this way instead of Delay(1500); 您可以通過此方法延時，代替Delay(1500);
    statemachine=1;
  }

    if ( a <= 0 && statemachine==1) {   
    Serial.println("thread 1 again!");
    lcd.backlight();
      lcd.setCursor(0,0);
  lcd.print(b);
    a = 2000; // 2000ms
    statemachine=2;
  }

    if ( a <= 0 && statemachine==2) {   
    Serial.println("thread 1 again again!");
    lcd.backlight();
      lcd.setCursor(0,0);
  lcd.print(b);
    a = 1700; // 1700ms
    statemachine=0;
  }

}


Ritos tarefa3; // Thread Task Two / 綫程任務3

void thread3() {

  //client.subscribe("inTopic");
  client.loop();

}

Ritos tarefa4; // Thread Task Two / 綫程任務4
void thread4() {

  //unsigned long now4 = millis();
  if (millis() - lastMsg > 500) { // >2000 => 2000ms
    
    //++value;
    
    String dht_msg;
    dht_msg = String("*") 
    + String(dht.getHumidity()) 
    + String("#") 
    + String(dht.getTemperature()) 
    + String("#") 
    + String(dht.getStatusString()) 
    + String("*")
    + String("hello world #%ld")
    + String("*");

    //String pp = "dagah";

    char dht_p[sizeof(dht_msg)];
    int i;
    for( i=0;i<dht_msg.length();i++)
      dht_p[i] = dht_msg[i];
    dht_p[i] = '\0';
    //printf("%s\n",p);

    //const char cnm[sizeof(dht_msg)] = {dht_msg};

    snprintf (msg, MSG_BUFFER_SIZE, dht_p);

    // float dht_h = dht.getHumidity();
    // snprintf (msg, MSG_BUFFER_SIZE, "h: %f", dht.getHumidity());

    //snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    // Serial.print("Publish message: ");
    // Serial.println(msg);
    client.publish("outTopic", msg);

    lastMsg = millis();
  }
  
}

Ritos tarefa5; // Thread Task Two / 綫程任務5
volatile int ms_5 = 1500;
void thread5() {

  ms_5--;

  if ( ms_5 <= 0 ) {

    snprintf (msg, MSG_BUFFER_SIZE, "h: %f", dht.getHumidity());
    client.publish("outTopic", msg);
    lastMsg = millis();

    ms_5 = 1500;
  }

}

Ritos tarefa6; // Thread Task Two / 綫程任務6
volatile int ms_6 = 3000;
void thread6() {

  ms_6--;

  if ( ms_6 <= 0 ) {

    //ets_delay_us(10000);

    snprintf (msg, MSG_BUFFER_SIZE, "ttttttttttttttt: %f", dht.getTemperature());
    client.publish("outTopic", msg);
    lastMsg = millis();

    ms_6 = 3000;
  }

}


Ritos tarefa8; // Thread Task Two / 綫程任務8
volatile int ms_8 = 10000;
unsigned long previousMillis = 0;
unsigned long interval = 30000;
void thread8() {
  ms_8--;

//client.loop();

  if ( ms_8 <= 0 ) {

      
  //定时重连
  if(WiFi.status()!= WL_CONNECTED){
    Serial.println("Failed to connect WiFi...");
    digitalWrite(LED_BUILTIN, HIGH);
  }

  if(!client.connected()){
    Serial.print("Failed to connect mqtt Server... ");
    Serial.println(client.state());
    digitalWrite(LED_BUILTIN, HIGH);
  }

  //ESP.wdtFeed();

  unsigned long currentMillis = millis();
  if (!client.connected() && (currentMillis - previousMillis >=interval) && WiFi.status()== WL_CONNECTED) {
    Serial.print(millis());
    Serial.println("Reconnecting to mqtt Server...");

    // while (WiFi.status() != WL_CONNECTED) {
    //   delay(500);
    //   Serial.print(".");
    // }

    // randomSeed(micros());
    // if (!client.connected()) {
    //   reconnect();
    // }

    // String clientId = "ESP8266Client-lcdStation", clientUser = "user8266", clientPasswd = "88888888";
    // clientId += String(random(0xffff), HEX);

      //ESP.wdtFeed();
      ESP.wdtDisable();

      client.disconnect();
    Serial.println(client.state());

    //client.connected();
    while(!client.connected()){
      ESP.wdtFeed();
      client.connect("ClientID2", "2user8266", "22222222");
      ESP.wdtFeed();
      Serial.println(client.state());
    }
    //reconnect();
    
      //ESP.wdtEnable(3000);
      system_soft_wdt_restart();
      ESP.wdtEnable((uint32_t) 300);
      //NOP;
      ESP.wdtFeed();

  }

  if(WiFi.status() == WL_CONNECTED){
    Serial.println("");
    Serial.println("WiFi connecteddddddddddddddddd");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    digitalWrite(LED_BUILTIN, LOW);

    if(client.connected()){
      Serial.println("Attempting MQTT connection...");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
      digitalWrite(LED_BUILTIN, LOW);

      previousMillis = currentMillis;
    } 
    
    else {
      Serial.print("Failed to connect mqtt Server...  ");
      Serial.println(client.state());
    }

  } 

    ms_8 = 10000;
  }

}

Ritos tarefa9; // Thread Task Two / 綫程任務9
volatile int ms_9 = 3000;
void thread9() {

  ms_9--;

  if ( ms_9 <= 0 ) {

    //ets_delay_us(1000);

    // 200 是大小 如果这个Json对象更加复杂，那么就需要根据需要去增加这个值.
  //StaticJsonDocument<200> doc;
 
  // StaticJsonDocument 在栈区分配内存   它也可以被 DynamicJsonDocument（内存在堆区分配） 代替
  DynamicJsonDocument  doc(200);
 
  //添加键值对
  doc["sensor"] = "gps";
  doc["time"] = 1351824120;
 
  // 添加数组.
  JsonArray data = doc.createNestedArray("data");
  data.add(48.756080);
  data.add(2.302038);
 
//串口打印结果
  serializeJson(doc, Serial);
  Serial.println();

//   /**
//  * 构造序列化json，格式化输出
//  * @param doc jsondocument对象
//  * @param output 输出内容
//  */
//   size_t serializeJsonPretty(const JsonDocument& doc, char* output, size_t outputSize);
//   size_t serializeJsonPretty(const JsonDocument& doc, char output[size]);
//   size_t serializeJsonPretty(const JsonDocument& doc, Print& output);
//   size_t serializeJsonPretty(const JsonDocument& doc, String& output);
//   size_t serializeJsonPretty(const JsonDocument& doc, std::string& output);
  
  // string str1 = "ABCDEFG";
  // char a[20];
  // strcpy(a,str1.c_str());//用到 c_str()函数


  String output;
  serializeJson(doc, output);
  Serial.println(output);

  String smsg = output;
	char cmsg[smsg.length()]; 
  strcpy(cmsg,smsg.c_str());

    snprintf (msg, MSG_BUFFER_SIZE, cmsg);
    client.publish("outTopic", msg);
    lastMsg = millis();

    ms_9 = 3000;
  }
  
}

void setup() {
  Serial.begin(115200);

  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  setup_wifi();
  //WiFi.setAutoReconnect(true);

  client.setServer(mqtt_server, 1883);
  client.setKeepAlive(65535); //不知道啥东西，保活时间？
  client.setCallback(callback);
  //client.setStream(sram);

  lcd.init(); //LCD init

  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hello, world!");
  lcd.setCursor(2,1);
  lcd.print("Ywrobot Arduino!");
  lcd.setCursor(0,2);
  lcd.print("Arduino LCM IIC 2004");
  lcd.setCursor(2,3);
  lcd.print("Power By Ec-yuan!");

  dht.setup(DHTPIN, DHTesp::DHTTYPE);
  delay(dht.getMinimumSamplingPeriod());

  if (!client.connected()) {
    reconnect();
  }

  lcd.noBacklight();
  lcd.clear();

  // Create threads task / 創建线程任務
  tarefa1.task(thread1);  
  //tarefa2.task(thread2);
  tarefa3.task(thread3);
  tarefa4.task(thread4);
  tarefa5.task(thread5);
  tarefa6.task(thread6);
  //tarefa7.task(thread7);
  tarefa8.task(thread8);
  tarefa9.task(thread9);

}

void loop() {
  // Nothing will be run here / 此處不會被運行任何東西
}
