#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QStringList>
#include <QHostAddress>
#include <QHostInfo>
#include "analysis.h"

class Thread : public QThread
{
	Q_OBJECT
public:
	explicit Thread(int id ,QObject *parent = nullptr);
	struct TargetInfo
	{
		QString ip;
		int	   port;
		bool	   empty=true;
	};
signals:
	
public slots:
	void run();
private:
	int socketID = 0;
	QTcpSocket *socket = NULL;
	QTcpSocket *target = NULL;
    bool isReady = false;
    
	QString targetIP;		//目标服务器ip
	int	   targetPort = 0;	//目标服务器端口
	int	   dataSum = 0;
private slots:
	bool approve();
	
	bool setTargetInfo(QString message);
	void writeToTarget();	//发送数据给目标
	void writeToSource();	//发送数据给源客户端
	void leave();

};

#endif // THREAD_H
