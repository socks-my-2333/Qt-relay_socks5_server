#include "analysis.h"

bool Analysis::getTargetDomain(QString fristRead,TargetInfo& info)
{
	QStringList fristList = fristRead.split("#");
	if(fristList.size() != 3)
		{
			return false;
		}
	info.ip = fristList.at(1);
	info.port = QString(fristList.at(2)).toInt();
	info.empty = false;
	return info.empty;
}
