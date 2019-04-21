#include "mainwindow.h"
#include <QApplication>
#include "server.h"
#include "udp.h"
#include <QUdpSocket>
#include <QDebug>
#include "windows.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	/*
	QUdpSocket *socket = new QUdpSocket();
	socket->bind(4444);
	
	QObject::connect(socket,&QUdpSocket::readyRead,[=](){
			QHostAddress add;
			quint16 p;
			qDebug()<<"recv: "<<Udp::read(socket,&add,&p);
		});
	*/
//	Udp::write(socket,"this is test",QHostAddress("127.0.0.1"),6666);
	MainWindow w;
	w.show();
	return a.exec();
}
