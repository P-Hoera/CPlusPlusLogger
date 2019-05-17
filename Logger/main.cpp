#include <windows.h>

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpReserved)  // reserved
{
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		break;
	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;
	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;
	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

/*

#include <iostream>

#include "LoggerFactory.h"
#include "SinkFactory.h"


void main()
{
	std::shared_ptr<Log::Logger> logger = Log::LoggerFactory::createLogger("TestLogger", "Test Module", Log::LogLevel::Info);

	std::shared_ptr<Log::LogViewerRPCSink> sink = Log::SinkFactory::createLogViewerRPCSink("TestSink");

	logger->connectToSink(sink);

	logger->logInfo("Test1");
	logger->logInfo("Test2");
	logger->logInfo("Test3");

	std::cout << std::boolalpha << logger->isConnectedToSink(sink) << std::endl;

	logger->disconnectFromSink(sink);

	std::cout << logger->isConnectedToSink(sink) << std::endl;

	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	//sink->initialize();

	std::cin.get();

	sink.reset();
}
*/