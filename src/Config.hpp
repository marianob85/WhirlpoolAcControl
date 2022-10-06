#pragma once
#include <string>
#include <IPAddress.h>

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

	struct ConfigSysLog
	{
		std::string server;
		std::string port;

		IPAddress getServer() const;
		uint16_t getPort() const;

		bool isSet() const;
	};

	const ConfigHost& host() const;
	const ConfigMqtt& mqtt() const;
	const ConfigSysLog& syslog() const;

	ConfigHost& host();
	ConfigMqtt& mqtt();
	ConfigSysLog& syslog();

private:
	ConfigHost m_host;
	ConfigMqtt m_mqtt;
	ConfigSysLog m_syslog;
};
