#include "SinkFactory.h"

namespace Log
{
	std::shared_ptr<ConsoleSink> SinkFactory::createConsoleSink(const std::string& sinkName)
	{
		std::shared_ptr<ConsoleSink> sink(new ConsoleSink(sinkName));
		return sink;
	}

	std::shared_ptr<DelayedConsoleSink> SinkFactory::createDelayedConsoleSink(const std::string& sinkName)
	{
		std::shared_ptr<DelayedConsoleSink> sink(new DelayedConsoleSink(sinkName));
		return sink;
	}

	std::shared_ptr<FileSink> SinkFactory::createFileSink(const std::string& sinkName, const std::string& fileName)
	{
		std::shared_ptr<FileSink> sink(new FileSink(sinkName, fileName));
		return sink;
	}

	std::shared_ptr<DelayedFileSink> SinkFactory::createDelayedFileSink(const std::string& sinkName)
	{
		std::shared_ptr<DelayedFileSink> sink(new DelayedFileSink(sinkName));
		return sink;
	}

	std::shared_ptr<LogViewerLocalRPCSink> SinkFactory::createLogViewerLocalRPCSink(const std::string& sinkName)
	{
		std::shared_ptr<LogViewerLocalRPCSink> sink(new LogViewerLocalRPCSink(sinkName));
		return sink;
	}

	std::shared_ptr<DelayedLogViewerLocalRPCSink> SinkFactory::createDelayedLogViewerLocalRPCSink(const std::string& sinkName)
	{
		std::shared_ptr<DelayedLogViewerLocalRPCSink> sink(new DelayedLogViewerLocalRPCSink(sinkName));
		return sink;
	}

	std::shared_ptr<LogViewerNetworkRPCSink> SinkFactory::createLogViewerNetworkRPCSink(const std::string& sinkName, const std::string& networkAddress)
	{
		std::shared_ptr<LogViewerNetworkRPCSink> sink(new LogViewerNetworkRPCSink(sinkName, networkAddress));
		return sink;
	}

	std::shared_ptr<DelayedLogViewerNetworkRPCSink> SinkFactory::createDelayedLogViewerNetworkRPCSink(const std::string& sinkName)
	{
		std::shared_ptr<DelayedLogViewerNetworkRPCSink> sink(new DelayedLogViewerNetworkRPCSink(sinkName));
		return sink;
	}
}