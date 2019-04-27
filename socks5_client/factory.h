#ifndef FACTORY_H
#define FACTORY_H

#include <QTcpSocket>
#include <QUdpSocket>
	
class Factory
{
public:
//	Factory();
	enum STYLE{TCP,UDP};
	static QTcpSocket* createTcpSocket();
	static QUdpSocket* createUdpSocket();
};

#endif // FACTORY_H
