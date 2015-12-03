#ifndef GLDSYSTEM
#define GLDSYSTEM

#ifdef __APPLE__
#pragma clang diagnostic ignored "-Winvalid-source-encoding"
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif

#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(__APPLE__) || defined(__x86_64__)
typedef long long PtrInt;
typedef unsigned long long PtrUInt;
#else
typedef int PtrInt;
typedef unsigned int PtrUInt;
#endif

typedef long long gint64;
typedef unsigned long long guint64;
typedef unsigned char byte;
typedef unsigned int GRgb;
typedef unsigned int TColor;

#endif // GLDSYSTEM

