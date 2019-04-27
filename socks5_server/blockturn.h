#ifndef BLOCKTURN_H
#define BLOCKTURN_H

#include <QThread>
#include <QObject>
#include <QUdpSocket>
#include "analysis.h"
#include "udp.h"

class blockTurn : public QThread
{
	Q_OBJECT
public:
	explicit blockTurn(QString host,quint16 port,QObject *parent = nullptr);
	
	struct TargetInfo
	{
		QString ip;
		int	   port;
		bool	   empty=true;
	};
	
signals:
	void sendPort(int port,QString sourceip,quint16 sourcePort);		//回显信号
	
public slots:
	void run();
	
private:
	QUdpSocket *udp;
	QUdpSocket *tar;
	QString sourceIP;		//客户端的IP
	QString targetIP;		//目标ip
	
	quint16 targetPort;		//目标端口
	quint16 sourcePort;		//客户端的端口
	quint16 localPort = 0;		//我们绑定的本地端口
private slots:
	void toTarget();	//数据发送到目标
	void toSource();	//数据返回到源也就是客户端
	
	void Leave();		//清洁函数
};

#endif // BLOCKTURN_H
