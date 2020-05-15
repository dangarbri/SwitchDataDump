#pragma once


#include <switch.h>

/**
 * Print a human readable error message to console
 * given the error result
 *
 * Note requires #define ENABLE_ERROR_CODE_PRINTS
 * @param[in] rc - Error result
 */
void error_code_print(Result rc, const char* file, int line);

/**
 * Macro for adding file and line to the error code message
 */
#define ERROR_RESULT_PRINT(rc) error_code_print((rc), __FILE__, __LINE__)

/**
 * Print error message if result is a failure code
 */
#define CHECK_RESULT(rc)          \
    if (R_FAILED((rc)))           \
    {                             \
        ERROR_RESULT_PRINT((rc)); \
    }
