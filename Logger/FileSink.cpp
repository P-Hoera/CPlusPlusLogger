#include "FileSink.h"

namespace Log
{
	FileSink::FileSink(const std::string& sinkName, const std::string& fileName)
		:FileSinkBase(sinkName, SinkType::FileSink)
	{
		FileSinkBase::initialize(fileName);
	}

	FileSink::~FileSink()
	{
		stopThread();
	}
}