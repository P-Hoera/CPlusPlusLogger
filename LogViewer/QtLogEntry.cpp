#include "QtLogEntry.h"

#include "LogLevelQColor.h"

QtLogEntry::QtLogEntry(const std::string& compiledLogEntry)
	:LogEntry(compiledLogEntry)
{
	m_timeStampQString = QString::fromStdString(getTimeStampString());
	m_logLevelQString = QString::fromStdString(getLogLevelString());
	m_moduleNameQString = QString::fromStdString(getModuleName());
	m_threadIDQString = QString::fromStdString(getThreadIDString());
	m_messageQString = QString::fromStdString(getMessage());

	m_backgroundQBrush = getLogLevelQColor(getLogLevel());
}

const QString& QtLogEntry::getTimeStampQString() const
{
	return m_timeStampQString;
}

const QString& QtLogEntry::getLogLevelQString() const
{
	return m_logLevelQString;
}

const QString& QtLogEntry::getModuleNameQString() const
{
	return m_moduleNameQString;
}

const QString& QtLogEntry::getThreadIDQString() const
{
	return m_threadIDQString;
}

const QString& QtLogEntry::getMessageQString() const
{
	return m_messageQString;
}

const QBrush& QtLogEntry::getBackgroundQBrush() const
{
	return m_backgroundQBrush;
}