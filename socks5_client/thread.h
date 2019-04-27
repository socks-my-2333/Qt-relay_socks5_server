#ifndef THREAD_H
#define THREAD_H
#include <QHostInfo>
#include <QHostAddress>
#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QThread>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QTime>
#include <QUdpSocket>
#include "analysis.h"
#include "udp.h"
#include "factory.h"

#define TCP_STYLE 1
#define UDP_STYLE 3
#define IPV4_POST 1
#define DOMAIN_POST 3


class Thread : public QThread
{
	Q_OBJECT
public:
	explicit Thread(int id ,QString ip = "127.0.0.1",int port = 6666,QObject *parent = nullptr);
	~Thread();
	
private:
	int socketID = 0;
	
	//分别是和目标，源通信的socket;
	QTcpSocket *socket =nullptr;
	QTcpSocket *target = nullptr;
	
	QUdpSocket *udpServer = nullptr;
	QUdpSocket *udpClient = nullptr;
	
	QString serverIP = "";		//服务器ip
	QString targetIp;	//目标IP
	QString targetDomain;	//目标域名
	QString ClientIP;			//socks5客户IP
	
	int dataSum =0;		//记录接受的数据包
	int	   serverPort = 0;			//服务器端口
	int targetPort = 0;		//目标端口
	int ServerudpPort = 0;		//本地监听的端口				1080
	int ClientudpPort = 0;		//socks5客户端端口			连接的客户端口
	int post = 0;			//协议类型
	int style = 0;			//通信方式
	
	QHostAddress clientAddres;
	QByteArray token;		//记录udp转发的头
	QByteArray afterUse;
signals:
	void sendSize(int size);
private slots:	
	void writeToSource();		//将数据发送到源客户端
	void writeToTarget();		//同上反之，将源客户端数据发出
	void udpToSource();
	void udpToTarget();
	
	bool udpInit();
	
	bool connectToTarget(QString ip, int port,int pro = 0);		//连接到目标服务器
	bool fristMutula();
	bool secondMutula();
	bool checkPro(QByteArray buf);
	
public slots:
	void run();		//线程开始函数
	void leave();		//线程中断后的socket清理函数
};

#endif // THREAD_H
