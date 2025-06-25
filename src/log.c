// ==================================================================
// Filename:    log.c
// Description: implementation of logger
//
// Created:     07.04.2025 by DimaSkup
// ==================================================================
#include "log.h"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>

// static
FILE* s_pLogFile = NULL;


///////////////////////////////////////////////////////////

void SetConsoleColor(const char* keyColor)
{
    printf("%s", keyColor);
}

void ResetConsoleColor()
{
    printf("%s", RESET);
}

///////////////////////////////////////////////////////////

bool InitLogger(void)
{
    // init  the logger: create a logger file and print info about it

    if ((s_pLogFile = fopen("dude_engine_log.txt", "w")) != NULL)
    {
        printf(LOG_INFO, "the log file is created");
        puts("\n");

        time_t rawTime;
        struct tm *info;
        char buffer[80];

        time(&rawTime);
        info = localtime(&rawTime);

        strftime(buffer, 80, "%x - %I:%M%p", info);        

        fprintf(s_pLogFile, "%s| the log file is created!\n", buffer);
        fprintf(s_pLogFile, "---------------------------------\n\n");
        return true;
    }
    else
    {
        PrintHelper("CRITICAL_ERROR", "can't create a log file");
        return false;
    }    
}

///////////////////////////////////////////////////////////

void CloseLogger(void)
{
    // print message about closing of the log file and actually close it

    time_t rawTime;
    struct tm *info;
    char buffer[80];

    time(&rawTime);
    info = localtime(&rawTime);

    strftime(buffer, 80, "%x - %I:%M%p", info);        

    fprintf(s_pLogFile, "\n------------------------------------------------\n");
    fprintf(s_pLogFile, "%s| the end of the log file\n", buffer);

    fclose(s_pLogFile);
}

///////////////////////////////////////////////////////////

void LogPrint(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char buffer[256];
    memset(buffer, 0, 256);

    vsprintf(buffer, format, args);
    
    SetConsoleColor(GREEN);
    PrintHelper("", buffer);
    ResetConsoleColor();

    va_end(args);
}

///////////////////////////////////////////////////////////

void LogDebug(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char buffer[256];
    memset(buffer, 0, 256);

    vsprintf(buffer, format, args);
    PrintHelper("DEBUG: ", buffer);

    va_end(args);
}

///////////////////////////////////////////////////////////

void LogError(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char buffer[256];
    memset(buffer, 0, 256);

    vsprintf(buffer, format, args);

    SetConsoleColor(BOLDRED);
    PrintHelper("ERROR: ", buffer);
    ResetConsoleColor();

    va_end(args);
}

///////////////////////////////////////////////////////////

void PrintHelper(const char* levText, const char* text)
{
    // a helper for printing messages into the console 
    // and into the logger text file

    char buffer[256];
    memset(buffer, 0, 256);

    sprintf(buffer, "[%05ld]  %s%s", clock(), levText, text);
    printf("%s\n", buffer);

    if (s_pLogFile)
        fprintf(s_pLogFile, "%s\n", buffer);
}


