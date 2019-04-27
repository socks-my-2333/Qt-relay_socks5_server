#include "factory.h"
/*
Factory::Factory()
{
	
}
*/
QTcpSocket *Factory::createTcpSocket()
{
	return new QTcpSocket();
}

QUdpSocket *Factory::createUdpSocket()
{
	return new QUdpSocket();
}

