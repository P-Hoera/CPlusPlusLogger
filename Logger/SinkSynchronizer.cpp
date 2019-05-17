#include "SinkSynchronizer.h"

namespace Log
{
	SinkSynchronizer::SinkSynchronizer()
	{
		m_thread = std::thread(&SinkSynchronizer::threadWorkFunction, this);
	}

	SinkSynchronizer::~SinkSynchronizer()
	{
		m_stopWorking.store(true, std::memory_order_relaxed);
		m_workerCondition.notify_all();
		m_thread.join();
	}

	void SinkSynchronizer::logToSinks(std::shared_ptr<RawLogEntry> rawLogEntry, std::shared_ptr<SinkVector> sinks)
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);
		m_entryQueue.push(EntryPair(rawLogEntry, sinks));
		lock.unlock();
		m_workerCondition.notify_all();
	}

	void SinkSynchronizer::threadWorkFunction()
	{
		while (true)
		{
			std::unique_lock<std::mutex> lock(m_queueMutex);

			if (m_entryQueue.size() == 0 && !m_stopWorking.load(std::memory_order_relaxed))
			{
				m_workerCondition.wait(lock, [&] {return m_entryQueue.size() || m_stopWorking.load(std::memory_order_relaxed); });
			}

			if (m_entryQueue.size() != 0)
			{
				size_t numberOfEntries = m_entryQueue.size();

				lock.unlock();

				for (size_t i = 0; i < numberOfEntries; ++i)
				{
					lock.lock();
					EntryPair entry = m_entryQueue.front();
					m_entryQueue.pop();
					lock.unlock();

					std::shared_ptr<LogEntry> logEntry(new LogEntry(*entry.first));

					for (std::shared_ptr<SinkBase> sink : *(entry.second))
					{
						sink->log(logEntry);
					}
				}
			}
			else if (m_stopWorking.load(std::memory_order_relaxed))
			{
				break;
			}
		}
	}
}