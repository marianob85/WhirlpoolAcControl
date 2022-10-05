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
		HostName
	};

public:
	WifiParameters( WiFiManager* wifiManager, const ConfigDevice* defaultConfig );

	void setSaveParamsCallback( std::function< void( const ConfigDevice* ) > callback );

public:
	void saveParamsCallback();

private:
	std::function< void( const ConfigDevice* ) > m_saveParamCallback;
	WiFiManager* m_wifiManager{};
	std::map< Parameters, WiFiManagerParameter > m_parameters;
};