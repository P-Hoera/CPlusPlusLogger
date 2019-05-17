#pragma once

#include "LoggerAPI.h"

#include "RawLogEntry.h"
#include "SinkBase.h"

#include <queue>
#include <mutex>
#include <vector>
#include <atomic>

#pragma warning( push )
#pragma warning( disable : 4251 )

namespace Log
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \brief A synchronizer to make sure that entries are written to all sinks in order of their occurence.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class LOGGER_API SinkSynchronizer
	{
		friend class LoggerFactory;
		using SinkVector = std::vector<std::shared_ptr<SinkBase>>; //!< SinkVector for easier use of the sink vector list.
		using EntryPair = std::pair<std::shared_ptr<RawLogEntry>, std::shared_ptr<SinkVector>>; //!< EntryPair a pair containing sink vector and log entry.
	public:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Destructor.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		~SinkSynchronizer();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Stores a raw log entry and its target sinks to be processed and forwarded to the sinks.
		///
		/// \param rawLogEntry The entry to be written.
		/// \param sinks The sinks to write the entry to.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void logToSinks(std::shared_ptr<RawLogEntry> rawLogEntry, std::shared_ptr<SinkVector> sinks);

	private:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructor.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		SinkSynchronizer();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Function used by the internal thread. It takes a stored Entry Pair, converts the raw log entry into a 
		///		   log entry and forwards it to the sinks.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void threadWorkFunction();

		std::queue<EntryPair> m_entryQueue; //!< The queue used to store new log entries and the sinks tehy should be logged to.
		std::mutex m_queueMutex; //!< The mutex used to protect the queue.
		std::atomic<bool> m_stopWorking = false; //!< The variable used to signal the thread that it should stop working.
		std::condition_variable m_workerCondition; //!< The condition variable used to resume the thread if new log entries are available.
		std::thread m_thread; //!< The thread used for the work function.
	};
}

#pragma warning( pop ) 