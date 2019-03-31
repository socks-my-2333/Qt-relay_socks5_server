#include "thread.h"
#include <QDebug>

Thread::Thread(int id, QObject *parent) : QThread(parent)
{
    this->socketID = id;
}

void Thread::run()
{
    this->socket = new QTcpSocket();
    this->target = new QTcpSocket();
    qDebug()<<"is running";
    if(!this->socket->setSocketDescriptor(this->socketID))
    {
        qDebug()<<"fail setSocketDescriptor error: "<<socket->errorString();
    }
    if(!this->socket->waitForReadyRead(5000))
    {
        return;
    }
    else
    {
        QString fristRead =  this->socket->readAll();
        qDebug()<<"nor problem: "<<fristRead;
        QStringList fristList = fristRead.split("#");
        if(fristList.size() != 3)
        {
            //清洁函数
            leave();
            return;
        }

        this->targetIP = fristList.at(1);
        this->targetPort = QString(fristList.at(2)).toInt();
	   
	    if(fristList.at(0) == "DOMAIN")
		   {
			   QHostInfo info = QHostInfo::fromName(targetIP);
			   if(!info.addresses().isEmpty())
				   {
					   QHostAddress addres = info.addresses().first();
					   this->targetIP = addres.toString();
				   }
			   else
				   {
					   leave();
					   return;
				   }
		   }
	   
        this->target->connectToHost(QHostAddress(this->targetIP),this->targetPort);
        if(!this->target->waitForConnected(2000))
        {
            leave();
            return;
        }
        qDebug()<<"connect to:  "<<this->targetIP<<"  "<<this->targetPort;

        connect(this->socket,&QTcpSocket::readyRead,this,&Thread::writeToTarget,Qt::DirectConnection);
        connect(this->socket,&QTcpSocket::disconnected,this,&Thread::leave,Qt::DirectConnection);
        connect(this->target,&QTcpSocket::disconnected,this,&Thread::leave,Qt::DirectConnection);
        connect(this->target,&QTcpSocket::readyRead,this,&Thread::writeToSource,Qt::DirectConnection);
        this->socket->write("#FINSH#");
    }

    this->exec();
}

void Thread::writeToTarget()
{
   
    QByteArray buffer = this->socket->readAll();
    for(int i =0;i<buffer.size();i++)
	    {
		    buffer[i] = buffer[i]^1611;
	    }
    this->target->write(buffer);
}

void Thread::writeToSource()
{
    
    QByteArray buffer = this->target->readAll();
    for(int i =0;i<buffer.size();i++)
	    {
		    buffer[i] = buffer[i]^1611;
	    }
    this->socket->write(buffer);
}

void Thread::leave()
{
    if(this->socket != NULL)
    {
        qDebug()<<"delete socket";
        this->socket->disconnectFromHost();
        this->socket->deleteLater();
        this->socket = NULL;
    }
    if(this->target != NULL)
    {
        qDebug()<<"delete target";
        this->target->disconnectFromHost();
        this->target->deleteLater();
        this->socket = NULL;
    }
    this->exit();
}
