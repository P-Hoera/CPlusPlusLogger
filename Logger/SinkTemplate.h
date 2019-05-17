#pragma once

#include "SinkBase.h"

#include "LoggerAPI.h"

namespace Log
{
	class LOGGER_API SpecificSinkBase : public SinkBase
	{
	public:
		virtual ~SpecificSinkBase()
		{
			stopThread();

			// other code goes here
		}

	protected:
		SpecificSinkBase(const std::string& sinkName, const SinkType sinkType)
			:SinkBase(sinkName, sinkType)
		{}

		void initialize()
		{
			auto initializeFunction = [&] { /* Initialization code goes here */ };
			SinkBase::initialize(initializeFunction);
		}

		void initializeAsDummy()
		{
			auto initializeAsDummyFunction = [&] { /* Initialization code for dummy goes here */ };
			SinkBase::initializeAsDummy(initializeAsDummyFunction);
		}

		virtual void write(const std::shared_ptr<LogEntry> logEntry) final
		{
			// code to write to the sink target goes here
		}
	};
}

namespace Log
{
	class LOGGER_API SpecificSink final : public SpecificSinkBase
	{
		friend class SinkFactory; //!< Friend to allow construction only from SinkFactory.
	public:
		virtual ~SpecificSink()
		{
			stopThread();
		}

	private:
		SpecificSink(const std::string& sinkName)
			:SpecificSink(sinkName, SinkType::SpecificSink)
		{
			SpecificSink::initialize();
		}
	};
}

namespace Log
{
	class LOGGER_API DelayedSpecificSink final : public SpecificSinkBase
	{
		friend class SinkFactory; //!< Friend to allow construction only from SinkFactory.
	public:
		virtual ~DelayedSpecificSink()
		{
			stopThread();
		}

		void initialize()
		{
			SpecificSinkBase::initialize();
		}

		void initializeAsDummy()
		{
			SpecificSinkBase::initializeAsDummy();
		}

	private:
		DelayedSpecificSink(const std::string& sinkName)
			:SpecificSinkBase(sinkName, SinkType::DelayedSpecificSink)
		{}
	};
}