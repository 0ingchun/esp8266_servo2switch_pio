#include "main.hpp"


/*----------------------------------------------用户函数------------------------------------------------*/

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


void reconnect_mqtt() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    // String clientId = "ESP8266Client-lcdStation", clientUser = "user8266", clientPasswd = "88888888";
    // String(random(0xffff), HEX)

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


/*----------------------------------------------定义任务------------------------------------------------*/


Ritos tarefa_mqtt_loop; // Thread Task Two / 綫程任務3

void thread_mqtt_loop() {

  //client.subscribe("inTopic");
  client.loop();

}


Ritos tarefa_mqtt_publish; // Thread Task Two / 綫程任務9
volatile int ms_9 = 3000;
void thread_mqtt_publish() {

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


Ritos tarefa_auto_reconnect; // Thread Task Two / 綫程任務8
volatile int ms_8 = 10000;
unsigned long previousMillis = 0;
unsigned long interval = 30000;
void thread_auto_reconnect() {
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
    //   reconnect_mqtt();
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
    //reconnect_mqtt();
    
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


Ritos tarefa_servo; // Thread Task Two / 綫程任務5
volatile int ms_5 = 1500;
void thread_servo() {

  ms_5--;

  if ( ms_5 <= 0 ) {

    // snprintf (msg, MSG_BUFFER_SIZE, "h: %f", dht.getHumidity());
    // client.publish("outTopic", msg);
    // lastMsg = millis();

    ms_5 = 1500;
  }

}

/*---------------------------------------------Arduino主函数-------------------------------------------------*/

void setup() {
  Serial.begin(115200);

  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  setup_wifi();
  //WiFi.setAutoReconnect(true);

  client.setServer(mqtt_server, 1883);
  client.setKeepAlive(65535); //不知道啥东西，保活时间？
  client.setCallback(callback);

  if (!client.connected()) {
    reconnect_mqtt();
  }

  // Create threads task / 創建线程任務

  tarefa_mqtt_loop.task(thread_mqtt_loop);
  tarefa_mqtt_publish.task(thread_mqtt_publish);
  tarefa_auto_reconnect.task(thread_auto_reconnect);

  tarefa_servo.task(thread_servo);

}

void loop() {
  // Nothing will be run here / 此處不會被運行任何東西
}
