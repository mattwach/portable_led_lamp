// Debugging routines

#ifndef DEBUG_H
#define DEBUG_H

// Uncomment to enable debugging
//#define DEBUG
void DEBUG_INIT();

#ifdef DEBUG
  #include <uart/uart.h>
  #define DEBUG_STR(msg) uart_str(msg)
  #define DEBUG_STRLN(msg) uart_strln(msg)
  #define DEBUG_PSTR(msg) uart_pstr(msg)
  #define DEBUG_PSTRLN(msg) uart_pstrln(msg)
#else
  #define DEBUG_STR(msg)
  #define DEBUG_STRLN(msg)
  #define DEBUG_PSTR(msg)
  #define DEBUG_PSTRLN(msg)
#endif
#endif
