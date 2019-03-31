#include "server.h"
#include <QDebug>

Server::Server(QObject *parent) : QTcpServer(parent)
{
	
}

void Server::incomingConnection(int id)
{
	Thread *t = new Thread(id,this->server_ip,this->server_port);
//	connect(t,&Thread::sendSize,this,&Server::getSize,Qt::AutoConnection);
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
	emit sendSize(size);
}

void Server::setInfo(QString ip, int port)
{
	this->server_ip = ip;
	this->server_port = port;
	qDebug()<<"ip "<<ip;
	qDebug()<<"port "<<port;
}
