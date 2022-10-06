#include "WifiParameters.hpp"

WifiParameters::WifiParameters( WiFiManager* wifiManager, const ConfigDevice* defaultConfig )
	: m_wifiManager( wifiManager )
{
	const auto& mqtt   = defaultConfig->mqtt();
	const auto& host   = defaultConfig->host();
	const auto& syslog = defaultConfig->syslog();
	m_parameters.emplace( Parameters::MqttServer, WiFiManagerParameter{ "server", "MQTT server IP" } );
	m_parameters.emplace(
		Parameters::MqttPort,
		WiFiManagerParameter{ "port", "MQTT server port", mqtt.port.c_str(), int( mqtt.port.length() ) } );
	m_parameters.emplace(
		Parameters::HostName,
		WiFiManagerParameter{ "hostname", "Wifi host name", host.hostName.c_str(), int( host.hostName.length() ) } );
	m_parameters.emplace( Parameters::MqttUser, WiFiManagerParameter{ "mqtt_user", "MQTT user" } );
	m_parameters.emplace( Parameters::MqttPasword, WiFiManagerParameter{ "mqtt_password", "MQTT password" } );
	m_parameters.emplace(
		Parameters::MqttName,
		WiFiManagerParameter{ "mqtt_device", "MQTT device", mqtt.name.c_str(), int( mqtt.name.length() ) } );

	m_parameters.emplace(
		Parameters::SyslogServer,
		WiFiManagerParameter{
			"syslog_server", "Syslog server", syslog.server.c_str(), int( syslog.server.length() ) } );
	m_parameters.emplace(
		Parameters::SyslogPort,
		WiFiManagerParameter{ "mqtt_device", "Syslog port", syslog.port.c_str(), int( syslog.port.length() ) } );

	for( auto& [ _, parameter ] : m_parameters )
		m_wifiManager->addParameter( &parameter );
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
	host.hostName = m_parameters.at( Parameters::HostName ).getValue();
	mqtt.name	  = m_parameters.at( Parameters::MqttName ).getValue();
	mqtt.server	  = m_parameters.at( Parameters::MqttServer ).getValue();
	mqtt.port	  = m_parameters.at( Parameters::MqttPort ).getValue();
	mqtt.user	  = m_parameters.at( Parameters::MqttUser ).getValue();
	mqtt.password = m_parameters.at( Parameters::MqttPasword ).getValue();
	syslog.server = m_parameters.at( Parameters::SyslogServer ).getValue();
	syslog.port	  = m_parameters.at( Parameters::SyslogPort ).getValue();
	m_saveParamCallback( &configDevice );
}
