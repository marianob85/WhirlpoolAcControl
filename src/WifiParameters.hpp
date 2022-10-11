#pragma once
#include <map>
#include <WiFiManager.h>
#include "Config.hpp"

class WifiParameters
{
	enum class Parameters
	{
		MqttUser,
		MqttPasword,
		MqttServer,
		MqttPort,
		MqttName,
		HostName,
		SyslogServer,
		SyslogPort,
	};

public:
	WifiParameters( WiFiManager* wifiManager, const ConfigDevice* defaultConfig );
	void setSaveParamsCallback( std::function< void( const ConfigDevice* ) > callback );

public:
	void saveParamsCallback();

private:
	void addParam( Parameters param, const char* id, const char* label, const std::string& defaultValue );
	void addParam( Parameters param, const char* id, const char* labe );

private:
	std::function< void( const ConfigDevice* ) > m_saveParamCallback;
	WiFiManager* m_wifiManager{};
	std::map< Parameters, std::unique_ptr< WiFiManagerParameter > > m_parameters;
};