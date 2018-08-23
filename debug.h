/*
Defines macros and the DEBUGGING flag for controlling whether detailed
debug information is generated.
*/

#ifndef __DEBUG_H__
#define __DEBUG_H__

// Genarete detailed debug information?
#define DEBUGGING 1

// Macro definitions
#if DEBUGGING
#define DEBUG(message) LOG("DBG", message)
#define DEBUG_VAL(message, value) LOG_VAL("DBG", message, value)
#else
#define DEBUG(message) ;
#define DEBUG_VAL(message, value) ;
#endif

#define INFO(message) LOG("INF", message)
#define INFO_VAL(message, value) LOG_VAL("INF", message, value)
#define ERROR(message) LOG("ERR", message)
#define ERROR_VAL(message, value) LOG_VAL("ERR", message, value)
#define LOG(type, message) Serial.print("["); Serial.print(type); Serial.print("] "); Serial.println(message);
#define LOG_VAL(type, message, value) Serial.print("["); Serial.print(type); Serial.print("] "); Serial.print(message); Serial.print(": "); Serial.println(value);

#endif
