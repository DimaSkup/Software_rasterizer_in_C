// ==================================================================
// Filename:   log.c
// Desc:       implementation of logger
//
// Created:    25.06.2025
// ==================================================================
#include "log.h"
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>


// static pointer to the logger file
FILE* s_pLogFile = NULL;

// global string buffer
char g_String[512];

// static string buffer for log internal purposes
char s_StrTempBuf[512];


//---------------------------------------------------------
// Desc:   set console color to some particular
// Args:   - keyColor: key code to change color
//---------------------------------------------------------
void SetConsoleColor(const char* keyColor)
{
    printf("%s", keyColor);
}

//---------------------------------------------------------
// Desc:   a helper for printing messages into the console
//         and into the logger file
// Args:   - fileName:   path to the caller file
//         - funcName:   name of the caller function
//         - lvlText:    type of the log message
//         - text:       log message content
//         - codeLine:   line of code where logger was called
//---------------------------------------------------------
void PrintHelper(
    const char* fileName,
    const char* funcName,
    const char* lvlText,
    const char* text,
    const int codeLine)
{
    const char* fmt = "[%05ld] %s %s: %s() (line: %d): %s\n"; 
    time_t t = clock();

    printf(fmt, t, lvlText, fileName, funcName, codeLine, text);

    if (s_pLogFile)
        fprintf(s_pLogFile, fmt, t, lvlText, fileName, funcName, codeLine, text);
}

//---------------------------------------------------------
// Desc:   create logger file into which we will write messages
//---------------------------------------------------------
int InitLogger(void)
{
    s_pLogFile = fopen("soft_raster_log.txt", "w");

    if (s_pLogFile != NULL)
    {
        printf("the log file is created\n");

        time_t rawTime;
        struct tm* info = NULL;
        char buffer[80];

        time(&rawTime);
        info = localtime(&rawTime);
        strftime(buffer, 80, "%x -%I:%M%p", info);

        fprintf(s_pLogFile, "%s| the log file is created!\n", buffer);
        fprintf(s_pLogFile, "--------------------------\n\n");

        return 1;
    }
    else
    {
        printf("CRITICAL_ERROR: can't create a log file");
        return 0;
    }
}

//---------------------------------------------------------
// Desc:   print msg about closing of the log file and close it
//---------------------------------------------------------
void ShutdownLogger(void)
{
    time_t rawTime;
    struct tm* info = NULL;
    char buffer[80];

    time(&rawTime);
    info = localtime(&rawTime);
    strftime(buffer, 80, "%x -%I:%M%p", info);

    fprintf(s_pLogFile, "\n--------------------------------------\n");
    fprintf(s_pLogFile, "%s| this is the end, my only friend, the end\n", buffer);

    fclose(s_pLogFile);
}

//---------------------------------------------------------
// Desc:   print a usual message into console (text is green)
// Args:   - fileName:   path to the caller file
//         - funcName:   name of the caller function
//         - codeLine:   line of code where logger was called
//         - format:     format string for variadic arguments
//         - ...:        variadic arguments
//---------------------------------------------------------
void LogMsg(
    const char* fileName,
    const char* funcName,
    const int codeLine,
    const char* format,
    ...)
{
    va_list args;
    va_start(args, format);
     
    // reset the buffer
    memset(s_StrTempBuf, 0, 512);

    // make a string with input log-message
    vsprintf(s_StrTempBuf, format, args);

    // print a message into the console and log-file
    SetConsoleColor(GREEN);
    PrintHelper(fileName, funcName, "", s_StrTempBuf, codeLine);
    SetConsoleColor(RESET);

    va_end(args); 
}

//---------------------------------------------------------
// Desc:   print a debug message into console (text is white)
// Args:   - fileName:   path to the caller file
//         - funcName:   name of the caller function
//         - codeLine:   line of code where logger was called
//         - format:     format string for variadic arguments
//         - ...:        variadic arguments
//---------------------------------------------------------
void LogDbg(
    const char* fileName,
    const char* funcName,
    const int codeLine,
    const char* format,
    ...)
{
    va_list args;
    va_start(args, format);
     
    // reset the buffer
    memset(s_StrTempBuf, 0, 512);

    // make a string with input log-message
    vsprintf(s_StrTempBuf, format, args);

    // print a message into the console and log-file
    SetConsoleColor(RESET);
    PrintHelper(fileName, funcName, "DEBUG:", s_StrTempBuf, codeLine);

    va_end(args); 
}

//---------------------------------------------------------
// Desc:   print usual message into console (text is green)
// Args:   - fileName:   path to the caller file
//         - funcName:   name of the caller function
//         - codeLine:   line of code where logger was called
//         - format:     format string for variadic arguments
//         - ...:        variadic arguments
//---------------------------------------------------------
void LogErr(
    const char* fileName,
    const char* funcName,
    const int codeLine,
    const char* format,
    ...)
{
    va_list args;
    va_start(args, format);
     
    // reset the buffer
    memset(s_StrTempBuf, 0, 512);

    // make a string with input log-message
    vsprintf(s_StrTempBuf, format, args);

    // print a message into the console and log-file
    SetConsoleColor(RED);
    PrintHelper(fileName, funcName, "ERROR:", s_StrTempBuf, codeLine);
    SetConsoleColor(RESET);

    va_end(args); 
}


