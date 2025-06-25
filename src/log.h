// ==================================================================
// Filename:    log.h
// Description: just logger :)
//
// Created:     07.04.2025 by DimaSkup
// ==================================================================
#ifndef LOG_H
#define LOG_H

#include <stdbool.h>

// defines to set console color
#define RESET       "\033[0m"
#define BLACK       "\033[30m"              /* Black */
#define RED         "\033[31m"              /* Red */
#define GREEN       "\033[32m"              /* Green */
#define YELLOW      "\033[33m"              /* Yellow */
#define BLUE        "\033[34m"              /* Blue */
#define MAGENTA     "\033[35m"              /* Magenta */
#define CYAN        "\033[36m"              /* Cyan */
#define WHITE       "\033[37m"              /* White */
#define BOLDBLACK   "\033[1m\033[30m"       /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"       /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"       /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"       /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"       /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"       /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"       /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"       /* Bold White */


// define for standard log message (info about caller: file_name, func_name, code_line, message)
#define LOG_INFO "%s: %s() (line: %d): %s",__FILE__, __func__, __LINE__

bool InitLogger(void);
void CloseLogger(void);

void SetConsoleColor(const char* keyColor);
void ResetConsoleColor();

void PrintHelper(const char* levText, const char* text);
void LogPrint(const char* format, ...);
void LogDebug(const char* format, ...);
void LogError(const char* format, ...);

#endif
