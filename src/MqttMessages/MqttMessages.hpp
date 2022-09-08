#pragma once
#include <string>
#include <map>
#include <AsyncMqttClient.h>
#include <Ticker.h>
#include <Whirlpool_YJ1B.hpp>
class MqttClientForIR
{
public:
	MqttClientForIR( WhirlpoolYJ1B* whirpoolData );

	void setup( const char* server,
				uint16_t port,
				const char* user,
				const char* password,
				const char* device,
				std::function< void( WhirlpoolYJ1B* ) > commit );
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
	void parseMessage( char* topic,
					   char* payload,
					   AsyncMqttClientMessageProperties properties,
					   size_t len,
					   size_t index,
					   size_t total );
	void sendInitValues();
	void publishStatus();
	void publishTemperature();
	void publishLight();
	void publishMode();
	void publishState();
	void publishFan();
	void publishSwing();
	void publishJet();
	void publishSleep();
	void publishCommited();
	void proceedCommand( const char* command, const char* value );

	void onTemperature( const char* value );
	void onCommit();

private:
	AsyncMqttClient m_mqttClient{};
	Ticker m_mqttReconnectTimer{};
	std::string m_device;
	WhirlpoolYJ1B* m_whirpoolData{};
	std::function< void( WhirlpoolYJ1B* ) > m_commitEvent;
	std::map< std::string_view, std::function< void( const char* ) > > m_commands;
};