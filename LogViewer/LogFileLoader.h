#pragma once

#include "SourceProcessor.h"

#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <fstream>

class LogFileLoader
{
	using LogFilePair = std::pair<std::string, std::shared_ptr<std::ifstream>>;
public:
	LogFileLoader(const std::shared_ptr<SourceProcessor>& sourceProcessor);

	~LogFileLoader();

	void loadFile(const std::string& fileName);

private:
	void threadWorkFunction();

	std::shared_ptr<SourceProcessor> m_sourceProcessor;
	std::queue<LogFilePair> m_logFileQueue;
	std::mutex m_queueMutex;
	std::atomic<bool> m_stopWorking = false;
	std::condition_variable m_workerCondition;
	std::thread m_thread;
};