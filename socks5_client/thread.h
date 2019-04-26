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
	QTcpSocket *socket =NULL;
	QTcpSocket *target = NULL;
	QUdpSocket *udpServer = NULL;
	QUdpSocket *udpClient = NULL;
	
	QString targetIp;	//目标IP
	QString targetDomain;	//目标域名
	int targetPort = 0;		//目标端口
	
	int ServerudpPort = 0;		//本地监听的端口
	int ClientudpPort = 0;		//socks5客户端端口
	QString ClientIP;			//socks5客户IP
	QHostAddress clientAddres;
	
	int post = 0;			//协议类型
	int style = 0;			//通信方式
	
	bool isUdp = false;		//udp是否开始转发
	
	QString serverIP = "";		//服务器ip
	int	   serverPort = 0;			//服务器端口
	
	int dataSum =0;		//记录接受的数据包
	
	QByteArray token;
	
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
