#include <Arduino.h>
#include "ArduinoJson.h"
 
String jsonStr = "{\"results\":[{\"location\":{\"id\":\"WW7MBNP039PE\",\"name\":\"泰安\",\"country\":\"CN\",\"path\":\"泰安,泰安,山东,中国\",\"timezone\":\"Asia/Shanghai\",\"timezone_offset\":\"+08:00\"},\"now\":{\"text\":\"晴\",\"code\":\"0\",\"temperature\":\"28\"},\"last_update\":\"2020-09-09T10:09:00+08:00\"}]}";
DynamicJsonDocument doc(512);
 
void setup()
{
  Serial.begin(115200);
  delay(3000);
}
void loop()
{
  //解析JSON字符串
  Serial.println("从jsonStr解码成的DynamicJsonDocument对象doc:");
  deserializeJson(doc, jsonStr);
  serializeJson(doc, Serial);
  Serial.println("从doc对象转换成的JsonObject类型对象root:");
  JsonObject root = doc.as<JsonObject>();
  serializeJson(root, Serial);
  Serial.println();
  Serial.println(doc.memoryUsage());
  Serial.println(measureJson(doc));
  Serial.println(measureJsonPretty(doc));
  Serial.println("root里有一个子JsonArray对象 results:");
  JsonArray results = root["results"];
  serializeJson(results, Serial);
  Serial.println();
  //JsonArray类型的数组results里面只有一个元素: JsonObject对象results[0],
  //该元素有三个子对象 location, now, last_update
  //location
  Serial.println("JsonObject对象 results[0]中的子JsonObject对象location:");
  JsonObject location = results[0]["location"];
  serializeJson(location, Serial);
  Serial.println();
  Serial.println("从JsonObject对象location中取出id");
  const char *id = location["id"];
  Serial.println(id);
  Serial.println("从JsonObject对象location中取出name");
  const char *name = location["name"];
  Serial.println(name);
  Serial.println("从JsonObject对象location中取出country");
  const char *country = location["country"];
  Serial.println(country);
  Serial.println("从JsonObject对象location中取出path");
  const char *path = location["path"];
  Serial.println(path);
  Serial.println("从JsonObject对象location中取出timezone");
  const char *timezone = location["timezone"];
  Serial.println(timezone);
  Serial.println("从JsonObject对象location中取出timezone_offset");
  const char *timezone_offset = location["timezone_offset"];
  Serial.println(timezone_offset);
  //now
  Serial.println("JsonObject对象 results[0]中的子JsonObject对象now(为了避免和timer中的now重名,把它叫做now1):");
  JsonObject now1 = results[0]["now"];
  serializeJson(now1, Serial);
  Serial.println("从JsonObject对象now1中取出text");
  const char *text = now1["text"];
  Serial.println(text);
  Serial.println("从JsonObject对象now1中取出code");
  const char *code = now1["code"];
  Serial.println(code);
  Serial.println("从JsonObject对象now1中取出temperature");
  const char *temperature = now1["temperature"];
  Serial.println(temperature);
  //last_update
  const char *last_update = results[0].getMember("last_update");
  Serial.println("last_update:");
  Serial.println(last_update);
  Serial.println();
  delay(10000);
}


#include <ArduinoJson.h>
 
void setup() {
  Serial.begin(115200);
  // 200 是大小 如果这个Json对象更加复杂，那么就需要根据需要去增加这个值.
  StaticJsonDocument<200> doc;
 
  // StaticJsonDocument 在栈区分配内存   它也可以被 DynamicJsonDocument（内存在堆区分配） 代替
  // DynamicJsonDocument  doc(200);
 
  //添加键值对
  doc["sensor"] = "gps";
  doc["time"] = 1351824120;
 
  // 添加数组.
  JsonArray data = doc.createNestedArray("data");
  data.add(48.756080);
  data.add(2.302038);
 
//串口打印结果
  serializeJson(doc, Serial);
}
 
void loop() {
}