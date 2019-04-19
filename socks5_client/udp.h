#ifndef UDP_H
#define UDP_H

#include <QObject>
#include <QUdpSocket>

class Udp : public QObject
{
	Q_OBJECT
public:
	explicit Udp(QObject *parent = nullptr);
public slots:
	static quint64 write(QUdpSocket* socket,const QByteArray &datagram, const QHostAddress &host, quint16 port);
	static QByteArray read(QUdpSocket* socket, QHostAddress *address,quint16 *port);
private:
	
};

#endif // UDP_H
