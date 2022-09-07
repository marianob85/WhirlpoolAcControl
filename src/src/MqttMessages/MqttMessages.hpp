#pragma once
#include <string>
#include <AsyncMqttClient.h>
#include <Ticker.h>

class MqttClientForIR
{
public:
	MqttClientForIR() = default;

	void setup( const char* server, uint16_t port, const char* user, const char* password, const char* device );
	void detach();

	void connect();

	void onDisconnect( AsyncMqttClientDisconnectReason reason );
	void onConnect( bool sessionPresent );
	void onPublish( uint16_t packetId );
	void onSubscribe( uint16_t packetId, uint8_t qos );
	void onUnsubscribe( uint16_t packetId );
	void onMessage( char* topic,
					char* payload,
					AsyncMqttClientMessageProperties properties,
					size_t len,
					size_t index,
					size_t total );

private:
	AsyncMqttClient m_mqttClient{};
	Ticker m_mqttReconnectTimer{};
	std::string m_device;
};