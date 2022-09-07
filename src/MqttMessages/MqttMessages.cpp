#include "MqttMessages.hpp"
#include <ESP8266WiFi.h>
using namespace std::literals;

MqttClientForIR::MqttClientForIR( WhirlpoolYJ1B* whirpoolData ) : m_whirpoolData( whirpoolData ) {}

void MqttClientForIR::setup(
	const char* server, uint16_t port, const char* user, const char* password, const char* device )
{
	m_device = "IR/"s + device;
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

	m_mqttClient.subscribe( ( m_device + "/#" ).c_str(), 2 );
	//m_mqttClient.publish( ( m_device + "/status" ).c_str(), 0, true, "test 1" );
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
	//parsowanie topic'a '
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
}
