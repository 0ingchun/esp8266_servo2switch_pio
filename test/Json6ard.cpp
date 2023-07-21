#include <ArduinoJson.h>
void setup()
{
    // 初始化串口
    Serial.begin(115200);
}
void loop() {
    /* 构建JSON */
    StaticJsonDocument<200> jsonBuffer;   //声明一个JsonDocument对象，长度200
    // 添加对象节点
    jsonBuffer["key1"] = 123;          // 整数
    jsonBuffer["key2"] = 123.123;      // 浮点数
    jsonBuffer["key3"] = "string";     // 字符串
    // 添加数组对象
    JsonArray array1 = jsonBuffer.createNestedArray("array");// 添加一个 "array" 数组
    array1.add(1);    // 往数组里面添加值
    array1.add(1.2);  // 往数组里面添加值
    array1.add(-10);  // 往数组里面添加值
    // 对象嵌套
    JsonObject root = jsonBuffer.createNestedObject("key4"); //添加一个对象节点
    root["key1"] = "hello";
    // 输出结果：打印构建好的JSON数据
    String output;
    serializeJson(jsonBuffer, output);  // 序列化JSON数据并导出字符串
    Serial.println(output);             // 串口打印最后输出的字符串

    /* 构建JSON的另外一种方法 */
    DynamicJsonDocument doc(200);       // 声明对象的另外一种方法，和上面的 StaticJsonDocument<200> jsonBuffer 一样
    char string[100] = {"{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}"};  // 声明一个JSON数据
    deserializeJson(doc, string);       
    JsonObject obj = doc.as<JsonObject>(); 
    obj["sensor"] = "fan";          // 修改原来的值 
    obj["add"] = 123;               // 添加新对象 
    // 输出结果：打印构建好的JSON数据
    String output2;
    serializeJson(doc, output2);    // 序列化JSON数据并导出字符串
    Serial.println(output2);        // 串口打印最后输出的字符串
    delay(1000);

    /* 解析JSON */
    StaticJsonDocument<200> jsonBuffer2; //声明一个JsonDocument对象，长度200
    // 声明一个JSON数据，用作测试
    char json[] ="{\"str\":\"welcome\",\"data1\":1351824120,\"data2\":[48.756080,2.302038],\"object\":{\"key1\":-254}}";
    // 反序列化JSON
    DeserializationError error = deserializeJson(jsonBuffer2, json);
    if (error) 
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    // 解析JSON
    const char* str = jsonBuffer2["str"];           // 读取字符串
    long data1 = jsonBuffer2["data1"];              // 读取整形数据
    double latitude = jsonBuffer2["data2"][0];      // 读取数组
    double longitude = jsonBuffer2["data2"][1];     // 读取数组
    int key1 = jsonBuffer2["object"]["key1"];       // 读取嵌套对象
    // 输出结果：打印解析后的值
    Serial.println(str);      
    Serial.println(data1);
    Serial.println(latitude, 6);
    Serial.println(longitude, 6);
    Serial.println(key1);
    delay(1000);
}
