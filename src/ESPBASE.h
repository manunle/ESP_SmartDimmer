/*
    ESP Base
    Copyright (C) 2017  Pedro Albuquerque

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



/*
 * * ESP8266 template with phone config web page
 * based on BVB_WebConfig_OTA_V7 from Andreas Spiess https://github.com/SensorsIot/Internet-of-Things-with-ESP8266
 *
 */
#ifndef ESPBASE_H
#define ESPBASE_H

#include <Arduino.h>

#include <string.h>

#ifdef ARDUINO_ESP32_DEV
    //ESP32 specific here
    #include <ESP32PWM.h>
    #include <ESP32WebServer.h>
    #include <WiFi.h>
    #include <ESPmDNS.h>
    #include <WiFiUdp.h>
    #include <ArduinoOTA.h>
    #include <Ticker-ESP32.h>
#elif ARDUINO_ESP8266_ESP01 || ARDUINO_ESP8266_NODEMCU
    // ESP8266 specific here
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
  #include <ESP8266WebServer.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  #include <Ticker.h>
  #include <EEPROM.h>
  #include <PubSubClient.h>
  extern "C" {
  #include "user_interface.h"
  }
#endif



class ESPBASE {
public:
    bool WIFI_connected, CFG_saved;
    void initialize();
    void httpSetup();
    void OTASetup();
    WiFiClient espClient;
    PubSubClient* mqttClient;
    void setupMQTTClient();
    void mqttSend(String topic,String preface,String msg);
    void loop();
    long lastReconnectAttempt = 0;
};

#include "parameters.h"
#include "global.h"
#include "wifiTools.h"
#include "NTP.h"


// Include the HTML, STYLE and Script "Pages"
#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPsettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "PAGE_NetworkConfiguration.h"


void mqttCallback(char* topic, byte* payload, unsigned int length);
//char tmpESP[100];
bool allowinterrupts=true;
void mqttSubscribe();

void ESPBASE::initialize(){

  allowinterrupts = true;
  CFG_saved = false;
  WIFI_connected = false;
  uint8_t timeoutClick = 50;
  mqttClient = new PubSubClient(espClient);
  String chipID;

  // define parameters storage
  #ifdef ARDUINO_ESP32_DEV
    EEPROM.begin("ESPBASE", false);
    PWM_initialize(LED_esp,256,0,5000,13);
  #elif ARDUINO_ESP8266_NODEMCU || ARDUINO_ESP8266_ESP01
    EEPROM.begin(1024); // define an EEPROM space of 1024 Bytes to store data
    String cfgver = CFGVER;
    char ccfgver[5];
    cfgver.toCharArray(ccfgver,5);
    EEPROM.write(0, ccfgver[0]);
    EEPROM.write(1, ccfgver[1]);
    EEPROM.write(2, ccfgver[2]);
    EEPROM.write(3, ccfgver[3]);
    WriteStringToEEPROM(26, "mtolive");
    WriteStringToEEPROM(58, "grizzlymatt");
    WriteStringToEEPROM(186, "192.168.1.6");
    EEPROMWritelong(218, 1883);
    WriteStringToEEPROM(90, "");
    WriteStringToEEPROM(122, "LightDimmer");
    WriteStringToEEPROM(154, "");
    WriteStringToEEPROM(222,"lights/heartbeat");                // up to 32 Byte - EEPROM 260
    EEPROMWritelong(254,60);                  // 4 Byte - EEPROM 292
    WriteStringToEEPROM(258,"lights/state");              // up to 32 Bytes EEPROM 258
    WriteStringToEEPROM(290,"lights/brightness");         // up to 32 Bytes EEPROM 290
    WriteStringToEEPROM(322,"lights/fade");               // up to 32 Bytes EEPROM 322
    WriteStringToEEPROM(354,"lights/motion");               // up to 32 Bytes EEPROM 354
    WriteStringToEEPROM(386,"lights/faderate");           // up to 32 Bytes EEPROM 386
    WriteStringToEEPROM(418,"lights/gimmestate");         // up to 32 Bytes EEPROM 418
    WriteStringToEEPROM(450,"");            // up to 32 Bytes EEPROM 450
    WriteStringToEEPROM(482,"lights/startup");              // up to 32 Bytes EEPROM 482
    WriteStringToEEPROM(514,"");                // up to 32 Bytes EEPROM 514
    WriteStringToEEPROM(546,"log/lights");                // up to 32 Bytes EEPROM 546
    WriteStringToEEPROM(578,"lights/RIP");                // up to 32 Bytes EEPROM 578

    EEPROM.commit();
  #endif

  //**** Network Config load
  CFG_saved = ReadConfig();
  Serial.print("OKKKKKK I got config ver ");
  Serial.println(CFGVER);
  //  Connect to WiFi access point or start as Access point
  if (CFG_saved) { //if config saved use saved values

      // Connect the ESP8266 to local WIFI network in Station mode
      // using SSID and password saved in parameters (config object)
      Serial.println("Booting");
      //printConfig();
      WiFi.mode(WIFI_OFF);
      WiFi.mode(WIFI_STA);
      WiFi.begin(config.ssid.c_str(), config.password.c_str());
      Serial.print("millis from:");Serial.println(millis());
      //WIFI_connected = WiFi.waitForConnectResult();
      while((WiFi.status()!= WL_CONNECTED) and --timeoutClick > 0) {
        delay(500);
        Serial.print(".");
      }
      Serial.print("millis to:");Serial.println(millis());
      if(WiFi.status()!= WL_CONNECTED )
      {
          Serial.println("Connection Failed! activating to AP mode...");
          WIFI_connected = false;
      }
      else
      {
        WIFI_connected = true;
        Serial.println("****** Connected! ******");
      }
      Serial.print("Wifi ip:");Serial.println(WiFi.localIP());
      setupMQTTClient();
  }

  if ( !WIFI_connected or !CFG_saved){ // if no values saved or not good use defaults
      WiFi.mode(WIFI_OFF);
      WiFi.mode(WIFI_STA);
      WiFi.begin("mtolive", "grizzlymatt");
      Serial.print("millis from:");Serial.println(millis());
      //WIFI_connected = WiFi.waitForConnectResult();
      while((WiFi.status()!= WL_CONNECTED) and --timeoutClick > 0) {
        delay(500);
        Serial.print(".");
      }
      Serial.print("millis to:");Serial.println(millis());
      Serial.print("Wifi ip:");Serial.println(WiFi.localIP());
/*    IPAddress local_IP(192,168,4,1);
    IPAddress gateway(192,168,4,1);
    IPAddress subnet(255,255,255,0);
    // DEFAULT CONFIG
    Serial.println("Setting AP mode default parameters");

    //load config with default values
    configLoadDefaults(getChipId());

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_IP,gateway,subnet);
    WiFi.softAP(config.ssid.c_str());
    Serial.print("Wifi ip:");Serial.println(WiFi.softAPIP());*/
   }

    //  Http Setup

    httpSetup();

    // ***********  OTA SETUP
    OTASetup();


    // start internal time update ISR
    #ifdef ARDUINO_ESP32_DEV
      tkSecond.attach(1000000, ISRsecondTick);
    #elif ARDUINO_ESP8266_ESP01 || ARDUINO_ESP8266_NODEMCU
      tkSecond.attach(1, ISRsecondTick);
    #endif

    Serial.println("Ready");

}


void ESPBASE::httpSetup(){
  // Start HTTP Server for configuration
  server.on ( "/", []() {
    Serial.println("admin.html");
    server.send_P ( 200, "text/html", PAGE_AdminMainPage);  // const char top of page
  }  );
  server.on ( "/favicon.ico",   []() {
    Serial.println("favicon.ico");
    server.send( 200, "text/html", "" );
  }  );
  // Network config
  server.on ( "/config.html", send_network_configuration_html );
  // Info Page
  server.on ( "/info.html", []() {
    Serial.println("info.html");
    server.send_P ( 200, "text/html", PAGE_Information );
  }  );
  server.on ( "/ntp.html", send_NTP_configuration_html  );

  //server.on ( "/appl.html", send_application_configuration_html  );
  server.on ( "/general.html", send_general_html  );
  //  server.on ( "/example.html", []() { server.send_P ( 200, "text/html", PAGE_EXAMPLE );  } );
  server.on ( "/style.css", []() {
    Serial.println("style.css");
    server.send_P ( 200, "text/plain", PAGE_Style_css );
  } );
  server.on ( "/microajax.js", []() {
    Serial.println("microajax.js");
    server.send_P ( 200, "text/plain", PAGE_microajax_js );
  } );
  server.on ( "/admin/values", send_network_configuration_values_html );
  server.on ( "/admin/connectionstate", send_connection_state_values_html );
  server.on ( "/admin/infovalues", send_information_values_html );
  server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
  //server.on ( "/admin/applvalues", send_application_configuration_values_html );
  server.on ( "/admin/generalvalues", send_general_configuration_values_html);
  server.on ( "/admin/devicename",     send_devicename_value_html);
  server.onNotFound ( []() {
    Serial.println("Page Not Found");
    server.send ( 400, "text/html", "Page not Found" );
  }  );
  server.begin();
  Serial.println( "HTTP server started" );

  return;
}

void ESPBASE::OTASetup(){

      //ArduinoOTA.setHostname(host);
      ArduinoOTA.onStart([]() { // what to do before OTA download insert code here
          allowinterrupts = false;
          noInterrupts();
          Serial.println("Start");
        });
      ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      });
      ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
          for (int i=0;i<30;i++){
            analogWrite(LED_esp,(i*100) % 1001);
            delay(50);
          }
          digitalWrite(LED_esp,HIGH); // Switch OFF ESP LED to save energy
          Serial.println("\nEnd");
          ESP.restart();
        });

      ArduinoOTA.onError([](ota_error_t error) {
          Serial.printf("Error[%u]: ", error);
          if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
          else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
          else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
          else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
          else if (error == OTA_END_ERROR) Serial.println("End Failed");
          ESP.restart();
        });

       /* setup the OTA server */
      ArduinoOTA.setPassword(config.OTApwd.c_str());    // ********    to be implemented as a parameter via Browser
      ArduinoOTA.begin();

}

void ESPBASE::setupMQTTClient() {
  int connectResult;
  
  if (config.MQTTServer != "") {
//    Serial.print("MQTT client: ");
    mqttClient->setServer(config.MQTTServer.c_str(), config.MQTTPort);
//    Serial.print("MQTT setting callback: ");
    mqttClient->setCallback(mqttCallback);
//    Serial.print("MQTT connecting");
//    if (configuration.mqtt_user == "") {
      connectResult = mqttClient->connect(config.DeviceName.c_str(),"RIP",0,false,config.DeviceName.c_str());
//    }
//    else {
//      connectResult = mqttClient.connect(config.DeviceName, configuration.mqtt_user, configuration.mqtt_password);
//    }
//    Serial.print("Did i connect");
    if (connectResult) {
//      Serial.println("Connected");
    }
    else {
//      Serial.print("Failed (");
//      Serial.print(mqttClient.state());
//      Serial.println(")");
    }
    
//    if (mqttClient->connected()) {
//      Serial.print("MQTT topic '");
//      Serial.print(mqtt_configuration_topic);
//      if (mqttClient->subscribe("TestTopic")) {
//        Serial.println("': Subscribed");
//        mqttSend(String("TestChip"),config.DeviceName,String(": subscribed"));
//        mqttSend(String("TestChip"),String("Test"),String(": subscribed"));
//      }
//      else {
//        Serial.print("': Failed");
//      }
//    }
  }
}

void ESPBASE::mqttSend(String topic,String preface,String msg)
{
  Serial.println(topic);
  Serial.println(preface);
  Serial.println(msg);
  String soutput=preface+msg;
  char mybytes[soutput.length()+1];
  char topicbytes[topic.length()+1];
  topic.toCharArray(topicbytes,topic.length()+1);  
  soutput.toCharArray(mybytes,soutput.length()+1);
//  mqttClient.publish(topicbytes,mybytes);
  mqttClient->publish(topic.c_str(),soutput.c_str());
}

void ESPBASE::loop()
{
  ArduinoOTA.handle();
  server.handleClient();

  customWatchdog = millis();

  if(WIFI_connected)
  {
    if(config.MQTTServer != "")
    {
      if (!mqttClient->connected()) 
      {
        long now = millis();
        if (now - lastReconnectAttempt > 5000) 
        {
          Serial.println("mqtt NOT connected");
          lastReconnectAttempt = now;
          // Attempt to reconnect
          bool connectResult = mqttClient->connect(config.DeviceName.c_str(),"RIP",0,false,config.DeviceName.c_str());
          if (connectResult) 
          {
            mqttSubscribe();
            String recon = "Recon";
            mqttSend(recon,config.DeviceName,": Reconnected");
            lastReconnectAttempt = 0;
          }
        }
      } else {
        // Client connected
        mqttClient->loop();
      }
    
      if(cHeartbeat >= config.HeartbeatEvery and config.HeartbeatEvery > 0)
      {
        cHeartbeat = 0;
        mqttSend(config.HeartbeatTopic,config.DeviceName," Still Here");
      }
    }
  }
  yield();

}
#endif
