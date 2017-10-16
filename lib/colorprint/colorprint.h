#pragma once

// Colored debugging print functions
void hal_printf(char* format, ...);
void hal_printfError(const char* format, ...);
void hal_printfWarning(char* format, ...);
void hal_printfSuccess(char* format, ...);
void hal_printfInfo(char* format, ...);
