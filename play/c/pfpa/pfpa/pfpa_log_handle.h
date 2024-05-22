#ifndef PFPA_LOG_HANDLE_H_
#define PFPA_LOG_HANDLE_H_

#include "pfpa/pfpa_macro.h"

EXTERN_C_BEGIN

/**
 * @brief init log
 *
 * @param console_level  console log level
 * @param file_level     file log level
 * @param filepath       log filepath
 *
 * @return  boolean
 */
PFPA_EXPORT
bool pfpa_log_init(int console_level, int file_level, const char *filepath);

EXTERN_C_END

#endif // !PFPA_LOG_HANDLE_H_
