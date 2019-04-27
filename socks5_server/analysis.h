#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <QObject>
#include <QByteArray>
#include <QStringList>

class Analysis
{
public:
	struct TargetInfo
	{
		QString ip;
		int	   port;
		bool	   empty=true;
	};
	static bool getTargetDomain(QString buff,TargetInfo& info);
};

#endif // ANALYSIS_H
