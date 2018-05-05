#pragma once
#ifdef _MSC_VER
#pragma comment(lib,"ws2_32.lib")                                               // Platform of MSVC 
#pragma comment( lib,"winmm.lib" )

#include <Windows.h>															//header 
#include <WinSock.h>
#include <strsafe.h>
#include <Mmsystem.h>
#include <assert.h>

#include "../../../typedef/typedef.h"
#endif

#ifdef __GNUC__																	//GNU C
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#endif


#ifndef BOOL
#define BOOL int
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif
