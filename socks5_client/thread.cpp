#include "thread.h"
#include <QDebug>
Thread::Thread(int id, ENCODE_TYPE type, QString ip, int port, QObject *parent) : QThread(parent)
{
	socketID = id;
	serverIP = ip;
	serverPort = port;
	
	afterUse.resize(10);
	afterUse[0] = 0x05;
	afterUse[1] = 0x00;
	afterUse[2] = 0x00;
	afterUse[3] = 0x01;
	afterUse[4] = 0x00;
	afterUse[5] = 0x00;
	afterUse[6] = 0x00;
	afterUse[7] = 0x00;
	afterUse[8] = 0x00;
	afterUse[9] = 0x00;
	this->encode_type = type;
	
}

Thread::~Thread()
{
	qDebug()<<"~Thread()";
}

void Thread::run()
{
	
	key = QCryptographicHash::hash(key,QCryptographicHash::Md5);
//	qDebug()<<"key "<<key;
	
	target = new QTcpSocket();	//用于跟中继服务器通信
	
	socket = new QTcpSocket();	//用于跟客户连接(socks5)
	
	if(!socket->setSocketDescriptor(socketID))
		{
			qDebug()<<"thread.cpp 13  # "<<socket->errorString();
		}
	
	
	if(socket->waitForReadyRead())	//第一次交互		socks5
		{
			if(!fristMutula())
				{
					leave();
					return;
				}
		}
	
	if(socket->waitForReadyRead())
		{
			if(!secondMutula())
				{
					leave();
					return;
				}
		}
	//认证通过
	connect(socket,&QTcpSocket::disconnected,this,&Thread::leave,Qt::DirectConnection);
	connect(target,&QTcpSocket::disconnected,this,&Thread::leave,Qt::DirectConnection);
	
	connect(socket,&QTcpSocket::readyRead,this,&Thread::writeToTarget,Qt::DirectConnection);	
	connect(target,&QTcpSocket::readyRead,this,&Thread::writeToSource,Qt::DirectConnection);
	if(style == UDP_STYLE)
		{
			connect(udpServer,&QUdpSocket::readyRead,this,&Thread::udpToTarget,Qt::DirectConnection);
			connect(udpClient,&QUdpSocket::readyRead,this,&Thread::udpToSource,Qt::DirectConnection);
		}
	qDebug()<<"start";

	exec();
}

void Thread::writeToSource()
{
	qDebug()<<"to writeToSource";
	if(!checkConnection())
		{
			qDebug()<<"to writeToSource return; ";
			return;
		}
		
	
	QByteArray buffer;
	buffer.clear();
	
	char *data = new char[65535];
	while(true)
		{
			int readSize = target->read(data,65535);
			if(readSize < 1)
				{
					break;
				}
			QByteArray temp(data,readSize);
			buffer += temp;
			memset(data,0,65535);
		}
	delete[] data;
	
	
	
	emit sendSize(buffer.size());
	switch (this->encode_type) {
		case ENCODE_TYPE::NONE:
			{
				break;
			}
		case ENCODE_TYPE::XOR:
			{
				
				for(int i =0;i<buffer.size();i++)
					{
						buffer[i] = buffer[i] ^ 1611;
					}
				
				break;
			}
		case ENCODE_TYPE::AES:
			{
				QAesWrap aes(key,key,QAesWrap::AesBit::AES_128);
				QByteArray rec = aes.encrypt(buffer,QAesWrap::AesMode::AES_ECB);
				buffer = rec;
				break;
			}
		}
	
	socket->write(buffer);
}

void Thread::writeToTarget()
{
	
	qDebug()<<"to writeToTarget";
	if(!checkConnection())
		{
			qDebug()<<"to writeToTarget return; ";
			return;
		}
	QByteArray buffer;
	buffer.clear();
	
	char *data = new char[65535];
	while(true)
		{
			int readSize = socket->read(data,65535);
			if(readSize < 1)
				{
					break;
				}
			QByteArray temp(data,readSize);
			buffer += temp;
			memset(data,0,65535);
		}
	delete[] data;

	QTime time;
	time.start();
	
	
	switch (this->encode_type) {
		case ENCODE_TYPE::NONE:
			{
				break;
			}
		case ENCODE_TYPE::XOR:
			{
				
				for(int i =0;i<buffer.size();i++)
					{
						buffer[i] = buffer[i] ^ 1611;
					}
				
				break;
			}
		case ENCODE_TYPE::AES:
			{
				QAesWrap aes(key,key,QAesWrap::AesBit::AES_128);
				QByteArray rec = aes.encrypt(buffer,QAesWrap::AesMode::AES_ECB);
				buffer = rec;
				break;
			}
		}
	
	

	qDebug()<<"加密用时 "<<time.elapsed();
	target->write(buffer);
	
	
}

void Thread::udpToSource()
{
	
	
	QHostAddress address;		//新建栈变量用于接受信息
	quint16 port;	
	
	QByteArray buff = Udp::read(udpClient,&address,&port);
	qDebug()<<"udp buff "<<buff;
	QByteArray send;
	send = token + buff;

	Udp::write(udpServer,send,QHostAddress("127.0.0.1"),static_cast<quint16>(ClientudpPort));
	
}

void Thread::udpToTarget()
{
	QHostAddress address;	
	quint16 port;

	QByteArray buff = Udp::read(udpServer,&address,&port);	     //用来接收客户端的数据，解析后由代理服务器发给目标
	
	ClientudpPort = port;
	ClientIP = address.toString();
	clientAddres =address;
	
	int allLength = Analysis::hexAllLength(buff);	//判断目标信息部分总长提取分离目标信息部分和数据部分
	targetPort =  Analysis::hexToPort(buff.mid(0,allLength+1));
	token = buff.mid(0,allLength+1);
	QByteArray tarIP;
	
	if(0x01 == buff[3])		//目标ip
		{
			
			tarIP =  Analysis::hexToIP(buff);
			targetIp = QString(tarIP);
			
		}
	else	if(0x03 == buff[3])		//目标域名
		{
			
			 tarIP = Analysis::hexToDomain(buff);
			 targetIp = Analysis::domainToIp(tarIP);
			 
		}
	else 
		{
			qDebug()<<"error # 133  udp";
			return;
		}
	QByteArray send(buff.mid(allLength+1,buff.size()-allLength));
	
	Udp::write(udpClient,send,QHostAddress(QString(tarIP)),static_cast<quint16>(targetPort));
}

bool Thread::udpInit()
{
	QByteArray send = afterUse;
	
	udpServer = new QUdpSocket();
	udpServer->bind();
	
	udpClient = new QUdpSocket();
	udpClient->bind();
	
	serverPort = udpServer->localPort();
	
	send[8] = (serverPort / 256);
	send[9] = (serverPort % 256);
	
	qDebug()<<"local port: "<<serverPort;
	socket->write(send);
	return true;
}

bool Thread::connectToTarget(QString ip, int port,int pro)
{
	if(ip == "clients1.google.com")
		{
			return false;
		}
	target->connectToHost(QHostAddress(serverIP),static_cast<quint16>(serverPort));		//连接到服务器
	if(target->waitForConnected())	//连接成功服务器
		{
			QString res;
			if(pro==0)		//如果是ip
				{
					res = QString("IPV4")+"#"+ip+"#"+QString::number(port);		//发送给我们的中继服务器
				}	
			else				//如果是域名
				{
					res = QString("DOMAIN")+"#"+ip+"#"+QString::number(port);
				}
			
			target->write(res.toUtf8());
			qDebug()<<"\n\n\nconnect server::  "<<res;
			
			if(target->waitForReadyRead(10000))
				{
					QByteArray buff = target->readAll();
					if(buff.indexOf("#FINSH#") != -1)
						{
							qDebug()<<"#FINSH#";
							return true;
						}
					qDebug()<<"回复呢但是没有 \"连接成功\" 的信息 ";
				}
			qDebug()<<"连接成功但是没有回复";
			return false;
		}
	else
		{
			qDebug()<<"\n connectToTarget # ERROR: "<<target->errorString();
			return false;
		}
}

bool Thread::fristMutula()
{
	QByteArray buf =  socket->readAll();
	QByteArray send;
//	qDebug()<<" buf one : "<<buf;
	if( 0x05 == buf[0] )		//简单认证获取第一位是不是 0x05(VER)
		{
			send.resize(2);
			send[0] = 0x05;
			send[1] = 0x00;
			socket->write(send,2);
			return true;
		}
	return false;
}

bool Thread::secondMutula()
{
	QByteArray buf =  socket->readAll();
	QByteArray send = afterUse;
	
//	qDebug()<<" buf two : "<<buf;
	Thread::msleep(5);
	if(0x05 == buf[0])
		{
			if (!checkPro(buf))	//检查协议是否支持
				{
					return false;
				}
			
			switch (buf[3])		//不支持ipv6
				{
				
				case 0x01:		//ipv4
					{
						post = IPV4_POST;	
						
						QByteArray ip = Analysis::hexToIP(buf);
						int port = Analysis::hexToPort(buf);
						
						targetIp = ip;
						targetPort = port;
						
						break;
					}
					
				case 0x03:		//domain
					{
						post = DOMAIN_POST;
						
						QByteArray domain = Analysis::hexToDomain(buf);
						int port = Analysis::hexToPort(buf);
						
						targetDomain = domain;
						targetPort = port;
						
						break;
					}
					
				default:	
					qDebug()<<"223# quit";
					return false;
				}
			
			//如果是udp
			if(style == UDP_STYLE)
				{
				//	return false;
					if(udpInit())
						{
							return true;
						}
					return false;
				}
			socket->write(send,send.size());
		}
	else
		{
			return false;
		}
	
	if(post == IPV4_POST)
		{
			if(!connectToTarget(targetIp,targetPort))
				{
					qDebug()<<"252# quit";
					return false;
				}
		}
	else if(post == DOMAIN_POST)
		{
			if(!connectToTarget(targetDomain,targetPort,1))
				{
					qDebug()<<"252# quit";
					return false;
				}
		}
	else
		{
			return false;
		}
	
	
	return true;
}

bool Thread::checkPro(QByteArray buf)
{
	switch (buf[1]) {
		case 0x01:		//TCP
			{
				style = TCP_STYLE;
				break;
			}
		case 0x03:		//UDP
			{
				style = UDP_STYLE;
				break;
			}
		default:
			return false;
		}
	return true;
}

bool Thread::checkConnection()
{
	if(socket != nullptr && target != nullptr)
		{
			return true;
		}
	return false;
}


void Thread::leave()
{
	if(udpServer != nullptr)
		{
			udpServer->disconnectFromHost();
			udpServer->close();
			udpServer->deleteLater();
		}
	if(udpClient != nullptr)
		{
			udpClient->disconnectFromHost();
			udpClient->close();
			udpClient->deleteLater();
		}
	if(socketStop == false )
		{
			socket->disconnectFromHost();
			socket->close();
			socket->deleteLater();
			socketStop = true;
		}
	if(targetStop == false)
		{
			target->disconnectFromHost();
			target->close();
			target->deleteLater();
			targetStop = true;
		}
	exit();
}

