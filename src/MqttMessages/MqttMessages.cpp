#include "MqttMessages.hpp"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <stdex.hpp>

using namespace std;
using namespace std::literals;

/*
Root: /manobit/AC/Sypialnia
Status:
	- / -> json
	- /temperature
Commands:
	- /cmd/commit
	- /cmd/temperature/XX ( in celcius )
	- /cmd/temperature ( payload raw data )
*/

constexpr std::string_view mqttRoot = "manobit/AC/";

MqttClientForIR::MqttClientForIR( WhirlpoolYJ1B* whirpoolData ) : m_device( mqttRoot ), m_whirpoolData( whirpoolData )
{
}

void MqttClientForIR::setup(
	const char* server, uint16_t port, const char* user, const char* password, const char* device )
{
	m_device += device;
	m_mqttClient.onConnect( std::bind( &MqttClientForIR::onConnect, this, std::placeholders::_1 ) );
	m_mqttClient.onDisconnect( std::bind( &MqttClientForIR::onDisconnect, this, std::placeholders::_1 ) );
	m_mqttClient.onSubscribe(
		std::bind( &MqttClientForIR::onSubscribe, this, std::placeholders::_1, std::placeholders::_2 ) );
	m_mqttClient.onUnsubscribe( std::bind( &MqttClientForIR::onUnsubscribe, this, std::placeholders::_1 ) );
	m_mqttClient.onPublish( std::bind( &MqttClientForIR::onPublish, this, std::placeholders::_1 ) );
	m_mqttClient.onMessage( std::bind( &MqttClientForIR::onMessage,
									   this,
									   std::placeholders::_1,
									   std::placeholders::_2,
									   std::placeholders::_3,
									   std::placeholders::_4,
									   std::placeholders::_5,
									   std::placeholders::_6 ) );

	m_mqttClient.setServer( server, port );
	m_mqttClient.setCredentials( user, password );
}

void MqttClientForIR::detach()
{
	m_mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
}

void MqttClientForIR::connect()
{
	Serial.println( "Connecting to MQTT..." );
	m_mqttClient.connect();
}

void MqttClientForIR::onDisconnect( AsyncMqttClientDisconnectReason reason )
{
	Serial.println( "Disconnected from MQTT." );
	if( WiFi.isConnected() )
		m_mqttReconnectTimer.once( 2, std::bind( &MqttClientForIR::connect, this ) );
}

void MqttClientForIR::onConnect( bool sessionPresent )
{
	Serial.println( "Connected to MQTT." );
	Serial.print( "Session present: " );
	Serial.println( sessionPresent );

	m_mqttClient.subscribe( ( m_device + "/cmd/#" ).c_str(), 2 );

	sendInitValues();
	// m_mqttClient.publish( ( m_device + "/status" ).c_str(), 0, true, "test 1" );
}

void MqttClientForIR::onPublish( uint16_t packetId )
{
	Serial.print( "Publish acknowledged." );
	Serial.print( "  packetId: " );
	Serial.println( packetId );
}

void MqttClientForIR::onSubscribe( uint16_t packetId, uint8_t qos )
{
	Serial.println( "Subscribe acknowledged." );
	Serial.print( "  packetId: " );
	Serial.println( packetId );
	Serial.print( "  qos: " );
	Serial.println( qos );
}

void MqttClientForIR::onUnsubscribe( uint16_t packetId )
{
	Serial.println( "Unsubscribe acknowledged." );
	Serial.print( "  packetId: " );
	Serial.println( packetId );
}

void MqttClientForIR::onMessage(
	char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total )
{
	Serial.println( "Publish received." );
	Serial.print( "  topic: " );
	Serial.println( topic );
	Serial.print( "  qos: " );
	Serial.println( properties.qos );
	Serial.print( "  dup: " );
	Serial.println( properties.dup );
	Serial.print( "  retain: " );
	Serial.println( properties.retain );
	Serial.print( "  len: " );
	Serial.println( len );
	Serial.print( "  index: " );
	Serial.println( index );
	Serial.print( "  total: " );
	Serial.println( total );

	// parseMessage();
}

void MqttClientForIR::sendInitValues()
{
	publishStatus();
	publishTemperature();
	publishLight();
	publishMode();
	publishState();
	publishFan();
	publishSwing();
	publishJet();
	publishSleep();
}

void MqttClientForIR::publishStatus()
{
	StaticJsonDocument< 1024 > doc;
	doc[ "temperature" ] = m_whirpoolData->getTemperature();
	doc[ "light" ]		 = m_whirpoolData->getLight();
	doc[ "mode" ]		 = tounderlying( m_whirpoolData->getMode() );
	doc[ "state" ]		 = m_whirpoolData->getPower();
	doc[ "fan" ]		 = tounderlying( m_whirpoolData->getFan() );
	doc[ "jet" ]		 = m_whirpoolData->getJet();
	doc[ "swing" ]		 = m_whirpoolData->getSwing();

	char buffer[ 1024 ];
	serializeJson( doc, buffer );

	m_mqttClient.publish( ( m_device + "/" ).c_str(), 0, true, buffer );
}

void MqttClientForIR::publishTemperature()
{
	m_mqttClient.publish(
		( m_device + "/temperature" ).c_str(), 0, true, to_string( m_whirpoolData->getTemperature() ).c_str() );
}

void MqttClientForIR::publishLight()
{
	m_mqttClient.publish( ( m_device + "/light" ).c_str(), 0, true, m_whirpoolData->getLightText().data() );
}

void MqttClientForIR::publishMode()
{
	m_mqttClient.publish( ( m_device + "/mode" ).c_str(), 0, true, m_whirpoolData->getModeText().data() );
}

void MqttClientForIR::publishState()
{
	m_mqttClient.publish( ( m_device + "/state" ).c_str(), 0, true, m_whirpoolData->getPowerText().data() );
}

void MqttClientForIR::publishFan()
{
	m_mqttClient.publish( ( m_device + "/fan" ).c_str(), 0, true, m_whirpoolData->getFanText().data() );
}

void MqttClientForIR::publishSwing()
{
	m_mqttClient.publish( ( m_device + "/swing" ).c_str(), 0, true, m_whirpoolData->getSwingText().data() );
}

void MqttClientForIR::publishJet()
{
	m_mqttClient.publish( ( m_device + "/jet" ).c_str(), 0, true, m_whirpoolData->getJetText().data() );
}

void MqttClientForIR::publishSleep() {}
