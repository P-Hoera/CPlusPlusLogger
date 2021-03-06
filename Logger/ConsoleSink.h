#pragma once

#include "LoggerAPI.h"

#include "ConsoleSinkBase.h"

#pragma warning( push )
#pragma warning( disable : 4251 )

namespace Log
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \brief Sink targeting the default Console. Is fully initialized upon construction.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class LOGGER_API ConsoleSink final : public ConsoleSinkBase
	{
		friend class SinkFactory; //!< Friend to allow construction only from SinkFactory.
	public:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Destructor.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual ~ConsoleSink();

	private:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructor.
		///
		/// \param sinkName A name for the sink that can be used to identify it.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		ConsoleSink(const std::string& sinkName);
	};
}

#pragma warning( pop ) 