#pragma once

#include "LoggerAPI.h"

#include <chrono>
#include <string>

#pragma warning( push )
#pragma warning( disable : 4251 )

namespace Log
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \brief A timestamp with resolution down to microseconds.
	///        The smallest date supported is the 01.01.1970 00:00:00.000.000
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class LOGGER_API TimeStamp
	{
	public:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Default constructor fills the members with the date 01.01.1970 00:00:00.000.000
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		TimeStamp();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructor fills the members with values from the string.
		///
		/// \param day The day, 1-31, depending on month.
		/// \param month The month 1-12.
		/// \param year The year 1970-9999.
		/// \param hour The hour 0-23.
		/// \param minute The minute 0-59.
		/// \param second The second 0-60, supports leap second.
		/// \param millisecond The millisecond 0-999.
		/// \param microsecond The microsecond 0-999.
		///
		/// \exception std::invalid_argument Thrown if the resulting time stamp is an invalid point in time
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		TimeStamp(const int day, const int month, const int year, const int hour, const int minute, const int second, const int millisecond, const int microsecond);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructor fills the members with values based on the time point provided using UTC.
		///
		/// \param timeStamp The time point to use for filling in the data.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		TimeStamp(const std::chrono::time_point<std::chrono::system_clock>& timeStamp);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Constructor fills the members with values from the string.
		///
		/// \param timeStampString The string that contains the time stamp data.
		///
		/// \exception std::invalid_argument Thrown if the time stamp string does not match the expected format 
		///									 or if the resulting time is invalid.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		TimeStamp(const std::string& timeStampString);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Returns the day.
		///
		/// \returns The day.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const int getDay() const;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Returns the month.
		///
		/// \returns The month.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const int getMonth() const;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Returns the year.
		///
		/// \returns The year.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const int getYear() const;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Returns the hour.
		///
		/// \returns The hour.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const int getHour() const;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Returns the minute.
		///
		/// \returns The minute.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const int getMinute() const;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Returns the second.
		///
		/// \returns The second.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const int getSecond() const;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Returns the millisecond.
		///
		/// \returns The millisecond.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const int getMillisecond() const;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Returns the microsecond.
		///
		/// \returns The microsecond.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const int getMicrosecond() const;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Checks if the given string is a valid time stamp format. Does not perform a validation if
		///		   the resulting data is a valid point in time.
		///
		/// \param timeStampString The string that contains the time stamp data to check.
		///
		/// \returns True if the time stamp has a valid format, otherwise false.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static bool isStringValidTimeStampString(const std::string& timeStampString);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Converts the contents of all members into a printable time stamp string.
		///
		/// \returns The resulting time stamp string.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::string convertToString() const;

	private:
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \brief Checks if the data stored is a valid point in time.
		///
		/// \returns True if the time stamp is a valid point in time, otherwise false.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool isValidPointInTime() const;

		int m_day = 1; //!< Day
		int m_month = 1; //!< Month
		int m_year = 1970; //!< Year
		int m_hour = 0; //!< Hour
		int m_minute = 0; //!< Minute
		int m_second = 0; //!< Second
		int m_millisecond = 0; //!< Millisecond
		int m_microsecond = 0; //!< Microsecond
	};
}

#pragma warning( pop ) 