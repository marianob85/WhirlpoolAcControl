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
