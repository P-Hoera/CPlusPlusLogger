#pragma once

#include "SourceProcessor.h"

#include <thread>

class LogViewerRPCServer
{
public:
	LogViewerRPCServer(const std::shared_ptr<SourceProcessor>& sourceProcessor);

	~LogViewerRPCServer();

private:
	void threadWorkFunction();

	std::thread m_thread; //!< The thread used for the work function.
};