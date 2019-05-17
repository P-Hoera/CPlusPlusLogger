#include "LogLevelQColor.h"

QColor getLogLevelQColor(const Log::LogLevel logLevel)
{
	switch (logLevel)
	{
	case Log::LogLevel::Fatal:
	{
		return QColor(190, 0, 0);
	}
	case Log::LogLevel::Error:
	{
		return QColor(235, 0, 0);
	}
	case Log::LogLevel::Warning:
	{
		return QColor(235, 235, 0);
	}
	case Log::LogLevel::Info:
	{
		return QColor(0, 140, 0);
	}
	case Log::LogLevel::Debug:
	{
		return QColor(0, 200, 0);
	}
	case Log::LogLevel::Trace:
	{
		return QColor(0, 235, 0);
	}
	}

	return QColor();
}