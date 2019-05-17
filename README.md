# CPlusPlusLogger
Is a small logger Framework for C++ and Windows.

It contains 3 Parts.


The Logger dll:

This contains all parts of the logger Framework.
To get a Logger use the Log::LoggerFactory. Loggers use a log level with their log functions. They also contain logAlways functions that ignore the current log level. These are meant for cases where a message has to be logged that does not fit the current log level, like successful software starts. All log and logAlways functions have 2 options: 1.) Log a string, 2.) call a lambda or fucntion. The latter one only calls the lambda/function if the log level is met and used the calling thread.
To get a Sink use the Log::SinkFactory and connect them with the Logger afterwards. All Sinks come in 2 forms. Normal ones are initialized on construction and forward all log entries immediately. Delayed ones store the incomming messages until tehy are initialized. They also have the option to be turned into dummys and ignore all messages for cases where an initialization is not possible. The delayed ones are useful for cases where file names or other information is not available during start up, but should be used with care as tehy use up more memory as long as they are uninitialized. There are also 2 sets of RPC Sinks that connect to the LogViewer to allow logs to be shown and filtered while the app is running. The local one connects only to LogViewer on the local PC, while the network one can connect via networks. However networks should be used with caution as the packet runtime drops the maximum amount of messages the sink can pass on.


The LogViewer:

This is a GUI based Analysis tool that can load and filter log files created by the Logger and is the target for RPC connections from RPC sinks.


The TestClient:

This is a small console application that sends a total of 100k entries randomly to 10 loggers used by 5 threads with random log levels using logAlways. The targets are file, local RPC and network RPC. Its only for testing purposes to the LogViewer.
