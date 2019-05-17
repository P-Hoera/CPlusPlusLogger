#include "LogFileLoader.h"

#include <iostream>

LogFileLoader::~LogFileLoader()
{
	m_stopWorking.store(true, std::memory_order_relaxed);
	m_workerCondition.notify_all();
	m_thread.join();
}

LogFileLoader::LogFileLoader(const std::shared_ptr<SourceProcessor>& sourceProcessor)
	:m_sourceProcessor(sourceProcessor)
{
	m_thread = std::thread(&LogFileLoader::threadWorkFunction, this);
}

void LogFileLoader::loadFile(const std::string& fileName)
{
	std::shared_ptr<std::ifstream> file(new std::ifstream(fileName));

	if (!file->is_open())
	{
		throw std::invalid_argument("Could not open file.");
	}

	std::string fileNameOnly = fileName.substr(fileName.find_last_of('\\') + 1);

	std::unique_lock<std::mutex> lock(m_queueMutex);
	m_logFileQueue.push(LogFilePair(fileNameOnly, file));
	lock.unlock();
	m_workerCondition.notify_all();
}

void LogFileLoader::threadWorkFunction()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);

		if (m_logFileQueue.size() == 0 && !m_stopWorking.load(std::memory_order_relaxed))
		{
			m_workerCondition.wait(lock, [&] {return m_logFileQueue.size() || m_stopWorking.load(std::memory_order_relaxed); });
		}

		if (m_logFileQueue.size() != 0)
		{
			LogFilePair entry = m_logFileQueue.front();
			m_logFileQueue.pop();
			lock.unlock();

			void* sourcePointer = m_sourceProcessor->openSource(std::shared_ptr<std::string>(new std::string(entry.first)), SourceType::File);

			while (!entry.second->eof() && entry.second->good() && !m_stopWorking.load(std::memory_order_relaxed))
			{
				std::shared_ptr<std::string> line(new std::string());
				std::getline(*entry.second, *line);
				*line += "\n";

				m_sourceProcessor->processCompiledLogEntry(sourcePointer, line);
			}

			m_sourceProcessor->closeSource(sourcePointer);
		}
		
		if (m_stopWorking.load(std::memory_order_relaxed))
		{
			break;
		}
	}
}