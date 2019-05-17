#include "TimeStamp.h"

#include <ctime>
#include <sstream>
#include <iomanip>
#include <regex>

namespace Log
{
	TimeStamp::TimeStamp()
	{}

	TimeStamp::TimeStamp(const int day, const int month, const int year, const int hour, const int minute, const int second, const int millisecond, const int microsecond)
		:m_day(day), m_month(month), m_year(year), m_hour(hour), m_minute(minute), m_second(second), m_millisecond(millisecond), m_microsecond(microsecond)
	{
		if (!isValidPointInTime())
		{
			throw std::invalid_argument("Invalid time point.");
		}
	}

	TimeStamp::TimeStamp(const std::chrono::time_point<std::chrono::system_clock>& timeStamp)
	{
		std::chrono::milliseconds millisecond = std::chrono::duration_cast<std::chrono::milliseconds>(timeStamp.time_since_epoch());
		std::chrono::microseconds microsecond = std::chrono::duration_cast<std::chrono::microseconds>(timeStamp.time_since_epoch());

		std::time_t time = std::chrono::duration_cast<std::chrono::seconds>(timeStamp.time_since_epoch()).count();

		std::tm t;
		gmtime_s(&t, &time);

		m_day = t.tm_mday;
		m_month = t.tm_mon + 1;
		m_year = t.tm_year + 1900;
		m_hour = t.tm_hour;
		m_minute = t.tm_min;
		m_second = t.tm_sec;
		m_millisecond = millisecond.count() % 1000;
		m_microsecond = microsecond.count() % 1000;
	}

	TimeStamp::TimeStamp(const std::string& timeStampString)
	{
		if (!isStringValidTimeStampString(timeStampString))
		{
			throw std::invalid_argument("Invalid time stamp string.");
		}

		//format is "00.00.0000 00:00:00.000.000" "day.month.year hour:minute:second.millisecond.microsecond"
		m_day = std::stoi(timeStampString.substr(0, 2));
		m_month = std::stoi(timeStampString.substr(3, 2));
		m_year = std::stoi(timeStampString.substr(6, 4));
		m_hour = std::stoi(timeStampString.substr(11, 2));
		m_minute = std::stoi(timeStampString.substr(14, 2));
		m_second = std::stoi(timeStampString.substr(17, 2));
		m_millisecond = std::stoi(timeStampString.substr(20, 3));
		m_microsecond = std::stoi(timeStampString.substr(24, 3));

		if (!isValidPointInTime())
		{
			throw std::invalid_argument("Invalid time stamp string.");
		}
	}

	const int TimeStamp::getDay() const
	{
		return m_day;
	}

	const int TimeStamp::getMonth() const
	{
		return m_month;
	}

	const int TimeStamp::getYear() const
	{
		return m_year;
	}

	const int TimeStamp::getHour() const
	{
		return m_hour;
	}

	const int TimeStamp::getMinute() const
	{
		return m_minute;
	}

	const int TimeStamp::getSecond() const
	{
		return m_second;
	}

	const int TimeStamp::getMillisecond() const
	{
		return m_millisecond;
	}

	const int TimeStamp::getMicrosecond() const
	{
		return m_microsecond;
	}

	bool TimeStamp::isStringValidTimeStampString(const std::string& timeStampString)
	{
		//format is "00.00.0000 00:00:00.000.000" "day.month.year hour:minute:second.millisecond.microsecond"
		std::regex timeStampRegex("^[0-9]{2}[.][0-9]{2}[.][0-9]{4}[ ][0-9]{2}[:][0-9]{2}[:][0-9]{2}[.][0-9]{3}[.][0-9]{3}$");
		std::smatch matches;

		std::regex_search(timeStampString, matches, timeStampRegex);

		return matches.size() == 1;		
	}

	std::string TimeStamp::convertToString() const
	{
		//format is "00.00.0000 00:00:00.000.000" "day.month.year hour:minute:second.millisecond.microsecond"
		std::ostringstream stream;
		stream << std::setfill('0') << std::setw(2) << m_day << ".";
		stream << std::setfill('0') << std::setw(2) << m_month << ".";
		stream << m_year << " ";
		stream << std::setfill('0') << std::setw(2) << m_hour << ":";
		stream << std::setfill('0') << std::setw(2) << m_minute << ":";
		stream << std::setfill('0') << std::setw(2) << m_second << ".";
		stream << std::setfill('0') << std::setw(3) << m_millisecond << ".";
		stream << std::setfill('0') << std::setw(3) << m_microsecond;
		return stream.str();
	}

	bool TimeStamp::isValidPointInTime() const
	{
		if (m_day <= 0)
		{
			return false;
		}

		if (m_month <= 0 || m_month > 12)
		{
			return false;
		}

		if (m_year < 1970 || m_year > 9999)
		{
			return false;
		}

		switch (m_month)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
		{
			if (m_day > 31)
			{
				return false;
			}
			break;
		}
		case 4:
		case 6:
		case 9:
		case 11:
		{
			if (m_day > 30)
			{
				return false;
			}
			break;
		}
		case 2:
		{
			if (m_year % 4 != 0 && m_day > 28)
			{
				return false;
			}
			else if (m_year % 4 == 0)
			{
				if (m_year % 100 == 0)
				{
					if (m_year % 400 == 0)
					{
						if (m_day > 29)
						{
							return false;
						}
					}
					else if (m_day > 28)
					{
						return false;
					}
				}
				else if (m_day > 29)
				{
					return false;
				}
			}
			break;
		}
		}

		if (m_hour < 0 || m_hour >= 24)
		{
			return false;
		}

		if (m_minute < 0 || m_minute >= 60)
		{
			return false;
		}

		// its > 60 instead of >= 60 because second 60 is used for leap seconds
		if (m_second < 0 || m_second > 60)
		{
			return false;
		}

		if (m_millisecond < 0 || m_millisecond >= 1000)
		{
			return false;
		}

		if (m_microsecond < 0 || m_microsecond >= 1000)
		{
			return false;
		}

		return true;
	}
}