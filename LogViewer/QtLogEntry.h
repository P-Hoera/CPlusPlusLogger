#pragma once

#include <LogEntry.h>

#include <qstring.h>
#include <qbrush.h>

class QtLogEntry : public Log::LogEntry
{
public:
	QtLogEntry(const std::string& compiledLogEntry);

	const QString& getTimeStampQString() const;

	const QString& getLogLevelQString() const;

	const QString& getModuleNameQString() const;

	const QString& getThreadIDQString() const;

	const QString& getMessageQString() const;

	const QBrush& getBackgroundQBrush() const;

private:
	QString m_timeStampQString;
	QString m_logLevelQString;
	QString m_moduleNameQString;
	QString m_threadIDQString;
	QString m_messageQString;
	QBrush m_backgroundQBrush;
};