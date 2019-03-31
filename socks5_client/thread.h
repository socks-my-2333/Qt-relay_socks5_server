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
	
	QString sourceIp;	//客户端ip
	QString targetIp;	//目标IP
	QString targetDomain;	//目标域名
	int targetPort = 0;		//目标端口
	int udpPort = 0;		//本地监听的端口
	
	int post = 0;			//地址类型
	int style = 0;
	
	bool isUdp = false;		//udp是否开始转发
	
	QString serverIP = "";		//服务器ip
	int	   serverPort = 0;			//服务器端口
	
	int dataSum =0;		//记录接受的数据包
	
	QByteArray temp2Target;
	QByteArray temp2Source;
	
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
	
	void getDomain(QByteArray buf);
	void getIP(QByteArray buf);
	
	QString hexToIP(QByteArray s ,QByteArray w, QByteArray l ,QByteArray q);	//将hexIP转stringip
	int	   hexToPort(QByteArray buf);
	QByteArray ipToHex(QString ip);
public slots:
	void run();		//线程开始函数
	void leave();		//线程中断后的socket清理函数
};

#endif // THREAD_H
