#include "server.h"
#include <QDebug>

Server::Server(QObject *parent) : QTcpServer(parent)
{

}

void Server::incomingConnection(qintptr id)
{
	Thread *t =new Thread(id,this->type);
	t->start();
}

bool Server::startServer(int port, Thread::encode code)
{
	this->type = code;
	qDebug()<<"type "<<code;
	qDebug()<<"port "<<port;
    if(this->listen(QHostAddress::AnyIPv4,port))
		{
			return true;
		}
	else
		{
			qDebug()<<"start server fail error:"<<this->errorString();
			return false;
		}
}

void Server::stopServer()
{
	this->close();
}
