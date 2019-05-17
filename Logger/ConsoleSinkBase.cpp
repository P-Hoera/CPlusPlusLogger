#include "ConsoleSinkBase.h"

#include <iostream>

namespace Log
{
	ConsoleSinkBase::~ConsoleSinkBase()
	{
		stopThread();
	}

	ConsoleSinkBase::ConsoleSinkBase(const std::string& sinkName, const SinkType sinkType)
		:SinkBase(sinkName, sinkType)
	{}

	void ConsoleSinkBase::initialize()
	{
		auto initializeFunction = [&] {};
		SinkBase::initialize(initializeFunction);
	}

	void ConsoleSinkBase::initializeAsDummy()
	{
		auto initializeAsDummyFunction = [&] {};
		SinkBase::initializeAsDummy(initializeAsDummyFunction);
	}

	void ConsoleSinkBase::write(const std::shared_ptr<LogEntry> logEntry)
	{
		std::cout << logEntry->getCompiledLogEntry();
	}
}