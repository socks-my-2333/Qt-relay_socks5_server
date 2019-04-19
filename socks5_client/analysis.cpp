#include "analysis.h"

Analysis::Analysis(QObject *parent) : QObject(parent)
{
	
}



QByteArray Analysis::hexToIP(QByteArray buf)
{
	bool ok;	//还原成IPV4地址格式
	QString ip = QString::number(QString(buf.mid(4,1).toHex()).toInt(&ok,16)) +"."+
			QString::number(QString(buf.mid(5,1).toHex()).toInt(&ok,16)) +"."+
			QString::number(QString(buf.mid(6,1).toHex()).toInt(&ok,16)) +"."+
			QString::number(QString(buf.mid(7,1).toHex()).toInt(&ok,16));
	return ip.toUtf8();
}

int Analysis::hexToPort(QByteArray buf)
{
	bool ok;
	return QString(buf.toHex()).right(4).toInt(&ok,16);
}

QByteArray Analysis::hexToDomain(QByteArray buf)
{

	QString str(buf.mid(4,1).toHex());
	
	bool ok;
	int domainLength=str.toInt(&ok,16);
	QByteArray domain(buf.mid(5,domainLength));	//获取域名
	return domain;
}

int Analysis::hexAllLength(QByteArray buf)
{
	if(buf[3] == 0x01)
		{
			return 9;
		}
	else if(buf[3] == 0x03)
		{
			bool ok;
			QString str(buf.mid(4,1).toHex());
			int domainLength=str.toInt(&ok,16);
			return 6+ domainLength;
		}
	return 0;
}

QByteArray Analysis::ipToHex(QString ip)
{
	QByteArray buf;
	buf.resize(4);
	QStringList list = ip.split(".");
	if(list.size() != 4)
		{
			return "";
		}
	for(int i=0;i<list.size();i++)
		{
			buf[i] = (QString(list.at(i)).toInt() % 256);
		}
	return buf;
}

QByteArray Analysis::portToHex(int port)
{
	QByteArray send;
	send.resize(2);
	send[0] = (port / 256);
	send[1] = (port % 256);
	
	return send;
}
