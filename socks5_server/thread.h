#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QStringList>
#include <QHostAddress>
#include <QHostInfo>
#include <QCryptographicHash>
#include "analysis.h"
#include "qaeswrap.h"

class Thread : public QThread
{
	Q_OBJECT
public:
	enum encode{NONE,XOR,AES};
	explicit Thread(int id ,encode code,QObject *parent = nullptr);
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
	int	   targetPort = 0;	//目标服务器端口
	int	   dataSum = 0;
	
	QTcpSocket *socket = NULL;
	QTcpSocket *target = NULL;
	
    
	QString targetIP;		//目标服务器ip

	
	QByteArray key = "1611";
	
	bool isReady = false;
	bool socketStop = false;
	bool targetStop = false;
	encode type;
private slots:
	bool approve();
	
	bool setTargetInfo(QString message);
	void writeToTarget();	//发送数据给目标
	void writeToSource();	//发送数据给源客户端
	void leave();

};

#endif // THREAD_H
