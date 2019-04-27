#include <QCoreApplication>
#include "server.h"
#include "udptask.h"
#include <QDebug>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
    /*
	if(argc <2)
		{
			qDebug()<<"enter argv";
			return 0;
		}
    */
	Server *s = new Server();
     s->startServer(6666);
	UdpTask *task = new UdpTask();
	task->start();
	
	return a.exec();
}
