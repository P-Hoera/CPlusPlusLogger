#include "SourceProcessor.h"

#include <qthread.h>

SourceProcessor::SourceProcessor(UIConnectionInterface* uiConnection)
	:m_uiConnection(uiConnection)
{
	m_thread = std::thread(&SourceProcessor::threadWorkFunction, this);
}

SourceProcessor::~SourceProcessor()
{
	m_stopWorking.store(true, std::memory_order_relaxed);
	m_workerCondition.notify_all();
	m_thread.join();
}

void* SourceProcessor::openSource(const std::shared_ptr<std::string>& sourceName, const SourceType sourceType)
{
	std::unique_lock<std::mutex> lock(m_sourcePointerMutex);
	int* sourcePointer = new int(m_nextSourceID);
	m_sourcePointerVector.push_back(std::shared_ptr<int>(sourcePointer));
	++m_nextSourceID;
	lock.unlock();

	m_uiConnection->openSource(*sourcePointer, sourceName, sourceType);

	return sourcePointer;
}

void SourceProcessor::closeSource(void* sourcePointer)
{
	std::unique_lock<std::mutex> lock(m_queueMutex);
	m_operationsQueue.push(Operation{ OperationType::CloseSource, reinterpret_cast<int*>(sourcePointer), nullptr });
	lock.unlock();
	m_workerCondition.notify_all();
}

void SourceProcessor::processCompiledLogEntry(void* sourcePointer, const std::shared_ptr<std::string>& compiledLogEntry)
{
	std::unique_lock<std::mutex> lock(m_queueMutex);
	m_operationsQueue.push(Operation{ OperationType::ProcessCompiledLogEntry, reinterpret_cast<int*>(sourcePointer), compiledLogEntry });
	lock.unlock();
	m_workerCondition.notify_all();
}

void SourceProcessor::threadWorkFunction()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);

		if (m_operationsQueue.size() == 0 && !m_stopWorking.load(std::memory_order_relaxed))
		{
			m_workerCondition.wait(lock, [&] {return m_operationsQueue.size() || m_stopWorking.load(std::memory_order_relaxed); });
		}

		if (m_operationsQueue.size() != 0)
		{
			Operation operation = m_operationsQueue.front();
			m_operationsQueue.pop();
			lock.unlock();

			switch (operation.operation)
			{
			case OperationType::ProcessCompiledLogEntry:
			{
				try
				{
					std::shared_ptr<QtLogEntry> logEntry(new QtLogEntry(*operation.compiledLogEntry));
					m_uiConnection->addLogEntry(*operation.sourcePointer, logEntry);
				}
				catch (const std::invalid_argument & exception)
				{
				}
				break;
			}
			case OperationType::CloseSource:
			{
				m_uiConnection->closeSource(*operation.sourcePointer);
				break;
			}
			}
		}

		if (m_stopWorking.load(std::memory_order_relaxed))
		{
			break;
		}
	}
}