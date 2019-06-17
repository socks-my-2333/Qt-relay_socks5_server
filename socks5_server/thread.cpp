#include "thread.h"
#include <QDebug>

Thread::Thread(int id, encode code, QObject *parent) : QThread(parent)
{
	socketID = id;
	this->type = code;
	
}

void Thread::run()
{

	key = QCryptographicHash::hash(key,QCryptographicHash::Md5);
	
	socket = new QTcpSocket();		//跟客户通信
	target = new QTcpSocket();		//跟目标通信,,比如说Google
	
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
	Analysis::TargetInfo tarinfo;
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
	
	switch (type) {
		case NONE:
			{
			break;
			}
		case XOR:
			{
			for(int i = 0;i<buffer.size();i++)
				{
					buffer[i] = buffer[i] ^ 1611;
				}
			break;
		}
		case AES:
			{
			QAesWrap aes(key,key,QAesWrap::AesBit::AES_128);
			QByteArray rec = aes.decrypt(buffer,QAesWrap::AesMode::AES_ECB);
			break;
			}
		default:
			{
			break;
			}
		}


	target->write(buffer);
}

void Thread::writeToSource()
{
	
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
	
	switch (type) {
		case NONE:
			{
			break;
			}
		case XOR:
			{
			for(int i = 0;i<buffer.size();i++)
				{
					buffer[i] = buffer[i] ^ 1611;
				}
			break;
			}
		case AES:
			{
			QAesWrap aes(key,key,QAesWrap::AesBit::AES_128);
			QByteArray rec = aes.decrypt(buffer,QAesWrap::AesMode::AES_ECB);
			break;
			}
		default:
			{
			break;
			}
		}

	socket->write(buffer);
}

void Thread::leave()
{
	qDebug()<<"dissconnected";
	if(socketStop == false)
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
