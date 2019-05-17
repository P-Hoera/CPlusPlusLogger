#pragma once

#ifndef LOGGER_NO_DLL
#ifdef LOGGER_DLL
#define LOGGER_API __declspec(dllexport)
#else
#define LOGGER_API __declspec(dllimport)
#endif
#else
#define LOGGER_API
#endif