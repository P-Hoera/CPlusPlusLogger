#pragma once

#include "LoggerAPI.h"

#include "SinkBase.h"

#include <fstream>

#pragma warning( push )
#pragma warning( disable : 4251 )

namespace Log
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \brief Base class of all sinks targeting a single log file. Must be initialized in order to sink incomming 
	///		   messages. Can be initialized as a dummy in case errors prevent a normal initialization. 
	///        Initialization of any kind should only be delayed as much as possible to prevent high memory consumtion.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class LOGGER_API FileSinkBase : public SinkBase
	{
	public:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Destructor.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual ~FileSinkBase();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Returns the name of the file used.
		///
		/// \returns If a file is opened the file name, otherwise empty.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const std::string& getFileName() const;

	protected:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructor.
		///
		/// \param sinkName A name for the sink that can be used to identify it.
		/// \param sinkType The type of the sink.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		FileSinkBase(const std::string& sinkName, const SinkType sinkType);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Writes a stored log entry to the file. It is only called after initialization or initialization 
		///		   as dummy was successful.
		///
		/// \param logEntry The log entry to be written.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual void write(const std::shared_ptr<LogEntry> logEntry) final;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Initializes the sink and opens the file. Does not do any initialization if the class is already initialized.
		///
		/// \param fileName The file to be used as sink target.
		///
		/// \exception std::invalid_argument Thrown if the file cannot be opened for writing.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void initialize(const std::string& fileName);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Initializes the sink as dummy. Does not do any initialization if the class is already initialized.
		///		   Initializing as dummy discards all log entries.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void initializeAsDummy();

	private:
		std::string m_fileName; //!< The name of the file used as sink target.
		std::ofstream m_file; //!< The file used as sink target.
	};
}

#pragma warning( pop ) 