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

static const std::map< string_view, bool > onoffMap{ { "on", true },	 { "off", false }, { "true", true },
													 { "false", false }, { "0", false },   { "1", true } };
static const std::map< string_view, Fan > fanMap{ { "Auto", Fan::Auto }, { "V1", Fan::V1 },	 { "V2", Fan::V2 },
												  { "V3", Fan::V3 },	 { "0", Fan::Auto }, { "1", Fan::V1 },
												  { "2", Fan::V2 },		 { "3", Fan::V3 } };

static const std::map< string_view, Mode > modeMap{
	{ "Sense_6th", Mode::Sense_6th },
	{ "Cool", Mode::Cool },
	{ "Dry", Mode::Dry },
	{ "Fan", Mode::Fan },
	{ "Heat", Mode::Heat },
	{ "0", Mode::Sense_6th },
	{ "1", Mode::Cool },
	{ "2", Mode::Dry },
	{ "3", Mode::Fan },
	{ "4", Mode::Heat },
};

template< typename T >
std::vector< T > split_string( T string, T delim )
{
	std::vector< T > result;
	size_t from = 0, to = 0;
	while( T::npos != ( to = string.find( delim, from ) ) )
	{
		result.push_back( string.substr( from, to - from ) );
		from = to + delim.length();
	}
	result.push_back( string.substr( from, to ) );
	return result;
}

MqttClientForIR::MqttClientForIR( WhirlpoolYJ1B* whirpoolData ) : m_device( mqttRoot ), m_whirpoolData( whirpoolData )
{
	m_commands.emplace( "temperature", std::bind( &MqttClientForIR::onTemperature, this, std::placeholders::_1 ) );
	m_commands.emplace( "light", std::bind( &MqttClientForIR::onLight, this, std::placeholders::_1 ) );
	m_commands.emplace( "mode", std::bind( &MqttClientForIR::onMode, this, std::placeholders::_1 ) );
	m_commands.emplace( "state", std::bind( &MqttClientForIR::onState, this, std::placeholders::_1 ) );
	m_commands.emplace( "fan", std::bind( &MqttClientForIR::onFan, this, std::placeholders::_1 ) );
	m_commands.emplace( "swing", std::bind( &MqttClientForIR::onSwing, this, std::placeholders::_1 ) );
	m_commands.emplace( "jet", std::bind( &MqttClientForIR::onJet, this, std::placeholders::_1 ) );
	m_commands.emplace( "sleep", std::bind( &MqttClientForIR::onSleep, this, std::placeholders::_1 ) );
	m_commands.emplace( "commit", std::bind( &MqttClientForIR::onCommit, this ) );
}

void MqttClientForIR::setup( const char* server,
							 uint16_t port,
							 const char* user,
							 const char* password,
							 const char* device,
							 std::function< void( WhirlpoolYJ1B* ) > commit )
{
	m_commitEvent = commit;
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

	string payload_s{ payload, payload + len };
	parseMessage( topic, payload_s );
}

void MqttClientForIR::parseMessage( char* topic, string_view payload )
{
	const string topic_s{ topic };
	const auto device = m_device + "/cmd/";
	if( topic_s.find( device, 0 ) == std::string::npos )
		return;

	const auto commands = split_string< std::string >( topic_s.substr( device.length() ), "/" );

	switch( commands.size() )
	{
	case 1:
		proceedCommand( commands.at( 0 ).c_str(), payload );
		break;
	case 2:
		proceedCommand( commands.at( 0 ).c_str(), commands.at( 1 ) );
		break;
	default:
		break;
	};
}

void MqttClientForIR::proceedCommand( string_view command, string_view payload )
{
	string value_s{ payload };
	transform( value_s.begin(), value_s.end(), value_s.begin(), ::tolower );
	if( const auto it = m_commands.find( command ); it != m_commands.end() )
		it->second( value_s );
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
	m_mqttClient.publish( ( m_device + "/" ).c_str(), 0, true, m_whirpoolData->get().c_str() );
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

void MqttClientForIR::publishSleep()
{
	m_mqttClient.publish( ( m_device + "/sleep" ).c_str(), 0, true, m_whirpoolData->getSleepText().data() );
}

void MqttClientForIR::publishCommited()
{
	m_mqttClient.publish( ( m_device + "/commited" ).c_str(), 0, false, m_whirpoolData->get().c_str() );
}

void MqttClientForIR::onTemperature( std::string_view value )
{
	if( value.empty() )
		return;
	try
	{

		if( "up"s.compare( value ) == 0 )
			m_whirpoolData->setTemperature( m_whirpoolData->getTemperature() + 1 );
		else if( "down"s.compare( value ) == 0 )
			m_whirpoolData->setTemperature( m_whirpoolData->getTemperature() - 1 );
		else
			m_whirpoolData->setTemperature( stoi( value.data() ) );
		publishTemperature();
		publishStatus();
	}
	catch( const std::invalid_argument& )
	{
	}
}

void MqttClientForIR::onLight( std::string_view value )
{
	if( const auto it = onoffMap.find( value ); it != onoffMap.end() )
	{
		m_whirpoolData->setLight( it->second );
		publishLight();
		publishStatus();
	}
}

void MqttClientForIR::onMode( std::string_view value )
{
	if( const auto it = modeMap.find( value ); it != modeMap.end() )
	{
		m_whirpoolData->setMode( it->second );
		publishMode();
		publishStatus();
	}
}

void MqttClientForIR::onState( std::string_view value )
{
	if( const auto it = onoffMap.find( value ); it != onoffMap.end() )
	{
		m_whirpoolData->setPower( it->second );
		publishState();
		publishStatus();
	}
}

void MqttClientForIR::onFan( std::string_view value )
{
	if( const auto it = fanMap.find( value ); it != fanMap.end() )
	{
		m_whirpoolData->setFan( it->second );
		publishSwing();
		publishStatus();
	}
}

void MqttClientForIR::onSwing( std::string_view value )
{
	if( const auto it = onoffMap.find( value ); it != onoffMap.end() )
	{
		m_whirpoolData->setSwing( it->second );
		publishSwing();
		publishStatus();
	}
}

void MqttClientForIR::onJet( std::string_view value )
{
	if( const auto it = onoffMap.find( value ); it != onoffMap.end() )
	{
		m_whirpoolData->setJet( it->second );
		publishJet();
		publishStatus();
	}
}

void MqttClientForIR::onSleep( std::string_view value )
{
	if( const auto it = onoffMap.find( value ); it != onoffMap.end() )
	{
		m_whirpoolData->setSleep( it->second );
		publishSleep();
		publishStatus();
	}
}
void MqttClientForIR::onCommit()
{
	m_commitEvent( m_whirpoolData );
	publishCommited();
}