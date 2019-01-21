#include <Arduino.h>
#include "ESPBASE.h"
#include "hw_timer.h"

WiFiClient espClient;
const byte mqttDebug = 1;
void zcDetectISR();
void dimTimerISR();

const byte zcPin = 12;
byte fade = 0;
byte curBrightness = 0;
byte tarBrightness = 255;
byte state = 1;
double dbrightness = 0.0;
double dfaderate = 1.0;
const byte outPin = 13;
byte zcState = 0; // 0 = ready; 1 = processing;
volatile int zc = 0;
volatile int zcount = 0;
volatile long tmcount = 0;
volatile long oldmillis = 0;
volatile long cycletime = 0;
long secondtimer = 0;
ESPBASE Esp;

void setup() 
{
  pinMode(zcPin, INPUT_PULLUP);
  pinMode(outPin, OUTPUT);
  Serial.begin(115200);
  Esp.initialize();
  mqttSubscribe();
  hw_timer_init(NMI_SOURCE, 0);
  hw_timer_set_func(dimTimerISR);
  attachInterrupt(zcPin, zcDetectISR, RISING);
}

void loop() 
{
  //  WebServer requests handling
  Esp.loop();
  if(secondtimer < millis())
  {
    secondtimer = millis() + 1000;
    Serial.print("cycle time = ");Serial.print(String(zc));Serial.print(" dbrightness=");Serial.print(String(dbrightness));Serial.print(" curBrightness=");Serial.print(String(curBrightness));Serial.print(" tmcount=");Serial.println(String(tmcount));
    zc = 0;
  }
}
 
void mqttLog(String logstr)
{
  Esp.mqttSend(config.mqtt_log_topic,"log: ",logstr);
}

void updateFade(bool newFade) 
{
  fade = newFade;
  
  if (Esp.mqttClient->connected()) 
  {
    String payload = (fade)?"ON":"OFF";
    if (mqttDebug) 
    {
      Serial.print("MQTT out: ");
      Serial.print(config.mqtt_fade_topic);
      Serial.print(" = ");
      Serial.println(payload);
    }
    Esp.mqttSend(config.mqtt_fade_topic, (fade) ?"ON":"OFF","");
  }
}

void updateState(bool newState) 
{
  mqttLog("old state = ");
  mqttLog(String(state));
  mqttLog(" new stat - ");
  mqttLog(String(newState));
  state = newState;
  
  if (Esp.mqttClient->connected()) 
  {
    String payload = (state)?"ON":"OFF";
    
    if (mqttDebug) 
    {
      Serial.print("MQTT out: ");
      Serial.print(config.mqtt_state_topic);
      Serial.print(" = ");
      Serial.println(payload);
    }
    
    Esp.mqttSend(config.mqtt_state_topic, payload,"");
  }
}

void updateBrightness(int newBrightness) 
{
  tarBrightness = newBrightness;
  
  if (Esp.mqttClient->connected()) 
  {
    String payload = String(tarBrightness);
//    String payload = String(zcount);
    
    if (mqttDebug) 
    {
      Serial.print("MQTT out: ");
      Serial.print(config.mqtt_brightness_topic);
      Serial.print(" = ");
      Serial.println(payload);
    }

    Esp.mqttSend(config.mqtt_brightness_topic, payload,"");
    Esp.mqttSend("Debug",String(tmcount),"");
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) 
{
  char c_payload[length];
  memcpy(c_payload, payload, length);
  c_payload[length] = '\0';
  
  String s_topic = String(topic);
  String s_payload = String(c_payload);
  
  if (mqttDebug) 
  {
    Serial.print("MQTT in: ");
    Serial.print(s_topic);
    Serial.print(" = ");
    Serial.println(s_payload);
  }

  if (s_topic == config.mqtt_state_topic) 
  {
    if (s_payload == "ON") 
    {
      if (mqttDebug) { Serial.println("Got the on"); }
      if (state != 1) 
      { 
          if (mqttDebug) { Serial.println("updating the state"); }
          updateState(1); 
      }
    }
    else if (s_payload == "OFF") 
    {
        if (state != 0) 
        { 
            updateState(0); 
        }
    }
  }
  else if (s_topic == config.mqtt_brightness_topic) 
  {
    if (mqttDebug) { Serial.println(""); }
    if (s_payload.toInt() != tarBrightness) 
    { 
        updateBrightness((byte) s_payload.toInt()); 
    }
  }
  else if (s_topic == config.mqtt_fade_topic) 
  {
    if (mqttDebug) { Serial.println(""); }
     
    if (s_payload == "ON") 
    {
      if (fade != 1) 
      { 
          updateFade(1); 
      }
    }
    else if (s_payload == "OFF") 
    {
      if (fade != 0) 
      { 
          updateFade(0); 
      }
    }
  }
  else if (s_topic == config.mqtt_faderate_topic) 
  {
    if (mqttDebug) { Serial.println(""); }
    char mybytes[s_payload.length()+1];
    s_payload.toCharArray(mybytes,s_payload.length()+1);
    Serial.println(mybytes);
    double newdfaderate = atof(mybytes); 
    Serial.println(newdfaderate);
    if (newdfaderate != dfaderate) 
    { 
      dfaderate = newdfaderate; 
      if (mqttDebug) { Serial.print("New dfaderate = "); }
      if (mqttDebug) { Serial.println(dfaderate); }
    }
  }
  else 
  {
    if (mqttDebug) { Serial.println(" [unknown message]"); }
  }
}

void mqttSubscribe()
{
  if (Esp.mqttClient->connected()) 
  {
    Serial.print("MQTT topic '");
    Serial.print(config.mqtt_state_topic);
    if (Esp.mqttClient->subscribe(config.mqtt_state_topic.c_str())) 
    {
      Serial.println("': Subscribed");
      Esp.mqttSend(String("subscribe"),config.DeviceName,String(": subscribed"));
    }
    else 
    {
      Serial.print("': Failed");
    }
    Serial.print("MQTT topic '");
    Serial.print(config.mqtt_brightness_topic);
    if (Esp.mqttClient->subscribe(config.mqtt_brightness_topic.c_str())) 
    {
      Serial.println("': Subscribed");
    }
    else 
    {
      Serial.print("': Failed");
    }
    Serial.print("MQTT topic '"); 
    Serial.print(config.mqtt_faderate_topic);
    if (Esp.mqttClient->subscribe(config.mqtt_faderate_topic.c_str())) 
    {
      Serial.println("': Subscribed");
    }
    else 
    {
      Serial.print("': Failed");
    }
    Serial.print("MQTT topic '"); 
    Serial.print(config.mqtt_fade_topic);
    if (Esp.mqttClient->subscribe(config.mqtt_fade_topic.c_str())) 
    {
      Serial.println("': Subscribed");
    }
    else 
    {
      Serial.print("': Failed");
    }
  }
}

void dimTimerISR() 
{
//    if(!allowinterrupts)
//      return;
    tmcount++;
    if (fade == 1) 
    {
      if (curBrightness > tarBrightness || (state == 0 && curBrightness > 0)) 
      {
        dbrightness = dbrightness-dfaderate;
        curBrightness = (int) dbrightness;
      }
      else if (curBrightness < tarBrightness && state == 1 && curBrightness < 255) 
      {
        dbrightness = dbrightness+dfaderate;
        curBrightness = (int) dbrightness;
      }
    }
    else 
    {
      if (state == 1) 
      {
        curBrightness = tarBrightness;
        dbrightness = curBrightness;
      }
      else 
      {
        curBrightness = 0;
        dbrightness = 0.0;
      }
    }
    
    if (curBrightness == 0) 
    {
      state = 0;
      digitalWrite(outPin, 0);
    }
    else if (curBrightness == 255) 
    {
      state = 1;
      digitalWrite(outPin, 1);
    }
    else 
    {
      digitalWrite(outPin, 1);
    }
    
    zcState = 0;
}

void zcDetectISR() 
{
//  if(!allowinterrupts)
//    return;
//  cycletime = millis() - oldmillis;
//  oldmillis = millis();
  zc++;
  zcount++;
  if (zcState == 0) 
  {
    zcState = 1;
  
    if (curBrightness < 255 && curBrightness > 0) 
    {
      digitalWrite(outPin, 0);
      
      int dimDelay = 30 * (255 - curBrightness) + 400;
      hw_timer_arm(dimDelay);
    }
  }
}
