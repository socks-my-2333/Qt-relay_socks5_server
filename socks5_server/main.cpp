#include <QCoreApplication>
#include "server.h"
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
	return a.exec();
}
