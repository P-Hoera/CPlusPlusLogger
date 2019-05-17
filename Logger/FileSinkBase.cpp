#include "FileSinkBase.h"

namespace Log
{
	FileSinkBase::~FileSinkBase()
	{
		stopThread();
		m_file.flush();
	}

	const std::string& FileSinkBase::getFileName() const
	{
		return m_fileName;
	}

	FileSinkBase::FileSinkBase(const std::string& sinkName, const SinkType sinkType)
		:SinkBase(sinkName, sinkType)
	{}

	void FileSinkBase::write(const std::shared_ptr<LogEntry> logEntry)
	{
		m_file << logEntry->getCompiledLogEntry();
	}

	void FileSinkBase::initialize(const std::string& fileName)
	{
		auto initializeFunction = [&] 
		{
			m_file.open(fileName);

			if (!m_file.is_open())
			{
				throw std::invalid_argument("Could not open file for writing.");
			}
										
			m_fileName = fileName; 
		};


		SinkBase::initialize(initializeFunction);
	}

	void FileSinkBase::initializeAsDummy()
	{
		auto initializeAsDummyFunction = [&] {};
		SinkBase::initializeAsDummy(initializeAsDummyFunction);
	}
}