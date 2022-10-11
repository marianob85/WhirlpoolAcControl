#include "WifiParameters.hpp"

using namespace std;

WifiParameters::WifiParameters( WiFiManager* wifiManager, const ConfigDevice* defaultConfig )
	: m_wifiManager( wifiManager )
{
	const auto& mqtt   = defaultConfig->mqtt();
	const auto& host   = defaultConfig->host();
	const auto& syslog = defaultConfig->syslog();

	addParam( Parameters::MqttServer, "server", "MQTT server IP" );
	addParam( Parameters::MqttPort, "port", "MQTT server port", mqtt.port );
	addParam( Parameters::HostName, "hostname", "Wifi host name", host.hostName );
	addParam( Parameters::MqttUser, "mqtt_user", "MQTT user" );
	addParam( Parameters::MqttPasword, "mqtt_password", "MQTT password" );
	addParam( Parameters::MqttName, "mqtt_device", "MQTT device", mqtt.name );
	addParam( Parameters::SyslogServer, "syslog_server", "Syslog server" );
	addParam( Parameters::SyslogPort, "mqtt_device", "Syslog port", syslog.port );

	for( auto& [ _, parameter ] : m_parameters )
		m_wifiManager->addParameter( parameter.get() );
}

void WifiParameters::setSaveParamsCallback( std::function< void( const ConfigDevice* ) > callback )
{
	m_saveParamCallback = callback;
	m_wifiManager->setSaveParamsCallback( std::bind( &WifiParameters::saveParamsCallback, this ) );
}

void WifiParameters::saveParamsCallback()
{
	ConfigDevice configDevice;
	auto& host	  = configDevice.host();
	auto& mqtt	  = configDevice.mqtt();
	auto& syslog  = configDevice.syslog();
	host.hostName = m_parameters.at( Parameters::HostName )->getValue();
	mqtt.name	  = m_parameters.at( Parameters::MqttName )->getValue();
	mqtt.server	  = m_parameters.at( Parameters::MqttServer )->getValue();
	mqtt.port	  = m_parameters.at( Parameters::MqttPort )->getValue();
	mqtt.user	  = m_parameters.at( Parameters::MqttUser )->getValue();
	mqtt.password = m_parameters.at( Parameters::MqttPasword )->getValue();
	syslog.server = m_parameters.at( Parameters::SyslogServer )->getValue();
	syslog.port	  = m_parameters.at( Parameters::SyslogPort )->getValue();
	m_saveParamCallback( &configDevice );
}

void WifiParameters::addParam( Parameters param, const char* id, const char* label, const string& defaultValue )
{
	m_parameters.emplace(
		param, make_unique< WiFiManagerParameter >( id, label, defaultValue.c_str(), int( defaultValue.length() ) ) );
}

void WifiParameters::addParam( Parameters param, const char* id, const char* label )
{
	m_parameters.emplace( param, make_unique< WiFiManagerParameter >( id, label ) );
}
