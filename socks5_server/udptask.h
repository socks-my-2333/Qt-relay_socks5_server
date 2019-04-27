#ifndef UDPTASK_H
#define UDPTASK_H

#include <QObject>
#include <QThread>
#include <QUdpSocket>
#include "analysis.h"
#include "blockturn.h"
#include "udp.h"

#define UDP_READY "UDP_READY"
class UdpTask : public QObject
{
	Q_OBJECT
public:
	explicit UdpTask(QObject *parent = nullptr);
	struct TargetInfo
	{
		QString ip;
		int	   port;
		bool	   empty=true;
	};
signals:
	
public slots:
	void start();
private:
	QUdpSocket *udp = nullptr;

private slots:
	void assignation();		//根据连接的udp分配线程
	void resendPort(int port,QString sourceip,quint16 sourceport);		//回复连接的端口
};

#endif // UDPTASK_H
