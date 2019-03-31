#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include "thread.h"

class Server : public QTcpServer
{
	Q_OBJECT
public:
	explicit Server(QObject *parent = nullptr);
	
signals:
	
public slots:
    void incomingConnection(qintptr id);
	bool startServer(int port=0);
	void stopServer();
};

#endif // SERVER_H
