#include "udp.h"

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
	char buf[1024] = {0};
	socket->readDatagram(buf,20,address,port);
	return QByteArray(buf,20);
}
