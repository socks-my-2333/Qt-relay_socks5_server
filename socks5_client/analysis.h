#ifndef ANALYSIS_H
#define ANALYSIS_H

/*
*
*    这个类用来拆分SOCKS5交互过程中的信息
*
*/

#include <QObject>
#include <QByteArray>
#include <QStringList>
#include <QHostInfo>

class Analysis 
{
//	Q_OBJECT
public:
//	explicit Analysis(QObject *parent = nullptr);
	
signals:
	
public slots:
	static QByteArray hexToIP(QByteArray buf);		//16进制转ip
	static int hexToPort(QByteArray buf);		//16进制转端口
	static QByteArray hexToDomain(QByteArray buf);		//16进制转域名
	static int hexAllLength(QByteArray buf);
	static QByteArray ipToHex(QString ip);
	static QByteArray portToHex(int port);
	static QString	domainToIp(QString doamin);
};

#endif // ANALYSIS_H
