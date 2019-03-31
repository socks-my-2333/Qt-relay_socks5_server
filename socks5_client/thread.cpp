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
	qDebug()<<"new Thread:"<<this->currentThreadId();
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
	connect(this->socket,&QTcpSocket::readyRead,this,&Thread::writeToTarget,Qt::DirectConnection);	//交给分发函数
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
	qDebug()<<"write to source:";
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
	qDebug()<<"write to target:";
	QByteArray buffer = this->socket->readAll();
	
	for(int i =0;i<buffer.size();i++)
		{
			buffer[i] = buffer[i]^ 1611;
		}
	
	this->target->write(buffer);
}

void Thread::udpToSource()
{
	QByteArray buff;
	QHostAddress address;
	quint16 port;
	udpClient->readDatagram(buff.data(),buff.size(),&address,&port);
	
	QByteArray temp = ipToHex(address.toString());
	
	QByteArray send;
	send.resize(10);
	send[0] = 0x00;
	send[1] = 0x00;
	send[2] = 0x00;
	send[3] = 0x01;
	send[4] = temp[0];
	send[5] = temp[1];
	send[6] = temp[2];
	send[7] = temp[3];
	
	udpServer->writeDatagram(send+buff,this->socket->peerAddress(),this->targetPort);
}

void Thread::udpToTarget()
{
	QByteArray buff;
	QHostAddress address;
	quint16 port;
	udpClient->readDatagram(buff.data(),buff.size(),&address,&port);
	
	QByteArray temp = ipToHex(address.toString());
}

bool Thread::udpInit()
{
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
	
	udpServer = new QUdpSocket();
	udpServer->bind();
	
	udpClient = new QUdpSocket();
	udpClient->bind();
	
	int localPort = udpServer->localPort();
	QByteArray bytePort = QString::number(localPort).toUtf8().toHex();
	
	if(localPort>255)
	{
		send[8] = bytePort[0];
		send[9] = bytePort[1];
	}
	else
	{
		send[9] = bytePort[0];
	}
	
	this->udpPort = localPort;
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
					qDebug()<<"没有接受到 \"连接成功\" 的信息";
				}
			qDebug()<<"代理服务器没有回复";
			return false;
		}
	else
		{
			qDebug()<<"263# ERROR:"<<this->target->errorString();
			return false;
		}
	return false;
}

bool Thread::fristMutula()
{
	QByteArray buf =  this->socket->readAll();
	QByteArray send;
	qDebug()<<" buf one : "<<buf;
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
	
	qDebug()<<" buf two : "<<buf;
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
						this->post = 1;	
						getIP(buf);
						break;
					}
					
				case 0x03:		//domain
					{
						this->post = 3;
						getDomain(buf);
						break;
					}
					
				default:	
					qDebug()<<"223# quit";
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
	return true;
}

bool Thread::checkPro(QByteArray buf)
{
	switch (buf[1]) {
		case 0x01:		//TCP
			{
				this->style = 1;
				break;
			}
		case 0x03:		//UDP
			{
				this->style = 3;
				break;
			}
		default:
			return false;
			break;
		}
	return true;
}

void Thread::getDomain(QByteArray buf)
{
	QByteArray buff;
	buff.resize(1);
	buff[0] = buf[4];
	QString str(buff.toHex());
	
	bool ok;
	int domainLength=str.toInt(&ok,16);
	QByteArray domain(buf.mid(5,domainLength));	//获取域名
	QByteArray temp(buf.mid(4+domainLength+1,2));
	int port = hexToPort(temp);
	
	this->targetPort = port;
	this->targetDomain = domain;
}

void Thread::getIP(QByteArray buf)
{
	QByteArray s,w,l,q;
	s[0] = buf[4];
	w[0] = buf[5];
	l[0] = buf[6];
	q[0] = buf[7];
	
	QString ip = hexToIP(s,w,l,q); //还原成IPV4地址格式
	
	QByteArray port = buf.mid(8,2);
	
	this->targetIp = ip;
	this->targetPort = hexToPort(port);
	qDebug()<<"268#  ip:"<<this->targetIp;
	qDebug()<<"269#  port:"<<this->targetPort;
}

QString Thread::hexToIP(QByteArray s, QByteArray w, QByteArray l, QByteArray q)
{
	bool ok;	//还原成IPV4地址格式
	QString ip = QString::number(QString(s.toHex()).toInt(&ok,16)) +"."+
			QString::number(QString(w.toHex()).toInt(&ok,16)) +"."+
			QString::number(QString(l.toHex()).toInt(&ok,16)) +"."+
			QString::number(QString(q.toHex()).toInt(&ok,16));
	return ip;
}

int Thread::hexToPort(QByteArray buf)
{
	bool ok;
	return QString(buf.toHex()).toInt(&ok,16);
}

QByteArray Thread::ipToHex(QString ip)
{
	QByteArray buf;
	buf.resize(4);
	QStringList list = ip.split(".");
	if(list.size() != 4)
		{
			return "";
		}
	for(int i=0;i<list.size();i++)
		{
			buf[i] = QString(list.at(i)).toUtf8().toHex()[0];
		}
	return buf;
}

void Thread::leave()
{
	if(this->socket != NULL)
		{
			this->socket->disconnectFromHost();
			this->socket->deleteLater();
		}
	if(this->target != NULL)
		{
			this->target->disconnectFromHost();
			this->target->deleteLater();
		}
	this->exit();
}
