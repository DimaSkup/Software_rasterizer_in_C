// ==================================================================
// Filename:    log.h
// Description: just logger :)
//
// Created:     07.04.2025 by DimaSkup
// ==================================================================
#ifndef LOG_H
#define LOG_H

#include <stdbool.h>

// define for standard log message (info about caller: file_name, func_name, code_line, message)
#define LOG_INFO "%s: %s() (line: %d): %s\n",__FILE__, __func__, __LINE__

bool InitLogger(void);
void CloseLogger(void);

void SetConsoleColor();
void ResetConsoleColor();

void PrintHelper(const char* levText, const char* text);
void LogPrint(const char* format, ...);

#endif
