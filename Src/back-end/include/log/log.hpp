#ifndef LOG_HPP
#define LOG_HPP

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define _IMG_BACKGROUND  // if you want to see your image on background

#ifdef _IMG_BACKGROUND
    #define ON_IMG(...) __VA_ARGS__
    #define OFF_IMG(...)

    #define ON_GRADIENT(...) 
    #define OFF_GRADIENT(...)

#else
    #define ON_IMG(...)
    #define OFF_IMG(...) __VA_ARGS__

    #define _GRADIENT  // if you want to see color gradient on background
 
    #ifdef _GRADIENT
        #define ON_GRADIENT(...) __VA_ARGS__
        #define OFF_GRADIENT(...)
        
    #else
        #define ON_GRADIENT(...)
        #define OFF_GRADIENT(...) __VA_ARGS__

    #endif
    
#endif

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum LogColor : char
{
    White ,
    Red   ,
    Green ,
    Pink  ,
    Yellow,
    Black ,
    Blue  ,
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void OpenLog();
void CloseLog();
void LogAdcPrint(const char* format, ...);
void LogTextColor(LogColor color);
void LogTextColorEnd();

void LogPrint(LogColor color, const char* format, ...);

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define LOG_OPEN()  OpenLog()
#define LOG_CLOSE() CloseLog()

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define LOG_COLOR(color)      LogTextColor (color)
#define LOG_ADC_PRINT(...)    LogAdcPrint  (       __VA_ARGS__)
#define LOG_PRINT(color, ...) LogPrint     (color, __VA_ARGS__)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define LOG_PLACE(color)                                LogPrint(color, "file [%s]\nline [%d]\nfunc [%s]\n",       __FILE__,       __LINE__,      __func__ )
#define LOG_PRINT_PLACE(color, file, line, func)        LogPrint(color, "file [%s]\nline [%d]\nfunc [%s]\n",         file  ,         line  ,        func   )
#define LOG_PRINT_STRUCT_PLACE(color, file, line, func) LogPrint(color, "file [%s]\nline [%d]\nfunc [%s]\n", (place).file  , (place).line  , (place).func  )

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define LOG_ERROR(msg)   LogPrint(Red,    "Error: %s\n"  , msg)
#define LOG_WARNING(msg) LogPrint(Yellow, "Warning: %s\n", msg)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define LOG_ALL_STRING_ARRAY(color, array, size) do                                                                      \
{                                                                                                                         \
    LogTextColor(color);                                                                                                   \
    for (size_t log_string_all_array_iterator = 0; log_string_all_array_iterator < size; log_string_all_array_iterator++)   \
    {                                                                                                                        \
        LogAdcPrint(#array "[%2lu] = '%s'\n", log_string_all_array_iterator, array[log_string_all_array_iterator]);           \
    }                                                                                                                          \
    LogTextColorEnd();                                                                                                          \
} while (0)                                                                                                                      \

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define LOG_ALL_CHAR_ARRAY(color, array, size) do                                                                        \
{                                                                                                                         \
    LogTextColor(color);                                                                                                   \
    for (size_t log_char_all_array_iterator = 0; log_char_all_array_iterator < size; log_char_all_array_iterator++)         \
    {                                                                                                                        \
        LogAdcPrint(#array "[%2lu] = '%c'\n", log_char_all_array_iterator, array[log_char_all_array_iterator]);               \
    }                                                                                                                          \
    LogTextColorEnd();                                                                                                          \
} while (0)                                                                                                                      \

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define LOG_ALL_DOUBLE_ARRAY(color, array, size) do                                                                      \
{                                                                                                                         \
    LogTextColor(color);                                                                                                   \
    for (size_t log_double_all_array_iterator = 0; log_double_all_array_iterator < size; log_double_all_array_iterator++)   \
    {                                                                                                                        \
        LogAdcPrint(#array "[%2lu] = '%f'\n", log_double_all_array_iterator, array[log_double_all_array_iterator]);           \
    }                                                                                                                          \
    LogTextColorEnd();                                                                                                          \
} while (0)                                                                                                                      \

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define LOG_ALL_INT_ARRAY(color, array, size) do                                                                         \
{                                                                                                                         \
    LogTextColor(color);                                                                                                   \
    for (size_t log_int_all_array_iterator = 0; log_int_all_array_iterator < size; log_int_all_array_iterator++)            \
    {                                                                                                                        \
        LogAdcPrint(#array "[%2lu] = '%d'\n", log_int_all_array_iterator, array[log_int_all_array_iterator]);                 \
    }                                                                                                                          \
    LogTextColorEnd();                                                                                                          \
} while (0)                                                                                                                      \

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define LOG_NS() LogPrint(LogColor::White, "\n")
#define LOG_TB() LogPrint(LogColor::White, "\t")
#define LOG_SP() LogPrint(LogColor::White, " ")
#define LOG_N_SP(n) do { for (size_t log_n_spaces_counter = 0; log_n_spaces_counter < n; log_n_spaces_counter++) LogPrint(LogColor::White, " "); } while (0)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif //LOG_HPP