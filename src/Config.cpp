#include "Config.hpp"

const ConfigDevice::ConfigHost& ConfigDevice::host() const
{
	return m_host;
}

const ConfigDevice::ConfigMqtt& ConfigDevice::mqtt() const
{
	return m_mqtt;
}

ConfigDevice::ConfigHost& ConfigDevice::host()
{
	return m_host;
}

ConfigDevice::ConfigMqtt& ConfigDevice::mqtt()
{
	return m_mqtt;
}

const ConfigDevice::ConfigSysLog& ConfigDevice::syslog() const
{
	return m_syslog;
}

ConfigDevice::ConfigSysLog& ConfigDevice::syslog()
{
	return m_syslog;
}

IPAddress ConfigDevice::ConfigSysLog::getServer() const
{
	IPAddress address;
	address.fromString( server.c_str() );
	return address;
}

uint16_t ConfigDevice::ConfigSysLog::getPort() const
{
	return std::stoi( port );
}
