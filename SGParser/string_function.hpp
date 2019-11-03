#pragma once

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

inline uint32_t len(char* str1)     { return strlen(str1); }
inline uint32_t len(const char* str1)     { return strlen(str1); }
inline uint32_t len(wchar_t* wstr1) { return wcslen(wstr1); }
inline uint32_t len(const wchar_t* wstr1) { return wcslen(wstr1); }

inline void cast_char(char* str1, const wchar_t* wstr2, uint32_t max) {	wcstombs(str1, wstr2, max); }
inline void cast_char(wchar_t* wstr1, const char* str2, uint32_t max) {	mbstowcs(wstr1, str2, max); }

inline void cast_char(char* str1, const wchar_t* wstr2) { const uint32_t len_str = len(wstr2); cast_char(str1, wstr2, len_str);  str1[len_str]  = 0x00; }
inline void cast_char(wchar_t* wstr1, const char* str2) { const uint32_t len_str = len(str2);  cast_char(wstr1, str2, len_str);  wstr1[len_str] = 0x00; }

inline void cpy(wchar_t* wstr1, const char*    str2)    { cast_char(wstr1, str2); }
inline void cpy(char*    str1,  const wchar_t* wstr2)   { cast_char(str1,  wstr2);}
inline void cpy(char*    str1, char*    str2)           { strcpy(str1, str2);     }
inline void cpy(wchar_t* wstr1, const wchar_t* wstr2)   { wcscpy(wstr1, wstr2);   }
inline void cat(char* str1,     const wchar_t* wstr2)       { cpy(str1  + len(str1),  wstr2);  }
inline void cat(char*    str1, char*    str2)           { cpy(str1  + len(str1), str2);    }
inline void cat(wchar_t* wstr1, const char* str2)       { cpy(wstr1 + len(wstr1), str2);   }
inline void cat(wchar_t* wstr1, const wchar_t* wstr2)   { cpy(wstr1 + len(wstr1), wstr2);  }