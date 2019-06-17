#include "server.h"
#include <QDebug>

Server::Server(QObject *parent) : QTcpServer(parent)
{
	timer = new QTimer();
	timer->start(1000);
	connect(timer,&QTimer::timeout,this,&Server::getSpeed);
	
}

void Server::incomingConnection(int id)
{
	Thread *t = new Thread(id,this->type,this->server_ip,this->server_port);
	connect(t,&Thread::sendSize,this,&Server::getSize,Qt::AutoConnection);
	t->start();
}

bool Server::startServer(QString address, int port)
{
	if(this->listen(QHostAddress(address),port))
		{
			qDebug()<<"success listen";
			return true;
		}
	return false;
	
}

void Server::stopServer()
{
	this->close();
	this->ip.clear();
	this->port = 0;
}

void Server::getSize(int size)
{
	
	this->lastSize = this->recSize;
	this->recSize += size;
//	emit sendSize(size);
}

void Server::setInfo(QString ip, int port,ENCODE_TYPE typ)
{
	this->server_ip = ip;
	this->server_port = port;
	this->type = typ;
	qDebug()<<"ip "<<ip;
	qDebug()<<"port "<<port;
	qDebug()<<"type "<<typ;
}

void Server::getSpeed()
{
	emit sendSize(recSize);
	timer->start(1000);
	int speed = recSize - lastSize;
	emit sendSpeed(speed);
}
