#include "ProcessInfo.h"

#include <Windows.h>

namespace Log
{
	ProcessInfo::ProcessInfo()
	{
		const int pathMaxLenth = 15000;
		char fullPath[pathMaxLenth];
		GetModuleFileName(NULL, fullPath, pathMaxLenth);
		std::string fullPathString(fullPath);
		m_processFileName = fullPathString.substr(fullPathString.find_last_of('\\') + 1);

		m_processID = GetCurrentProcessId();
	}

	ProcessInfo::~ProcessInfo()
	{}

	const std::string& ProcessInfo::getProcessFileName() const
	{
		return m_processFileName;
	}

	const unsigned long ProcessInfo::getProcessID() const
	{
		return m_processID;
	}
}