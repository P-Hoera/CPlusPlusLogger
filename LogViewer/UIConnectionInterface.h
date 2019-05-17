#pragma once

#include "SourceType.h"

#include "QtLogEntry.h"

#include <string>
#include <memory>

class UIConnectionInterface
{
public:
	virtual void openSource(const int sourceID, const std::shared_ptr<std::string>& sourceName, const SourceType sourceType) = 0;

	virtual void closeSource(const int sourceID) = 0;

	virtual void addLogEntry(const int sourceID, const std::shared_ptr<QtLogEntry>& logEntry) = 0;
};