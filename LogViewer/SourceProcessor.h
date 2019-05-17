#pragma once

#include "UIConnectionInterface.h"

#include <queue>
#include <mutex>
#include <atomic>

class SourceProcessor
{
	enum class OperationType
	{
		ProcessCompiledLogEntry,
		CloseSource
	};

	struct Operation
	{
		OperationType operation;
		int* sourcePointer;
		std::shared_ptr<std::string> compiledLogEntry;
	};

public:
	SourceProcessor(UIConnectionInterface* uiConnection);

	~SourceProcessor();

	void* openSource(const std::shared_ptr<std::string>& sourceName, const SourceType sourceType);

	void closeSource(void* sourcePointer);

	void processCompiledLogEntry(void* sourcePointer, const std::shared_ptr<std::string>& compiledLogEntry);

private:
	void threadWorkFunction();

	UIConnectionInterface* m_uiConnection;
	std::queue<Operation> m_operationsQueue;
	std::mutex m_queueMutex;
	std::vector<std::shared_ptr<int>> m_sourcePointerVector;
	std::mutex m_sourcePointerMutex;
	int m_nextSourceID = 0;
	std::atomic<bool> m_stopWorking = false;
	std::condition_variable m_workerCondition;
	std::thread m_thread;
};