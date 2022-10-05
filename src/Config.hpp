#pragma once
#include <string>

class ConfigDevice
{
public:
	struct ConfigHost
	{
		std::string hostName{ "IR_MQTT" };
	};

	struct ConfigMqtt
	{
		std::string server;
		std::string port{ "1883" };
		std::string password;
		std::string user;
		std::string name{ "default" };
	};

	const ConfigHost& host() const;
	const ConfigMqtt& mqtt() const;

	ConfigHost& host();
	ConfigMqtt& mqtt();

private:
	ConfigHost m_host;
	ConfigMqtt m_mqtt;
};
