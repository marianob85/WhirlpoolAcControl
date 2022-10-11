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
#define DHT_PIN 2
#define DHT_READ_RATE 20000

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
#include <Syslog.h>
#include "MqttMessages.hpp"
#include "Whirlpool_YJ1B.hpp"
#include "DHTSensor.hpp"
#include "Config.hpp"
#include "WifiParameters.hpp"
#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3"

using namespace std;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
WhirlpoolYJ1B g_whirpool;
MqttClientForIR mqtt( &g_whirpool );
DHTSensor dhtSensor( DHT_PIN );
ESP8266HTTPUpdateServer httpUpdater;
// https://www.mischianti.org/2020/06/30/how-to-create-a-rest-server-on-esp8266-or-esp32-post-put-patch-delete-part-3/
ESP8266WebServer server( 80 );
ConfigDevice deviceConfig;

WiFiUDP udpSysLogClient;
Syslog syslog( udpSysLogClient, SYSLOG_PROTO_BSD );

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

void saveParamsCallback( const ConfigDevice* config )
{
	deviceConfig	   = *config;
	const auto& host   = config->host();
	const auto& mqtt   = config->mqtt();
	const auto& syslog = config->syslog();

	DynamicJsonDocument json( 512 );
	json[ "mqtt_server" ]	= mqtt.server;
	json[ "mqtt_port" ]		= mqtt.port;
	json[ "host_name" ]		= host.hostName;
	json[ "mqtt_user" ]		= mqtt.user;
	json[ "mqtt_password" ] = mqtt.password;
	json[ "mqtt_device" ]	= mqtt.name;
	json[ "syslog_server" ] = syslog.server;
	json[ "syslog_port" ]	= syslog.port;

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
	WiFiManager wifiManager;
	WifiParameters wifiParameters( &wifiManager, &deviceConfig );

	wifiManager.setHostname( deviceConfig.host().hostName.c_str() );
	wifiManager.setAPCallback( configModeCallback );

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

	auto& host	  = deviceConfig.host();
	host.hostName = json[ "host_name" ].as< string >();
	auto& mqtt	  = deviceConfig.mqtt();
	mqtt.name	  = json[ "host_name" ].as< string >();
	mqtt.password = json[ "mqtt_password" ].as< string >();
	mqtt.user	  = json[ "mqtt_user" ].as< string >();
	mqtt.port	  = json[ "mqtt_port" ].as< string >();
	mqtt.server	  = json[ "mqtt_server" ].as< string >();
	auto& syslog  = deviceConfig.syslog();
	syslog.server = json[ "syslog_server" ].as< string >();
	;
	syslog.port = json[ "syslog_port" ].as< string >();
	;
	file.close();
}

void onCommit( WhirlpoolYJ1B* data )
{
	Serial.print( "Commit: " );
	Serial.println( data->getJson().c_str() );
	syslog.log( data->getJson().c_str() );
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
	static auto lastTime = millis();
	const auto current	 = millis();
	if( current - lastTime > 1000 )
	{
		lastTime = current;
		state	 = true;
		digitalWrite( LED_BUILTIN, LOW );
	}
	if( state && current - lastTime > 5 )
	{
		state = false;
		digitalWrite( LED_BUILTIN, HIGH );
	}
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
	server.on(
		F( "/api/v1/status" ), HTTP_GET, []() { server.send( 200, F( "text/html" ), g_whirpool.getJson().c_str() ); } );
	server.on( F( "/api/v1/environment" ),
			   HTTP_GET,
			   []() { server.send( 200, F( "text/html" ), dhtSensor.getJson().c_str() ); } );
	server.on( F( "/api/v1/commit" ),
			   HTTP_PATCH,
			   []()
			   {
				   onCommit( &g_whirpool );
				   server.send( 200, F( "text/html" ), g_whirpool.getJson().c_str() );
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
	Serial.begin( 115200 );
	while( !Serial )
		;
	// initialize digital pin LED_BUILTIN as an output.
	pinMode( LED_BUILTIN, OUTPUT );

	if( const auto success = LittleFS.begin(); !success )
		hang( "Error mounting the file system" );

	readConfiguration();

	const auto& mqttConfig = deviceConfig.mqtt();
	mqtt.setup( mqttConfig.server.c_str(),
				std::stoi( mqttConfig.port ),
				mqttConfig.user.c_str(),
				mqttConfig.password.c_str(),
				mqttConfig.name.c_str(),
				onCommit );
	setupWifi();

	static const auto dhcp = WiFi.gatewayIP().toString();
	configTime( MY_TZ, dhcp.c_str() );

	const auto& hostConfig	 = deviceConfig.host();
	const auto& syslogConfig = deviceConfig.syslog();
	if( syslogConfig.isSet() )
	{
		syslog.server( syslogConfig.getServer(), syslogConfig.getPort() );
		syslog.deviceHostname( hostConfig.hostName.c_str() );
		syslog.appName( hostConfig.hostName.c_str() );
		syslog.defaultPriority( LOG_KERN );
		Serial.print( "Connected to syslog: " );
		Serial.print( syslogConfig.getServer() );
		Serial.print( ":" );
		Serial.println( syslogConfig.getPort() );
	}
	else
	{
		Serial.println( "Syslog not set" );
	}
	IrSender.begin( false );
	IrSender.enableIROut( AC_KHZ );

	httpUpdater.setup( &server, "/update" );

	// Set server routing
	restServerRouting();
	// Set not found response
	server.onNotFound( handleNotFound );
	// Start server
	server.begin();

	Serial.println( "Setup end." );
	syslog.log( "System startup" );
}

// the loop function runs over and over again forever
void loop()
{
	blink();
	dhtSensor.loop( DHT_READ_RATE );
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
		case '2':
			Serial.println( dhtSensor.getJson().c_str() );
			break;
		case 'r':
			syslog.log( "System reset to default" );
			WiFiManager wifiManager;
			wifiManager.resetSettings();
			ESP.reset();
			break;
		}
	}
}
