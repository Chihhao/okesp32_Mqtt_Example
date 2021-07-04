// 開發板選用 ESP32 Dev Module

#include "wifiboy_lib.h"
#include <Preferences.h>
#define KEY_R    32
#define KEY_L    33
#define KEY_B    34
#define KEY_A    35
#define KEY_U    36
#define KEY_D    39
#define KEY_MENU 0
#define BUZZER   17
#define SPI_MISO 12
#define SPI_MOSI 13
#define SPI_CLK  14
#define SPI_CS   15
#define TFT_DC   4
#define TFT_BKLT 27
#define IO_5     5
#define IO_LED   16
#define LCD_H    128
#define LCD_W    160
#define BLOCK_H  7

int bLast_KEY_R = LOW; // debounce used
int bLast_KEY_L = LOW; // debounce used
int bLast_KEY_B = LOW; // debounce used
int bLast_KEY_A = LOW; // debounce used
int bLast_KEY_U = LOW; // debounce used
int bLast_KEY_D = LOW; // debounce used
int bLast_KEY_MENU = LOW; // debounce used
int bNow_KEY_R = LOW; // debounce used
int bNow_KEY_L = LOW; // debounce used
int bNow_KEY_B = LOW; // debounce used
int bNow_KEY_A = LOW; // debounce used
int bNow_KEY_U = LOW; // debounce used
int bNow_KEY_D = LOW; // debounce used
int bNow_KEY_MENU = LOW; // debounce used
int debounceDelay = 20;  // debounce used

#include <WiFi.h>
WiFiClient espClient;
const char* ssid     = "ch_wifi";
const char* password = "wifi_CH_1201";

#include <PubSubClient.h>
PubSubClient client(espClient);
const char* mqttServer = "10.0.4.3";
const int mqttPort = 1883;
const char* mqttUser = "chihhaolai";
const char* mqttPassword = "1201";
const char* topic = "testTopic";

void init_keys(){
  pinMode(KEY_L,INPUT_PULLUP); 
  pinMode(KEY_R,INPUT_PULLUP); 
  pinMode(KEY_U,INPUT_PULLUP); 
  pinMode(KEY_D,INPUT_PULLUP);
  pinMode(KEY_B,INPUT_PULLUP); 
  pinMode(KEY_A,INPUT_PULLUP);
  pinMode(KEY_MENU,INPUT_PULLUP); 
  pinMode(IO_5,INPUT_PULLUP);
  pinMode(IO_LED, INPUT_PULLUP);
}

void init_screen(){
  wb_init(3);
  // 上半部狀態列
  iconWifi(wbRED); 
  iconMqtt(wbRED);  

  // send區塊
  wb_drawRect(0, 24, LCD_W, 52, wbBLUE);
  wb_fillRect(0, 24, 10, 52, wbBLUE);
  wb_setTextColor(wbWHITE, wbWHITE);
  wb_drawString("S", 3, 32, 1, 1);
  wb_drawString("E", 3, 42, 1, 1);
  wb_drawString("N", 3, 52, 1, 1);
  wb_drawString("D", 3, 62, 1, 1);
  
  // receive區塊
  wb_drawRect(0, 24+52, LCD_W, 52, wbYELLOW);
  wb_fillRect(0, 24+52, 10, 52, wbYELLOW);
  wb_setTextColor(wbBLACK, wbBLACK);
  wb_drawString("R", 3, 32+52, 1, 1);
  wb_drawString("E", 3, 42+52, 1, 1);
  wb_drawString("C", 3, 52+52, 1, 1);
  wb_drawString("V", 3, 62+52, 1, 1);
   
}

void checkWifi(){
  if(WiFi.status()== WL_CONNECTED){
    return;
  }
  WiFi.disconnect();
  delay(10);
  WiFi.begin(ssid,password);

  iconWifi(wbRED);
    
  while (WiFi.status()!= WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); //10.0.4.27
  
  iconWifi(wbGREEN);
}

void checkMQTT(){
  if(client.connected()){
    client.loop();
    return;
  }
  client.disconnect();
  delay(10);  

  iconMqtt(wbRED);

  client.setCallback(callback);
  client.setServer(mqttServer,mqttPort);
  while (!client.connected()){    
    Serial.println("Connecting to MQTT..");
    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
      client.subscribe(topic);
      Serial.println("MQTT Connected");
      iconMqtt(wbGREEN);
    }else {
      Serial.print("MQTT Connect fail: state: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  init_screen();
  init_keys();  
}

void loop(){ 
  checkWifi();
  checkMQTT(); 
  
  if (debounceRead(KEY_A, bNow_KEY_A, bLast_KEY_A)) {
    sendMqttMsg("Key_A pressed !");
  }
  if (debounceRead(KEY_B, bNow_KEY_B, bLast_KEY_B)) {
    sendMqttMsg("Key_B pressed !");
  }
  if (debounceRead(KEY_R, bNow_KEY_R, bLast_KEY_R)) {
    sendMqttMsg("Key_R pressed !");
  }
  if (debounceRead(KEY_L, bNow_KEY_L, bLast_KEY_L)) {
    sendMqttMsg("Key_L pressed !");
  }
  if (debounceRead(KEY_U, bNow_KEY_U, bLast_KEY_U)) {
    sendMqttMsg("Key_U pressed !");
  }
  if (debounceRead(KEY_D, bNow_KEY_D, bLast_KEY_D)) {
    sendMqttMsg("Key_D pressed !");
  }
  if (debounceRead(KEY_MENU, bNow_KEY_MENU, bLast_KEY_MENU)) {
    sendMqttMsg("123456789_123456789_123456789_123456789_123456789_");
  }

  delay(10);
}

void callback(char*topic, byte* payload, unsigned int length) {
  Serial.print("MQTT recv <--- "); Serial.println(topic);
  Serial.print("Message: ");
  char msg[length+1];
  for (int i = 0; i< length; i++) {
    msg[i]=(char)payload[i];
    Serial.print((char)payload[i]);
  }
  msg[length]='\0';
  Serial.println();
  Serial.println("-----------------------");

  wb_fillRect(10, 25+52, LCD_W-11, 52-2, wbBLACK); //清空文字框
  wb_setTextColor(wbBLACK, wbWHITE);
  wb_drawString(topic, 14, 32+52, 2, 2); //印TOPIC
  if(length<=24){
    wb_setTextColor(wbWHITE, wbWHITE);
    wb_drawString(msg, 14, 52+52, 2, 1);
  }
  else{
    char msg1[25];
    strncpy(msg1, msg, 24);
    msg1[24]='\0';
    wb_setTextColor(wbWHITE, wbWHITE); 
    wb_drawString(msg1, 14, 52+52, 2, 1);
    
    char msg2[25];
    strncpy(msg2, &msg[24], 24);
    msg2[24]='\0';
    wb_setTextColor(wbWHITE, wbWHITE);    
    wb_drawString(msg2, 14, 62+52, 2, 1);
  }

}

void sendMqttMsg(char* msg){
  client.publish(topic, msg, false);
  Serial.print("MQTT send ---> "); Serial.println(topic); 
  Serial.print("Message: "); Serial.println(msg);
  Serial.println("-----------------------");

  wb_fillRect(10, 25, LCD_W-11, 52-2, wbBLACK); //清空文字框
  wb_setTextColor(wbBLACK, wbWHITE);
  wb_drawString(topic, 14, 32, 2, 2); //印TOPIC

  if(strlen(msg)<=24){
    wb_setTextColor(wbWHITE, wbWHITE);
    wb_drawString(msg, 14, 52, 2, 1);    
  }
  else{
    char msg1[25];
    strncpy(msg1, msg, 24);
    msg1[24]='\0';
    wb_setTextColor(wbWHITE, wbWHITE); 
    wb_drawString(msg1, 14, 52, 2, 1);
    
    char msg2[25];
    strncpy(msg2, &msg[24], 24);
    msg2[24]='\0';
    wb_setTextColor(wbWHITE, wbWHITE);    
    wb_drawString(msg2, 14, 62, 2, 1);
  }
  
}

bool debounceRead(int button, int& buttonState, int& lastButtonState){
  
  int reading = !digitalRead(button);
  
  if (reading != lastButtonState) {
    delay(debounceDelay);
  }
       
  if (reading != buttonState) {
    buttonState = reading;  
    if (buttonState == HIGH) {          
      lastButtonState = reading; 
      return(HIGH);
    }
  }
  
  lastButtonState = reading;
  return(LOW);

}

void iconWifi(uint16_t color){
  wb_fillRect(2, 2, 42, 20, wbBLACK);
  wb_fillRect(2, 2, 42, 20, color);
  wb_setTextColor(wbBLACK, wbBLACK);
  wb_drawString("WiFi", 10, 5, 1, 2);
}

void iconMqtt(uint16_t color){
  wb_fillRect(46, 2, 42, 20, wbBLACK);
  wb_fillRect(46, 2, 42, 20, color);
  wb_setTextColor(wbBLACK, wbBLACK);
  wb_drawString("Mqtt", 50, 5, 1, 2);
}
