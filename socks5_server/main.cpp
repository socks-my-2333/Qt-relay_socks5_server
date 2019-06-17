#include <QCoreApplication>
#include "server.h"
#include "udptask.h"	//处理udp请求
#include <QDebug>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	if(argc <3)
		{
			return 0;
		}
	QByteArray argOne(argv[1]);
	QByteArray argTwo(argv[2]);
	Thread::encode code;
	if(argTwo == "NONE")
		{
			code = Thread::NONE;
		}
	else if(argTwo == "XOR")
		{
			code =Thread::XOR;
		}
	else if(argTwo == "AES")
		{
			code = Thread::AES;
		}
	
	Server *s = new Server();
     s->startServer(QString(argOne).toInt(),code);
	
	return a.exec();
}
