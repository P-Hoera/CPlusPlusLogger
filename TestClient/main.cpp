#include <iostream>
#include <stdlib.h>
#include <time.h>

#include <LoggerFactory.h>
#include <SinkFactory.h>

struct Entry
{
	std::shared_ptr<Log::Logger> logger;
	Log::LogLevel logLevel;
	std::string message;
};

class LoggerThread
{
public:
	LoggerThread()
	{
		m_thread = std::thread(&LoggerThread::threadWorkFunction, this);
	}

	~LoggerThread()
	{
		m_stopWorking.store(true, std::memory_order_relaxed);
		m_workerCondition.notify_all();
		m_thread.join();
	}

	void log(const Entry& entry)
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);
		m_entryQueue.push(entry);
		lock.unlock();
		m_workerCondition.notify_all();
	}

private:
	void threadWorkFunction()
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
				Entry entry = m_entryQueue.front();
				m_entryQueue.pop();
				lock.unlock();
				entry.logger->logAlways(entry.logLevel, entry.message);
			}

			if (m_stopWorking.load(std::memory_order_relaxed))
			{
				break;
			}
		}
	}

	std::queue<Entry> m_entryQueue;
	std::mutex m_queueMutex;
	std::atomic<bool> m_stopWorking = false;
	std::condition_variable m_workerCondition;
	std::thread m_thread;
};

void main()
{
	std::vector<std::shared_ptr<Log::Logger>> loggerVector;
	for (int i = 0; i < 10; ++i)
	{
		loggerVector.push_back(Log::LoggerFactory::createLogger("Test Logger " + std::to_string(i), "Test Module " + std::to_string(i), Log::LogLevel::Info));
	}

	std::shared_ptr<Log::FileSink> fileSink = Log::SinkFactory::createFileSink("Test File Sink", "testfile.txt");
	std::shared_ptr<Log::LogViewerLocalRPCSink> localRPCSink;

	try
	{
		localRPCSink = Log::SinkFactory::createLogViewerLocalRPCSink("Test Local RPC Sink");
	}
	catch (const std::runtime_error & exception)
	{
		std::cout << "Local RPC Server not available" << std::endl;
	}

	std::shared_ptr<Log::LogViewerNetworkRPCSink> networkRPCSink;
	try
	{
		networkRPCSink = Log::SinkFactory::createLogViewerNetworkRPCSink("Test Network RPC Sink", "");
	}
	catch (const std::runtime_error& exception)
	{
		std::cout << "Network RPC Server not available" << std::endl;
	}

	for (auto& entry : loggerVector)
	{
		entry->connectToSink(fileSink);
		entry->connectToSink(localRPCSink);
		entry->connectToSink(networkRPCSink);
	}

	int numberOfThreads = 5;
	std::vector<LoggerThread> loggerThreads(numberOfThreads);

	srand(time(NULL));

	for (int i = 0; i < 100000; ++i)
	{
		int loggerToUse = rand() % loggerVector.size();
		Log::LogLevel logLevelToUse = static_cast<Log::LogLevel>(rand() % 6);
		int threadToUse = rand() % numberOfThreads;

		Entry entry;
		entry.logger = loggerVector[loggerToUse];
		entry.logLevel = logLevelToUse;
		entry.message = "Entry: " + std::to_string(i) + " Logger: " + loggerVector[loggerToUse]->getModuleName() + " Log Level: " + Log::logLevelToString(logLevelToUse);
		loggerThreads[threadToUse].log(entry);
	}

	std::cin.get();
}