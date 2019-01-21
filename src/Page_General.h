//
//  HTML PAGE
//

const char PAGE_AdminGeneralSettings[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>General Settings</strong>
<hr>
<form action="" method="post">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
	<td align="right">Name of Device</td>
	<td><input type="text" id="devicename" name="devicename" value=""></td>
</tr>
<tr>
  <td align="right">OTA Password</td>
  <td><input type="text" id="OTApwd" name="OTApwd" value=""></td>
</tr>
<tr>
  <td align="right">MQTT Server</td>
  <td><input type="text" id="MQTTServer" name="MQTTServer" value=""></td>
</tr>
<tr>
  <td align="right">MQTT Port</td>
  <td><input type="text" id="MQTTPort" name="MQTTPort" value=""></td>
</tr>
<tr>
  <td align="right">Heartbeat Topic</td>
  <td><input type="text" id="HeartbeatTopic" name="HeartbeatTopic" value=""></td>
</tr>
<tr>
  <td align="right">Heartbeat Every Seconds</td>
  <td><input type="text" id="HeartbeatEvery" name="HeartbeatEvery" value=""></td>
</tr>
<tr>
  <td align="right">MQTT State Topic</td>
  <td><input type="text" id="mqtt_state_topic" name="mqtt_state_topic" value=""></td>
</tr>
<tr>
  <td align="right">Brightness Topic</td>
  <td><input type="text" id="mqtt_brightness_topic" name="mqtt_brightness_topic" value=""></td>
</tr>
<tr>
  <td align="right">Fade Topic</td>
  <td><input type="text" id="mqtt_fade_topic" name="mqtt_fade_topic" value=""></td>
</tr>
<tr>
  <td align="right">PIR state topic</td>
  <td><input type="text" id="pir_state_topic" name="pir_state_topic" value=""></td>
</tr>
<tr>
  <td align="right">Faderate Topic</td>
  <td><input type="text" id="mqtt_faderate_topic" name="mqtt_faderate_topic" value=""></td>
</tr>
<tr>
  <td align="right">Get State Topic</td>
  <td><input type="text" id="mqtt_gimmestate_topic" name="mqtt_gimmestate_topic" value=""></td>
</tr>
<tr>
  <td align="right">not used</td>
  <td><input type="text" id="notused" name="notused" value=""></td>
</tr>
<tr>
  <td align="right">Start Topic</td>
  <td><input type="text" id="mqtt_start_topic" name="mqtt_start_topic" value=""></td>
</tr>
<tr>
  <td align="right">Not Used</td>
  <td><input type="text" id="mqtt_heartbeat" name="mqtt_heartbeat" value=""></td>
</tr>
<tr>
  <td align="right">Log Topic</td>
  <td><input type="text" id="mqtt_log_topic" name="mqtt_log_topic" value=""></td>
</tr>
<tr>
  <td align="right">Last Will and Testement Topic</td>
  <td><input type="text" id="mqtt_lwt_topic" name="mqtt_lwt_topic" value=""></td>
</tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<script>

 

window.onload = function ()
{
	load("style.css","css", function() 
	{
		load("microajax.js","js", function() 
		{
				setValues("/admin/generalvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


// Functions for this Page
void send_devicename_value_html()
{
		
	String values ="";
	values += "devicename|" + (String) config.DeviceName + "|div\n";
  values += "OTApwd|" + (String) config.OTApwd + "|div\n";
  values += "MQTTServer|" + (String) config.MQTTServer + "|div\n";
  values += "MQTTPort|" + (String) config.MQTTPort + "|div\n";
  values += "HeartbeatTopic|" + (String) config.HeartbeatTopic + "|div\n";
  values += "HeartbeatEvery|" + (String) config.HeartbeatEvery + "|div\n";
  values += "mqtt_state_topic|" + (String) config.mqtt_state_topic + "|div\n";
  values += "mqtt_brightness_topic|" + (String) config.mqtt_brightness_topic + "|div\n";
  values += "mqtt_fade_topic|" + (String) config.mqtt_fade_topic + "|div\n";
  values += "pir_state_topic|" + (String) config.pir_state_topic + "|div\n";
  values += "mqtt_faderate_topic|" + (String) config.mqtt_faderate_topic + "|div\n";
  values += "mqtt_gimmestate_topic|" + (String) config.mqtt_gimmestate_topic + "|div\n";
  values += "notused|" + (String) config.mqtt_mystate_topic + "|div\n";
  values += "mqtt_start_topic|" + (String) config.mqtt_start_topic + "|div\n";
  values += "mqtt_heartbeat|" + (String) config.mqtt_heartbeat + "|div\n";
  values += "mqtt_log_topic|" + (String) config.mqtt_log_topic + "|div\n";
  values += "mqtt_lwt_topic|" + (String) config.mqtt_lwt_topic + "|div\n";
  Serial.println(values);
	server.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 
	
}

void send_general_html()
{
	
	if (server.args() > 0 )  // Save Settings
	{	
		String temp = "";
		for ( uint8_t i = 0; i < server.args(); i++ ) {
			if (server.argName(i) == "devicename") config.DeviceName = urldecode(server.arg(i)); 
      if (server.argName(i) == "OTApwd") config.OTApwd = urldecode(server.arg(i));
      if (server.argName(i) == "MQTTServer") config.MQTTServer = urldecode(server.arg(i));
      if (server.argName(i) == "MQTTPort") config.MQTTPort = server.arg(i).toInt();
      if (server.argName(i) == "HeartbeatTopic") config.HeartbeatTopic = urldecode(server.arg(i));
      if (server.argName(i) == "HeartbeatEvery") config.HeartbeatEvery = server.arg(i).toInt();
      if (server.argName(i) == "mqtt_state_topic"){ config.mqtt_state_topic = urldecode(server.arg(i));      Serial.print("set state topic=");Serial.println(config.mqtt_state_topic);}
      if (server.argName(i) == "mqtt_brightness_topic") {config.mqtt_brightness_topic = urldecode(server.arg(i));Serial.print("set brightness topic=");Serial.println(config.mqtt_brightness_topic);}
      if (server.argName(i) == "mqtt_fade_topic") config.mqtt_fade_topic = urldecode(server.arg(i));
      if (server.argName(i) == "pir_state_topic") config.pir_state_topic = urldecode(server.arg(i));
      if (server.argName(i) == "mqtt_faderate_topic") config.mqtt_faderate_topic = urldecode(server.arg(i));
      if (server.argName(i) == "mqtt_gimmestate_topic") config.mqtt_gimmestate_topic = urldecode(server.arg(i));
      if (server.argName(i) == "notused") config.mqtt_mystate_topic = urldecode(server.arg(i));
      if (server.argName(i) == "mqtt_start_topic") config.mqtt_start_topic = urldecode(server.arg(i));
      if (server.argName(i) == "mqtt_heartbeat") config.mqtt_heartbeat = urldecode(server.arg(i));
      if (server.argName(i) == "mqtt_log_topic") config.mqtt_log_topic = urldecode(server.arg(i));
      if (server.argName(i) == "mqtt_lwt_topic") {config.mqtt_lwt_topic = urldecode(server.arg(i));Serial.print("set lwt topic=");Serial.println(config.mqtt_lwt_topic);}
		}
		WriteConfig();
		firstStart = true;
	}
	server.send_P ( 200, "text/html", PAGE_AdminGeneralSettings ); 
	Serial.println(__FUNCTION__); 
}

void send_general_configuration_values_html()
{
	String values ="";
	values += "devicename|" +  (String)  config.DeviceName +  "|input\n";
  values += "OTApwd|" +  (String)  config.OTApwd +  "|input\n";
  values += "MQTTServer|" +  (String)  config.MQTTServer +  "|input\n";
  values += "MQTTPort|" +  (String)  config.MQTTPort +  "|input\n";
  values += "HeartbeatTopic|" +  (String)  config.HeartbeatTopic +  "|input\n";
  values += "HeartbeatEvery|" +  (String)  config.HeartbeatEvery +  "|input\n";
  values += "mqtt_state_topic|" +  (String)  config.mqtt_state_topic +  "|input\n";
  values += "mqtt_brightness_topic|" +  (String)  config.mqtt_brightness_topic +  "|input\n";
  values += "mqtt_fade_topic|" +  (String)  config.mqtt_fade_topic +  "|input\n";
  values += "pir_state_topic|" +  (String)  config.pir_state_topic +  "|input\n";
  values += "mqtt_faderate_topic|" +  (String)  config.mqtt_faderate_topic +  "|input\n";
  values += "mqtt_gimmestate_topic|" +  (String)  config.mqtt_gimmestate_topic +  "|input\n";
  values += "notused|" +  (String)  config.mqtt_mystate_topic +  "|input\n";
  values += "mqtt_start_topic|" +  (String)  config.mqtt_start_topic +  "|input\n";
  values += "mqtt_heartbeat|" +  (String)  config.mqtt_heartbeat +  "|input\n";
  values += "mqtt_log_topic|" +  (String)  config.mqtt_log_topic +  "|input\n";
  values += "mqtt_lwt_topic|" +  (String)  config.mqtt_lwt_topic +  "|input\n";
  Serial.println(values);
	server.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__); 
  AdminTimeOutCounter=0;
}
