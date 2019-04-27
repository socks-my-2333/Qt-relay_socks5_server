#include "udptask.h"

UdpTask::UdpTask(QObject *parent) : QObject(parent)
{
	
}

void UdpTask::start()
{
	Udp::bind(udp,QHostAddress::AnyIPv4,1080);
	connect(udp,&QUdpSocket::readyRead,this,&UdpTask::assignation);
}

void UdpTask::assignation()
{
	QHostAddress address;
	quint16 port;
	QByteArray buff = Udp::read(udp,&address,&port);
	if(buff == UDP_READY)
		{
			blockTurn *turn = new blockTurn(address.toString().replace("::ffff:",""),port);	//去掉多余的ffff
			connect(turn,&blockTurn::sendPort,this,&UdpTask::resendPort,Qt::QueuedConnection);
			turn->start();
		}
}

void UdpTask::resendPort(int port, QString sourceip, quint16 sourceport)
{
	Udp::write(udp,QString::number(port).toUtf8(),QHostAddress(sourceip),sourceport);	//将新分配的udp服务告诉客户端供予连接使用
}
