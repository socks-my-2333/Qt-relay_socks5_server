#include "thread.h"
#include <qDebug>
Thread::Thread(int id, QString ip, int port, QObject *parent) : QThread(parent)
{
	this->socketID = id;
	this->serverIP = ip;
	this->serverPort = port;
}

Thread::~Thread()
{
	qDebug()<<"~Thread()";
}

void Thread::run()
{
//	qDebug()<<"new Thread:"<<this->currentThreadId();
	this->target = new QTcpSocket();
	this->socket = new QTcpSocket();
	
	if(!this->socket->setSocketDescriptor(this->socketID))
		{
			qDebug()<<"thread.cpp 13  # "<<this->socket->errorString();
		}
	
	
	if(this->socket->waitForReadyRead())	//第一次交互
		{
			if(!fristMutula())
				{
					leave();
					return;
				}
		}
	
	if(this->socket->waitForReadyRead())
		{
			if(!secondMutula())
				{
					leave();
					return;
				}
		}
	
	connect(this->socket,&QTcpSocket::disconnected,this,&Thread::leave,Qt::DirectConnection);
	connect(this->target,&QTcpSocket::disconnected,this,&Thread::leave,Qt::DirectConnection);
	connect(this->socket,&QTcpSocket::readyRead,this,&Thread::writeToTarget,Qt::DirectConnection);	
	connect(this->target,&QTcpSocket::readyRead,this,&Thread::writeToSource,Qt::DirectConnection);
	if(this->style == 3)
		{
			connect(this->udpServer,&QUdpSocket::readyRead,this,&Thread::udpToTarget,Qt::DirectConnection);
			connect(this->udpClient,&QUdpSocket::readyRead,this,&Thread::udpToSource,Qt::DirectConnection);
		}
	qDebug()<<"start";
	this->exec();
}

void Thread::writeToSource()
{
//	qDebug()<<"write to source:";
	QByteArray buffer = this->target->readAll();
	
	emit sendSize(buffer.size());
	
	for(int i =0;i<buffer.size();i++)
		{
			buffer[i] = buffer[i]^ 1611;
		}
	
	this->socket->write(buffer);
}

void Thread::writeToTarget()
{
//	qDebug()<<"write to target:";
	QByteArray buffer = this->socket->readAll();
	
	for(int i =0;i<buffer.size();i++)
		{
			buffer[i] = buffer[i]^ 1611;
		}
	
	this->target->write(buffer);
}

void Thread::udpToSource()
{
	int readSize = udpClient->bytesAvailable()+1;
	char *recv = new char[readSize];
	QHostAddress address;
	quint16 port;
	udpClient->readDatagram(recv,readSize,&address,&port);
	
	
	QByteArray buff(recv,readSize);
//	qDebug()<<"udp recv:"<<buff;
	
	QByteArray tempIP = Analysis::ipToHex(this->targetIp);
	QByteArray tempPort = Analysis::portToHex(this->targetPort);
	
	QByteArray send;
	/*
	send.resize(10);
	send[0] = 0x00;
	send[1] = 0x00;
	send[2] = 0x00;
	send[3] = 0x01;
	send[4] = tempIP[0];
	send[5] = tempIP[1];
	send[6] = tempIP[2];
	send[7] = tempIP[3];
	send[8] = tempPort[0];
	send[9] = tempPort[1];
	*/
	send = this->token + buff;
	qDebug()<<"result "<<send;
	udpServer->writeDatagram(send,QHostAddress(this->ClientIP),(quint16)this->ClientudpPort);
	delete recv;
	recv = NULL;
}

void Thread::udpToTarget()
{
	int readSize = udpServer->bytesAvailable() + 1;
	qDebug()<<"UDP size "<<readSize;
	char *recv = new char[readSize];
		
	
	
	QHostAddress address;	
	quint16 port;
	udpServer->readDatagram(recv,readSize,&address,&port);  //用来接收客户端的数据，解析后由代理服务器发给目标
	
	QByteArray buff(recv,readSize);
	
	int allLength = Analysis::hexAllLength(buff);	//判断目标信息部分总长提取分离目标信息部分和数据部分
	this->targetPort =  Analysis::hexToPort(buff.mid(0,allLength+1));
	this->token = buff.mid(0,allLength);
	QByteArray tarIP;
	if(0x01 == buff[3])		//目标ip
		{
			tarIP =  Analysis::hexToIP(buff);
			this->targetIp = QString(tarIP);

			udpClient->writeDatagram(buff.mid(allLength+1,buff.size()),QHostAddress(QString(tarIP)),(quint16)targetPort);
		}
	else	if(0x03 == buff[3])		//目标域名
		{
			 tarIP = Analysis::hexToDomain(buff);
			 QHostInfo info = QHostInfo::fromName(tarIP);
			 this->targetIp = QString(info.addresses().first().toString());
			 udpClient->writeDatagram(buff.mid(allLength+1,buff.size()),info.addresses().first(),(quint16)targetPort);
		}
	else 
		{
			qDebug()<<"error # 143  udp";
			return;
		}

	
	delete recv;
	recv = NULL;
	
}

bool Thread::udpInit()
{
	this->ClientIP = this->targetIp;
	this->ClientudpPort = this->targetPort;
	
	QByteArray send;
	send.resize(8);
	send[0] = 0x05;
	send[1] = 0x00;
	send[2] = 0x00;
	send[3] = 0x01;
	send[4] = 0x00;
	send[5] = 0x00;
	send[6] = 0x00;
	send[7] = 0x00;
	send[8] = 0x00;
	send[9] = 0x00;
	
	udpServer = new QUdpSocket();
	udpServer->bind();
	
	udpClient = new QUdpSocket();
	udpClient->bind();
	
	this->serverPort = udpServer->localPort();
	
	send[8] = (serverPort / 256);
	send[9] = (serverPort % 256);
	
	qDebug()<<"local port: "<<serverPort;
	socket->write(send);
	return true;
}

bool Thread::connectToTarget(QString ip, int port,int pro)
{
	this->target->connectToHost(QHostAddress(this->serverIP),this->serverPort);		//连接到服务器
	if(this->target->waitForConnected())	//连接成功服务器
		{
			QString res;
			if(pro==0)		//如果是ip
				{
					res = QString("IPV4")+"#"+ip+"#"+QString::number(port);
				}	
			else				//如果是域名
				{
					res = QString("DOMAIN")+"#"+ip+"#"+QString::number(port);
				}
			
			this->target->write(res.toUtf8());
			qDebug()<<"\n\n\nconnect server::  "<<res;
			
			if(this->target->waitForReadyRead(10000))
				{
					QByteArray buff = this->target->readAll();
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
			qDebug()<<"\n connectToTarget # ERROR: "<<this->target->errorString();
			return false;
		}
	return false;
}

bool Thread::fristMutula()
{
	QByteArray buf =  this->socket->readAll();
	QByteArray send;
//	qDebug()<<" buf one : "<<buf;
	if( 0x05 == buf[0] )		//简单认证获取第一位是不是 0x05(VER)
		{
			send.resize(2);
			send[0] = 0x05;
			send[1] = 0x00;
			this->socket->write(send,2);
			return true;
		}
	return false;
}

bool Thread::secondMutula()
{
	QByteArray buf =  this->socket->readAll();
	QByteArray send;
	send.resize(10);
	send[0] = 0x05;
	send[1] = 0x00;
	send[2] = 0x00;
	send[3] = 0x01;
	send[4] = 0x00;
	send[5] = 0x00;
	send[6] = 0x00;
	send[7] = 0x00;
	send[8] = 0x00;
	send[9] = 0x00;
	
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
						this->post = IPV4_POST;	
						
						QByteArray ip = Analysis::hexToIP(buf);
						int port = Analysis::hexToPort(buf);
						
						this->targetIp = ip;
						this->targetPort = port;
						
						break;
					}
					
				case 0x03:		//domain
					{
						this->post = DOMAIN_POST;
						
						QByteArray domain = Analysis::hexToDomain(buf);
						int port = Analysis::hexToPort(buf);
						
						this->targetDomain = domain;
						this->targetPort = port;
						
						break;
					}
					
				default:	
					qDebug()<<"223# quit";
					return false;
				}
			
			//如果是udp
			if(this->style == 3)
				{
					if(udpInit())
						{
							return true;
						}
					return false;
				}
			this->socket->write(send,send.size());
		}
	else
		{
			return false;
		}
	
	if(this->post == 1)
		{
			if(!connectToTarget(this->targetIp,this->targetPort))
				{
					qDebug()<<"252# quit";
					return false;
				}
		}
	else if(this->post ==3)
		{
			if(!connectToTarget(this->targetDomain,this->targetPort,1))
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
				this->style = TCP_STYLE;
				break;
			}
		case 0x03:		//UDP
			{
				this->style = UDP_STYLE;
				break;
			}
		default:
			return false;
			break;
		}
	return true;
}


void Thread::leave()
{
	if(this->socket != NULL)
		{
			this->socket->disconnectFromHost();
			this->socket->close();
			this->socket->deleteLater();
		}
	if(this->target != NULL)
		{
			this->target->disconnectFromHost();
			this->target->close();
			this->target->deleteLater();
		}
	this->exit();
}
