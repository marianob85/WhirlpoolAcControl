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
#include <NTPClient.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include "src/MqttMessages/MqttMessages.hpp"
#include "src/IRremote/IRremote.hpp"
#include "src/Whirlpool_YJ1B/Whirlpool_YJ1B.hpp"

char mqtt_server[ 40 ];
char mqtt_port[ 6 ]		 = "1883";
char host_name[ 20 ]	 = "IR_MQTT";
char mqtt_password[ 20 ] = "";
char mqtt_user[ 20 ]	 = "";
char mqtt_device[ 20 ]	 = "default";

WhirlpoolYJ1B g_whirpool;
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

MqttClientForIR mqtt;

WiFiUDP ntpUDP;
NTPClient timeClient( ntpUDP, "pool.ntp.org" );

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
	mqtt.setup( mqtt_server, std::stoi( mqtt_port ), mqtt_user, mqtt_password, mqtt_device );
	setupWifi();

	timeClient.begin();
	timeClient.setTimeOffset( 3600 );

	IrSender.begin( false );
	IrSender.enableIROut( AC_KHZ );

	Serial.println( "Setup end." );
}

void send()
{
	IrSender.mark( AC_HEADER_MARK );
	IrSender.space( AC_HEADER_SPACE );

	IrSender.sendPulseDistanceWidthRawData(
		AC_BIT_MARK, AC_ONE_SPACE, AC_BIT_MARK, AC_ZERO_SPACE, g_whirpool.data().raw, 68, true );

	delay( DELAY_AFTER_SEND );
}

// the loop function runs over and over again forever
void loop()
{
	digitalWrite( LED_BUILTIN, HIGH ); // turn the LED on (HIGH is the voltage level)
	delay( 100 );					   // wait for a second
	digitalWrite( LED_BUILTIN, LOW );  // turn the LED off by making the voltage LOW
	delay( 100 );					   // wait for a second
	Serial.println( "Alive!" );

	if( Serial.available() > 0 )
	{
		Serial.println( "Sending\n" );
		timeClient.update();
		g_whirpool.setClockAMPM( false )
			.setClockSeconds( timeClient.getSeconds() )
			.setClockMinutes( timeClient.getMinutes() )
			.setClockHours( timeClient.getHours() );

		const auto incomingByte = Serial.read();
		switch( incomingByte )
		{
		case '0':
			timeClient.update();
			Serial.println( timeClient.getFormattedTime() );
			break;
		case '1':
			g_whirpool.setMode( static_cast< Mode >( ( static_cast< uint8_t >( g_whirpool.getMode() ) + 1 )
													 % static_cast< uint8_t >( Mode::Heat ) ) );
			// g_whirpool.printDebug();
			send();
			break;
		case '2':
			WiFiManager wifiManager;
			wifiManager.resetSettings();
			ESP.reset();
			break;
		}
	}
}
