#pragma once


typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
typedef unsigned __int64 qword;

#pragma warning (push)
#pragma warning (disable : 4035)
inline __int64 _rdtsc() { __asm { rdtsc } }
#pragma warning(pop)

