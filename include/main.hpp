#pragma once

/*----------------------------------------------外部库------------------------------------------------*/

#include <Arduino.h>
#include <ESP8266WiFi.h>


#include<cstring>
#include<string>

/*---------------------------------------------用户库-------------------------------------------------*/

//#include "HTTPClient.h"

#include "ritos.h"

#include "ESP8266WebServer.h"
#include "ESP8266_ISR_Servo.h"
#include "PubSubClient.h"
#include "ArduinoJson.h"

/*---------------------------------------------定义参数-------------------------------------------------*/

const char* ssid = "White-House";
const char* password = "qazwsxedcrfv123!";
const char* mqtt_server = "192.168.2.200";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(500) //50  //消息长度
char msg[MSG_BUFFER_SIZE];
int value = 0;

String clientId = "ESP8266Client-lcdStation" + String(random(0xffff), HEX), clientUser = "user8266", clientPasswd = "88888888";

#define LED_PIN 2

#ifndef ESP8266
  #error This code is designed to run on ESP8266 platform! Please check your Tools->Board setting.
#endif

#define TIMER_INTERRUPT_DEBUG       1
#define ISR_SERVO_DEBUG             1

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "ESP8266_ISR_Servo.h"

// Published values for SG90 servos; adjust if needed
#define MIN_MICROS      800  //544
#define MAX_MICROS      2450

#define NUM_SERVOS    3

typedef struct
{
  int     servoIndex;
  uint8_t servoPin;
} ISR_servo_t;

ISR_servo_t ISR_servo[NUM_SERVOS] =
{
  { -1, 14 }, { -1, 12 }, { -1, 13 }
};