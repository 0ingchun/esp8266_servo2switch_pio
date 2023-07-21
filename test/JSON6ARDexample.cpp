#include <ArduinoJson.h>

void setup() {

  Serial.begin(9600);
    
  DynamicJsonDocument doc(1024);
 
  // WARNING: the string in the input  will be duplicated in the JsonDocument.
  String input = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
  deserializeJson(doc, input);
  JsonObject obj = doc.as<JsonObject>();

  // You can use a String to get an element of a JsonObject
  // No duplication is done.
  long time = obj[String("time")];
  String sensor = obj["sensor"];
  double latitude = doc["data"][0];
  double longitude = doc["data"][1];

  // Print values.
  Serial.println(sensor);
  Serial.println(time);
  Serial.println(latitude, 6);
  Serial.println(longitude, 6);

  obj["sensor"] = "new gps";
  obj["code"] = 200;
  // Lastly, you can print the resulting JSON to a String
  String output;
  serializeJson(doc, output);

  Serial.println(output);
}

void loop() {
  // not used in this example
}
