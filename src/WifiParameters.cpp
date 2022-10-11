#include "WifiParameters.hpp"

using namespace std;

WifiParameters::WifiParameters( WiFiManager* wifiManager, const ConfigDevice* defaultConfig )
	: m_wifiManager( wifiManager )
{
	const auto& mqtt = defaultConfig->mqtt();
	const auto& host = defaultConfig->host();

	addParam( Parameters::HostName, "hostname", "Wifi host name", host.hostName, 20 );
	addParam( Parameters::MqttServer, "mqtt_server", "MQTT server IP", 40 );
	addParam( Parameters::MqttPort, "mqtt_port", "MQTT server port", mqtt.port, 5 );
	addParam( Parameters::MqttUser, "mqtt_user", "MQTT user", 20 );
	addParam( Parameters::MqttPasword, "mqtt_password", "MQTT password", 20 );
	addParam( Parameters::MqttName, "mqtt_device", "MQTT device", mqtt.name, 20 );
	addParam( Parameters::SyslogServer, "syslog_server", "Syslog server IP", 15 );
	addParam( Parameters::SyslogPort, "syslog_port", "Syslog port", 5 );

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

void WifiParameters::addParam(
	Parameters param, const char* id, const char* label, const string& defaultValue, int length )
{
	m_parameters.emplace( param, make_unique< WiFiManagerParameter >( id, label, defaultValue.c_str(), length ) );
}

void WifiParameters::addParam( Parameters param, const char* id, const char* label, int length )
{
	m_parameters.emplace( param, make_unique< WiFiManagerParameter >( id, label, "", length ) );
}
