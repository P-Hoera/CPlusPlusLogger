#pragma once

#include "LoggerAPI.h"

#include "LogEntry.h"
#include "SinkType.h"

#include <queue>
#include <mutex>
#include <atomic>
#include <string>
#include <functional>

#pragma warning( push )
#pragma warning( disable : 4251 )

namespace Log
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \brief Base class of all Sinks. Stores log entries for processing by subclass to store them into their sink target.
	///        In order to enable processing the class has to be initialized. It can also be initialized as a dummy 
	///		   in case errors prevent a normal initialization. In this case all stored entries are discarded. 
	///        Initialization of any kind should only be delayed as much as possible to prevent high memory consumtion.
	///		   Deriving classes must implement write() at some point and call stopThread() within any destructor before 
	///		   any other code gets executed to make sure the internal thread is always stopped first to prevent crashes 
	///		   during destruction.
	///
	/// \warning When fully initialized this class runs a thread. All deriving classes have to call stopThread() in
	///			 their destructor befor any other code gets executed to make sure the thread has stoped before 
	///			 destruction kills the write() implemtation or any resources used by it.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class LOGGER_API SinkBase
	{
	public:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Destructor.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual ~SinkBase();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Stores a log entry to be written to the sink target.
		///
		/// \param logEntry The log entry to log.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void log(const std::shared_ptr<LogEntry> logEntry);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Returns the type of this sink.
		///
		/// \returns The type of this sink.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		SinkType getSinkType() const;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Returns if the sink is initialized.
		///
		/// \returns True if the sink is initialized, otherwise false.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool isInitialized() const;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Returns if the sink is initialized as a dummy and thus discards all log entries.
		///
		/// \returns True if the sink is initialized as a dummy, otehrwise false.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool isDummy() const;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Returns the sinks name.
		///
		/// \returns The name of the sink.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const std::string& getSinkName() const;

	protected:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructor.
		///
		/// \param sinkName A name for the sink that can be used to identify it.
		/// \param sinkType The type of the sink.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		SinkBase(const std::string& sinkName, const SinkType sinkType);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Fully initializes the sink and the subclass used in the order subclass->class. Does not do any 
		///		   initialization if the class is already initialized.
		///
		/// \param subClassInitalizeFunction A function or lambda that contains the code used for initializing the subclass. 
		///									 It should throw an exception if the initialization fails and clean up the state 
		///									 of the subclass back to an uninitialized state.
		///
		/// \exception std::invalid_argument Thrown if the function provided is null.
		/// \exception unknown Provided function could throw unknown exceptions.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void initialize(const std::function<void()>& subClassInitalizeFunction);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Initializes the sink and the subclass used as a dummy in the order subclass->class. Does not do any 
		///		   initialization if the class is already initialized. Initializing as dummy discards all log entries.
		///
		/// \param subClassInitalizeAsDummyFunction A function or lambda that contains the code used for initializing the
		///											subclass as dummy. The code contained should never fail, if it does it 
		///											should throw an exception and clean up the state of the subclass back 
		///											to an uninitialized state.
		///
		/// \exception std::invalid_argument Thrown if the function provided is null.
		/// \exception unknown Provided function could throw unknown exceptions.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void initializeAsDummy(const std::function<void()>& subClassInitalizeAsDummyFunction);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Writes a stored log entry to the sink target. Every subclass has to provide this function. It is only 
		///		   called after initialization or initialization as dummy was successful.
		///
		/// \param logEntry The log entry to be written.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual void write(const std::shared_ptr<LogEntry> logEntry) = 0;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Tells the thread forwarding entries to stop working.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void stopThread();

	private:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief The function used by the internal thread. It forwards log entries stored by log() to subclass write()
		///		   function. This function is only active after initialization or initialization as dummy was successful.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void threadWorkFunction();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Initializes the class and subclass either normaly or as dummy.
		///
		/// \param subClassInitalizeFunction The function to initialize the subclass either normaly or as dummy.
		/// \param asDummy True triggers dummy initialization, false normal initialization.
		///
		/// \exception std::invalid_argument Thrown if the function provided is null.
		/// \exception unknown Provided function could throw unknown exceptions.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void initialize(const std::function<void()>& subClassInitalizeFunction, const bool asDummy);

		std::string m_sinkName; //!< The name of the sink.
		SinkType m_sinkType; //!< The type of the sink.
		std::atomic<bool> m_isInitialized = false; //!< If the sink is initialized.
		std::atomic<bool> m_isDummy = false; //!< If the sink is initialized as a dummy.
		std::mutex m_initializationMutex; //!< The mutex used to protect initialization.
		std::queue<std::shared_ptr<LogEntry>> m_entryQueue; //!< The queue used for storing log entries.
		std::mutex m_queueMutex; //!< The mutex used to protect the queue.
		std::atomic<bool> m_stopWorking = false; //!< The variable used to signal the thread that it should stop working.
		std::condition_variable m_workerCondition; //!< The condition variable used to resume the thread if new log entries are available.
		std::thread m_thread; //!< The thread used for the work function.
	};
}

#pragma warning( pop ) 