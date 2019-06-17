#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QHostAddress>
#include <QHostInfo>
#include <QTimer>
#include "thread.h"

class Server : public QTcpServer
{
	Q_OBJECT
public:
	explicit Server(QObject *parent = nullptr);

private:
	QHostAddress ip;		
	int port;
	
	int recSize = 0;			//数据累加
	int lastSize = 0;
	
	QString server_ip = "";		//服务器ip
	int server_port = 0;		//服务器端口
	
	QTimer *timer;
	
	ENCODE_TYPE type;
signals:
	void sendSize(int id );
	void sendSpeed(int id);
public slots:
	void incomingConnection(int id);		//重载分配socket的槽函数
	bool startServer(QString address, int port);		//开始服务
	void stopServer();		//停止服务
	void getSize(int size);
	void setInfo(QString ip, int port,ENCODE_TYPE type);
	void getSpeed();
};

#endif // SERVER_H
