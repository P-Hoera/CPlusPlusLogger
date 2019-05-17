#include "SinkBase.h"

namespace Log
{
	void SinkBase::log(const std::shared_ptr<LogEntry> logEntry)
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);	
		m_entryQueue.push(logEntry);
		lock.unlock();
		m_workerCondition.notify_all();
	}

	SinkType SinkBase::getSinkType() const
	{
		return m_sinkType;
	}

	bool SinkBase::isInitialized() const
	{
		return m_isInitialized.load(std::memory_order_relaxed);
	}

	bool SinkBase::isDummy() const
	{
		return m_isDummy.load(std::memory_order_relaxed);
	}

	const std::string& SinkBase::getSinkName() const
	{
		return m_sinkName;
	}

	SinkBase::~SinkBase()
	{
		stopThread();
	}

	SinkBase::SinkBase(const std::string& sinkName, const SinkType sinkType)
		:m_sinkName(sinkName), m_sinkType(sinkType)
	{
	}

	void SinkBase::initialize(const std::function<void()>& subClassInitalizeFunction)
	{
		initialize(subClassInitalizeFunction, false);
	}

	void SinkBase::initializeAsDummy(const std::function<void()>& subClassInitalizeAsDummyFunction)
	{
		initialize(subClassInitalizeAsDummyFunction, true);
	}

	void SinkBase::initialize(const std::function<void()>& subClassInitalizeFunction, const bool asDummy)
	{
		std::unique_lock<std::mutex> lock(m_initializationMutex);

		if (subClassInitalizeFunction == nullptr)
		{
			throw std::invalid_argument("Invalid initialization function.");
		}

		if (isInitialized())
		{
			return;
		}

		subClassInitalizeFunction();

		m_isDummy.store(asDummy, std::memory_order_relaxed);
		m_thread = std::thread(&SinkBase::threadWorkFunction, this);
		m_isInitialized.store(true, std::memory_order_relaxed);
	}

	void SinkBase::stopThread()
	{
		if (isInitialized() && !m_stopWorking.load(std::memory_order_relaxed))
		{
			m_stopWorking.store(true, std::memory_order_relaxed);
			m_workerCondition.notify_all();
			m_thread.join();
		}
	}

	void SinkBase::threadWorkFunction()
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
					std::shared_ptr<LogEntry> logEntry = m_entryQueue.front();
					m_entryQueue.pop();
					lock.unlock();

					if (!isDummy())
					{
						write(logEntry);
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