#include "udp.h"
#include <QDebug>

Udp::Udp(QObject *parent) : QObject(parent)
{
	
}

quint64 Udp::write(QUdpSocket *socket, const QByteArray &datagram, const QHostAddress &host, quint16 port)
{
	quint64 recvSize = socket->writeDatagram(datagram,host,port);
	return recvSize;
}

QByteArray Udp::read(QUdpSocket *socket, QHostAddress *address, quint16 *port)
{
	if(address->toString().isEmpty())
		{
			address->setAddress("127.0.0.1");
		}
	
	int size = socket->bytesAvailable();
	char *buf = new char[size];
	
	socket->readDatagram(buf,size,address,port);
	QByteArray retn(buf,size);
	delete[] buf;
	buf = NULL;
	return retn;
}
