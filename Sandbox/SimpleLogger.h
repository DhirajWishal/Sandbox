#pragma once

#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <inttypes.h>

/**
 * Formatting command is set using the # tag. This tag can include,
 * * 0: 8 bit integers.
 * * 1: 16 bit integers.
 * * 2: 32 bit integers.
 * * 3: 64 bit integers.
 * * 4: string.
 * * 5: pointer.
 *
 * Example: LogError("My age is #0.", 19);
 */

#define SL_COLOR_BLUE			{ 0x1b, '[', '1', ';', '3', '4', 'm', 0 }
#define SL_COLOR_GREEN			{ 0x1b, '[', '1', ';', '9', '2', 'm', 0 }
#define SL_COLOR_YELLOW			{ 0x1b, '[', '1', ';', '9', '3', 'm', 0 }
#define SL_COLOR_RED			{ 0x1b, '[', '1', ';', '3', '1', 'm', 0 }
#define SL_COLOR_DARK_RED		{ 0x1b, '[', '4', ';', '3', '1', 'm', 0 }
#define SL_COLOR_DEFAULT		{ 0x1b, '[', '0', ';', '3', '9', 'm', 0 }

#define SL_WIDE_CHAR

namespace SimpleLogger {
	/**
	 * Switch from one color to another.
	 * Once switched, make sure to color it back to default or else the console will be colored to that specific color.
	 *
	 * @param color: The 8 character color code.
	 */
	void SwitchColor(const char(&color)[8]) { printf("%s", color); }

	/**
	 * Print the current time to the console.
	 */
	void PrintTime_C()
	{
		_tzset();
		char pBuffer[12];
		_strtime_s(pBuffer);
		printf("{ %s } ", pBuffer);
	}

	/**
	 * Print the log string.
	 *
	 * @param format: The string to print with or without formatting.
	 * @param VA_ARGS: The arguments for formatting.
	 */
	void Print(const char* format, ...)
	{
		va_list args = {};
		va_start(args, format);

		while (*format != '\0')
		{
			if (*format == '#')
			{
				++format;
				switch (*format)
				{
				case '0': printf("%d", va_arg(args, int32_t)); break;
				case '1': printf("%d", va_arg(args, int32_t)); break;
				case '2': printf("%d", va_arg(args, int32_t)); break;
				case '3': printf("%lld", va_arg(args, int64_t)); break;
				case '4': printf("%s", va_arg(args, const char*)); break;
				case '5': printf("%p", va_arg(args, void*)); break;
				default: printf("%c", *format); break;
				}
			}
			else
				printf("%c", *format);

			++format;
		}
	}

	/**
	 * Log debug information.
	 * Color: Blue.
	 *
	 * @param format: The string to be printed with or without formatting.
	 * @param args: The arguments to be formatted.
	 */
	template<class... Args>
	void LogDebug(const char* format, Args&&... args)
	{
		SwitchColor(SL_COLOR_BLUE);
		PrintTime_C();
		Print(format, args...);
		SwitchColor(SL_COLOR_DEFAULT);
	}

	/**
	 * Log information.
	 * Color: Green.
	 *
	 * @param format: The string to be printed with or without formatting.
	 * @param args: The arguments to be formatted.
	 */
	template<class... Args>
	void LogInfo(const char* format, Args&&... args)
	{
		SwitchColor(SL_COLOR_GREEN);
		PrintTime_C();
		Print(format, args...);
		SwitchColor(SL_COLOR_DEFAULT);
	}

	/**
	 * Log warning information.
	 * Color: Yellow.
	 *
	 * @param format: The string to be printed with or without formatting.
	 * @param args: The arguments to be formatted.
	 */
	template<class... Args>
	void LogWarning(const char* format, Args&&... args)
	{
		SwitchColor(SL_COLOR_YELLOW);
		PrintTime_C();
		Print(format, args...);
		SwitchColor(SL_COLOR_DEFAULT);
	}

	/**
	 * Log error information.
	 * Color: Red.
	 *
	 * @param format: The string to be printed with or without formatting.
	 * @param args: The arguments to be formatted.
	 */
	template<class... Args>
	void LogError(const char* format, Args&&... args)
	{
		SwitchColor(SL_COLOR_RED);
		PrintTime_C();
		Print(format, args...);
		SwitchColor(SL_COLOR_DEFAULT);
	}

	/**
	 * Log fatal information.
	 * Color: Dark Red.
	 *
	 * @param format: The string to be printed with or without formatting.
	 * @param args: The arguments to be formatted.
	 */
	template<class... Args>
	void LogFatal(const char* format, Args&&... args)
	{
		SwitchColor(SL_COLOR_DARK_RED);
		PrintTime_C();
		Print(format, args...);
		SwitchColor(SL_COLOR_DEFAULT);
	}

#ifdef SL_WIDE_CHAR
	/**
	 * Switch from one color to another.
	 * Once switched, make sure to color it back to default or else the console will be colored to that specific color.
	 *
	 * @param color: The 8 character color code.
	 */
	void SwitchColor(const wchar_t(&color)[8]) { wprintf(L"%s", color); }

	/**
	 * Print the current time to the console.
	 */
	void PrintTime_W()
	{
		_tzset();
		wchar_t pBuffer[12];
		_wstrtime_s(pBuffer);
		wprintf(L"{ %s } ", pBuffer);
	}

	/**
	 * Print the log string.
	 *
	 * @param format: The string to print with or without formatting.
	 * @param VA_ARGS: The arguments for formatting.
	 */
	void Print(const wchar_t* format, ...)
	{
		va_list args = {};
		va_start(args, format);

		while (*format != '\0')
		{
			if (*format == '#')
			{
				++format;
				switch (*format)
				{
				case '0': wprintf(L"%d", va_arg(args, int32_t)); break;
				case '1': wprintf(L"%d", va_arg(args, int32_t)); break;
				case '2': wprintf(L"%d", va_arg(args, int32_t)); break;
				case '3': wprintf(L"%lld", va_arg(args, int64_t)); break;
				case '4': wprintf(L"%s", va_arg(args, const wchar_t*)); break;
				case '5': wprintf(L"%p", va_arg(args, void*)); break;
				default: wprintf(L"%c", *format); break;
				}
			}
			else
				printf("%c", *format);

			++format;
		}
	}

	/**
	 * Log debug information.
	 * Color: Blue.
	 *
	 * @param format: The string to be printed with or without formatting.
	 * @param args: The arguments to be formatted.
	 */
	template<class... Args>
	void LogDebug(const wchar_t* format, Args&&... args)
	{
		SwitchColor(SL_COLOR_BLUE);
		PrintTime_W();
		Print(format, args...);
		SwitchColor(SL_COLOR_DEFAULT);
	}

	/**
	 * Log information.
	 * Color: Green.
	 *
	 * @param format: The string to be printed with or without formatting.
	 * @param args: The arguments to be formatted.
	 */
	template<class... Args>
	void LogInfo(const wchar_t* format, Args&&... args)
	{
		SwitchColor(SL_COLOR_GREEN);
		PrintTime_W();
		Print(format, args...);
		SwitchColor(SL_COLOR_DEFAULT);
	}

	/**
	 * Log warning information.
	 * Color: Yellow.
	 *
	 * @param format: The string to be printed with or without formatting.
	 * @param args: The arguments to be formatted.
	 */
	template<class... Args>
	void LogWarning(const wchar_t* format, Args&&... args)
	{
		SwitchColor(SL_COLOR_YELLOW);
		PrintTime_W();
		Print(format, args...);
		SwitchColor(SL_COLOR_DEFAULT);
	}

	/**
	 * Log error information.
	 * Color: Red.
	 *
	 * @param format: The string to be printed with or without formatting.
	 * @param args: The arguments to be formatted.
	 */
	template<class... Args>
	void LogError(const wchar_t* format, Args&&... args)
	{
		SwitchColor(SL_COLOR_RED);
		PrintTime_W();
		Print(format, args...);
		SwitchColor(SL_COLOR_DEFAULT);
	}

	/**
	 * Log fatal information.
	 * Color: Dark Red.
	 *
	 * @param format: The string to be printed with or without formatting.
	 * @param args: The arguments to be formatted.
	 */
	template<class... Args>
	void LogFatal(const wchar_t* format, Args&&... args)
	{
		SwitchColor(SL_COLOR_DARK_RED);
		PrintTime_W();
		Print(format, args...);
		SwitchColor(SL_COLOR_DEFAULT);
	}

#endif // SL_WIDE_CHAR
}