#include "blockturn.h"

blockTurn::blockTurn(QString host, quint16 port, QObject *parent) : QThread(parent)
{
	this->sourceIP = host;
	this->sourcePort = port;
}

void blockTurn::run()
{
	udp =new QUdpSocket();

	if(udp->bind(QHostAddress::AnyIPv4))
		{
			localPort = udp->localPort();
			emit sendPort(static_cast<quint16>(localPort),sourceIP,sourcePort);
		}
	else
		{
			Leave();
			return;
		}
	if(udp->waitForReadyRead())
		{		//第一次交互获取信息是发送给谁的
			QByteArray buff = Udp::read(udp);
			Analysis::TargetInfo info;
			Analysis::getTargetDomain(QString(buff),info);
			targetIP = info.ip;
			targetPort = info.port;
		}
	else
		{
			Leave();
			return;
		}
	//实例化一个UDP用来发送,区分开发送给客户和发送给目标的
	tar= new QUdpSocket();
	if(!tar->bind(QHostAddress::AnyIPv4))
		{
			Leave();
			return;
		}
	
	connect(udp,&QUdpSocket::readyRead,this,&blockTurn::toTarget);
	connect(tar,&QUdpSocket::readyRead,this,&blockTurn::toSource);
	this->exec();
}

void blockTurn::toTarget()
{
	QByteArray buff = Udp::read(udp);
	Udp::write(tar,buff,QHostAddress(this->targetIP),targetPort);
}

void blockTurn::toSource()
{
	QByteArray buff = Udp::read(tar);
	Udp::write(udp,buff,QHostAddress(sourceIP),sourcePort);
}

void blockTurn::Leave()
{
	if(udp != nullptr)
		{
			udp->disconnectFromHost();
			udp->close();
			udp->deleteLater();
		}
	if(tar != nullptr)
		{
			tar->disconnectFromHost();
			tar->close();
			tar->deleteLater();
		}
	exit(0);
}
