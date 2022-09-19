#define AC_KHZ 38
#define AC_UNIT 560						// 21.28 periods of 38 kHz
#define AC_HEADER_MARK ( 16 * AC_UNIT ) // 9000
#define AC_HEADER_SPACE ( 8 * AC_UNIT ) // 4500
#define AC_BIT_MARK AC_UNIT
#define AC_ONE_SPACE ( 3 * AC_UNIT ) // 1690
#define AC_ZERO_SPACE AC_UNIT
#define DELAY_AFTER_SEND 2000
#define RAW_BUFFER_LENGTH 0
#define IR_SEND_PIN 4

#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <time.h> // https://werner.rothschopf.net/202011_arduino_esp8266_ntp_en.htm
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <IRremote.hpp>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "MqttMessages/MqttMessages.hpp"
#include "Whirlpool_YJ1B/Whirlpool_YJ1B.hpp"

char mqtt_server[ 40 ];
char mqtt_port[ 6 ]		 = "1883";
char host_name[ 20 ]	 = "IR_MQTT";
char mqtt_password[ 20 ] = "";
char mqtt_user[ 20 ]	 = "";
char mqtt_device[ 20 ]	 = "default";

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

WhirlpoolYJ1B g_whirpool;
MqttClientForIR mqtt( &g_whirpool );

#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3"

Ticker blinker;
ESP8266HTTPUpdateServer httpUpdater;
// https://www.mischianti.org/2020/06/30/how-to-create-a-rest-server-on-esp8266-or-esp32-post-put-patch-delete-part-3/
ESP8266WebServer server( 80 );

void hang( const char* message )
{
	Serial.println( message );
	delay( 2000 );
	ESP.reset();
}

void configModeCallback( WiFiManager* myWiFiManager )
{
	Serial.println( "Entered config mode" );
	Serial.println( WiFi.softAPIP() );
	Serial.println( myWiFiManager->getConfigPortalSSID() );
}

void saveParamsCallback( const WiFiManagerParameter* server,
						 const WiFiManagerParameter* port,
						 const WiFiManagerParameter* hostName,
						 const WiFiManagerParameter* user,
						 const WiFiManagerParameter* password,
						 const WiFiManagerParameter* device )
{
	strncpy( mqtt_server, server->getValue(), sizeof( mqtt_server ) );
	strncpy( mqtt_port, port->getValue(), sizeof( mqtt_port ) );
	strncpy( host_name, hostName->getValue(), sizeof( host_name ) );
	strncpy( mqtt_user, user->getValue(), sizeof( mqtt_user ) );
	strncpy( mqtt_password, password->getValue(), sizeof( mqtt_password ) );
	strncpy( mqtt_device, device->getValue(), sizeof( mqtt_device ) );

	DynamicJsonDocument json( 512 );
	json[ "mqtt_server" ]	= mqtt_server;
	json[ "mqtt_port" ]		= mqtt_port;
	json[ "host_name" ]		= host_name;
	json[ "mqtt_user" ]		= mqtt_user;
	json[ "mqtt_password" ] = mqtt_password;
	json[ "mqtt_device" ]	= mqtt_device;

	File configFile = LittleFS.open( "/config.json", "w" );
	if( !configFile )
		hang( "failed to open config file for writing" );

	serializeJson( json, Serial );
	serializeJson( json, configFile );
	configFile.close();
}

void onWifiConnect( const WiFiEventStationModeGotIP& event )
{
	Serial.println( "Connected to Wi-Fi." );
	mqtt.connect();
}

void onWifiDisconnect( const WiFiEventStationModeDisconnected& event )
{
	Serial.println( "Disconnected from Wi-Fi." );
	mqtt.detach();
}

void setupWifi()
{
	WiFiManagerParameter custom_mqtt_server( "server", "MQTT server IP", mqtt_server, sizeof( mqtt_server ) );
	WiFiManagerParameter custom_mqtt_port( "port", "MQTT server port", mqtt_port, sizeof( mqtt_port ) );
	WiFiManagerParameter custom_hostname( "hostname", "Wifi host name", host_name, sizeof( host_name ) );
	WiFiManagerParameter custom_mqtt_user( "mqtt_user", "MQTT user", mqtt_user, sizeof( mqtt_user ) );
	WiFiManagerParameter custom_mqtt_password(
		"mqtt_password", "MQTT password", mqtt_password, sizeof( mqtt_password ) );
	WiFiManagerParameter custom_mqtt_device( "mqtt_device", "MQTT device", mqtt_device, sizeof( mqtt_device ) );

	WiFiManager wifiManager;
	wifiManager.setHostname( host_name );
	wifiManager.setAPCallback( configModeCallback );
	wifiManager.setSaveParamsCallback( std::bind( saveParamsCallback,
												  &custom_mqtt_server,
												  &custom_mqtt_port,
												  &custom_hostname,
												  &custom_mqtt_user,
												  &custom_mqtt_password,
												  &custom_mqtt_device ) );

	wifiManager.addParameter( &custom_mqtt_server );
	wifiManager.addParameter( &custom_mqtt_port );
	wifiManager.addParameter( &custom_hostname );

	wifiConnectHandler	  = WiFi.onStationModeGotIP( onWifiConnect );
	wifiDisconnectHandler = WiFi.onStationModeDisconnected( onWifiDisconnect );

	if( !wifiManager.autoConnect( "Manobit IR Config" ) )
	{
		Serial.println( "failed to connect and hit timeout" );
		// reset and try again, or maybe put it to deep sleep
		ESP.reset();
		delay( 1000 );
	}

	// if you get here you have connected to the WiFi
	Serial.println( F( "WIFIManager connected!" ) );

	Serial.print( F( "IP --> " ) );
	Serial.println( WiFi.localIP() );
	Serial.print( F( "GW --> " ) );
	Serial.println( WiFi.gatewayIP() );
	Serial.print( F( "SM --> " ) );
	Serial.println( WiFi.subnetMask() );
	Serial.print( F( "DNS 1 --> " ) );
	Serial.println( WiFi.dnsIP( 0 ) );
	Serial.print( F( "DNS 2 --> " ) );
	Serial.println( WiFi.dnsIP( 1 ) );
}

void readConfiguration()
{
	File file = LittleFS.open( "/config.json", "r" );
	if( !file )
		return;

	DynamicJsonDocument json( 512 );
	DeserializationError error = deserializeJson( json, file );
	if( error )
		hang( "Failed to parse json configuration" );

	strncpy( mqtt_server, json[ "mqtt_server" ], sizeof( mqtt_server ) );
	strncpy( mqtt_port, json[ "mqtt_port" ], sizeof( mqtt_port ) );
	strncpy( host_name, json[ "host_name" ], sizeof( host_name ) );
	strncpy( mqtt_user, json[ "mqtt_user" ], sizeof( mqtt_user ) );
	strncpy( mqtt_password, json[ "mqtt_password" ], sizeof( mqtt_password ) );
	strncpy( mqtt_device, json[ "mqtt_device" ], sizeof( mqtt_device ) );

	file.close();
}

void onCommit( WhirlpoolYJ1B* data )
{
	Serial.print( "Commit: " );
	Serial.println( data->get().c_str() );
	time_t now; // this is the epoch
	tm tm;
	time( &now );			  // read the current time
	localtime_r( &now, &tm ); // update the structure tm with the current time

	data->setClockHours( tm.tm_hour ).setClockMinutes( tm.tm_min ).setClockSeconds( tm.tm_sec );

	IrSender.mark( AC_HEADER_MARK );
	IrSender.space( AC_HEADER_SPACE );
	IrSender.sendPulseDistanceWidthRawData(
		AC_BIT_MARK, AC_ONE_SPACE, AC_BIT_MARK, AC_ZERO_SPACE, data->data().raw, 68, true );
}

void blink()
{
	static bool state{ false };
	digitalWrite( LED_BUILTIN, state ); // turn the LED on (HIGH is the voltage level)
	state = !state;
}

void showTime()
{
	time_t now; // this is the epoch
	tm tm;
	time( &now );			  // read the current time
	localtime_r( &now, &tm ); // update the structure tm with the current time
	Serial.print( "year:" );
	Serial.print( tm.tm_year + 1900 ); // years since 1900
	Serial.print( "\tmonth:" );
	Serial.print( tm.tm_mon + 1 ); // January = 0 (!)
	Serial.print( "\tday:" );
	Serial.print( tm.tm_mday ); // day of month
	Serial.print( "\thour:" );
	Serial.print( tm.tm_hour ); // hours since midnight  0-23
	Serial.print( "\tmin:" );
	Serial.print( tm.tm_min ); // minutes after the hour  0-59
	Serial.print( "\tsec:" );
	Serial.print( tm.tm_sec ); // seconds after the minute  0-61*
	Serial.print( "\twday" );
	Serial.print( tm.tm_wday ); // days since Sunday 0-6
	if( tm.tm_isdst == 1 )		// Daylight Saving Time flag
		Serial.print( "\tDST" );
	else
		Serial.print( "\tstandard" );
	Serial.println();
}

void restServerRouting()
{
	server.on( F( "/api/v1" ), HTTP_GET, []() { server.send( 200, F( "text/html" ), g_whirpool.get().c_str() ); } );
	server.on( F( "/api/v1/commit" ),
			   HTTP_PATCH,
			   []()
			   {
				   onCommit( &g_whirpool );
				   server.send( 200, F( "text/html" ), g_whirpool.get().c_str() );
			   } );
}

// Manage not found URL
void handleNotFound()
{
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for( uint8_t i = 0; i < server.args(); i++ )
	{
		message += " " + server.argName( i ) + ": " + server.arg( i ) + "\n";
	}
	server.send( 404, "text/plain", message );
}

// the setup function runs once when you press reset or power the board
void setup()
{
	// initialize digital pin LED_BUILTIN as an output.
	pinMode( LED_BUILTIN, OUTPUT );
	Serial.begin( 115200 );
	delay( 1000 );

	if( const auto success = LittleFS.begin(); !success )
		hang( "Error mounting the file system" );

	readConfiguration();
	mqtt.setup( mqtt_server, std::stoi( mqtt_port ), mqtt_user, mqtt_password, mqtt_device, onCommit );
	setupWifi();

	static const auto dhcp = WiFi.gatewayIP().toString();
	configTime( MY_TZ, dhcp.c_str() );

	IrSender.begin( false );
	IrSender.enableIROut( AC_KHZ );

	blinker.attach_ms( 200, blink );

	httpUpdater.setup( &server, "/update" );

	// Set server routing
	restServerRouting();
	// Set not found response
	server.onNotFound( handleNotFound );
	// Start server
	server.begin();

	Serial.println( "Setup end." );
}

// the loop function runs over and over again forever
void loop()
{
	server.handleClient();
	if( Serial.available() > 0 )
	{
		const auto incomingByte = Serial.read();
		switch( incomingByte )
		{
		case '0':
			showTime();
			break;
		case '1':
			onCommit( &g_whirpool );
			break;
		case 'r':
			WiFiManager wifiManager;
			wifiManager.resetSettings();
			ESP.reset();
			break;
		}
	}
}
