#include "thread.h"
#include <QDebug>

Thread::Thread(int id, QObject *parent) : QThread(parent)
{
	socketID = id;
}

void Thread::run()
{
	socket = new QTcpSocket();
	target = new QTcpSocket();
	
	qDebug()<<"is running";
	if(!socket->setSocketDescriptor(socketID))
		{
			qDebug()<<"fail setSocketDescriptor error: "<<socket->errorString();
		}
	
	
	if(!approve())
		{
			leave();
			return;
		}
	connect(socket,&QTcpSocket::readyRead,this,&Thread::writeToTarget,Qt::DirectConnection);
	connect(socket,&QTcpSocket::disconnected,this,&Thread::leave,Qt::DirectConnection);
	connect(target,&QTcpSocket::disconnected,this,&Thread::leave,Qt::DirectConnection);
	connect(target,&QTcpSocket::readyRead,this,&Thread::writeToSource,Qt::DirectConnection);
	socket->write("#FINSH#");
	
	exec();
}

bool Thread::approve()
{
	qDebug()<<"approve";
	if(!socket->waitForReadyRead(5000))
		{
			return false;
		}
	else
		{
			QString fristRead =  socket->readAll();
			
			if(!setTargetInfo(fristRead))
				{
					return false;
				}
			
			target->connectToHost(QHostAddress(targetIP),targetPort);
			
			if(!target->waitForConnected(2000))
				{
					return false;
				}
		}
	return true;
}

bool Thread::setTargetInfo(QString fristRead)
{
	qDebug()<<"setTargetInfo";
	TargetInfo tarinfo;
	if(Analysis::getTargetDomain(fristRead,tarinfo))
		{
			return false;
		}
	
	targetIP = tarinfo.ip;
	targetPort = tarinfo.port;
	
	QHostInfo info = QHostInfo::fromName(targetIP);
	if(!info.addresses().isEmpty())
		{
			QHostAddress addres = info.addresses().first();
			targetIP = addres.toString();		//设置目标ip
			return true;
		}
	else
		{
			return false;
		}
	return false;
}

void Thread::writeToTarget()
{
	
	QByteArray buffer = socket->readAll();
	for(int i =0;i<buffer.size();i++)
		{
			buffer[i] = buffer[i]^1611;
		}
	target->write(buffer);
}

void Thread::writeToSource()
{
	
	QByteArray buffer = target->readAll();
	for(int i =0;i<buffer.size();i++)
		{
			buffer[i] = buffer[i]^1611;
		}
	socket->write(buffer);
}

void Thread::leave()
{
	if(socket != nullptr)
		{
			qDebug()<<"delete socket";
			socket->disconnectFromHost();
			socket->close();
			socket->deleteLater();
		}
	if(target != nullptr)
		{
			qDebug()<<"delete target";
			target->disconnectFromHost();
			target->close();
			target->deleteLater();
		}
	exit();
}
