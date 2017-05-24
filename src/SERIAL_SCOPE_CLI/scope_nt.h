// scope_nt.h : Windows NT api call predefinition & configures
//
#pragma once

#include "stdafx.h"
#include "scope_conf.h"

/* MACRO */
// windows registry:
#define MAX_KEY_LENGTH      255
#define MAX_VALUE_NAME      16383

// app:
#define APP_VER             1
//#define   COM_PORT_NUMBER     _T(COM3")
#define MAX_PORT            8           // COM1 - COM8

// COM port: 57,600 bps, 8 data bits, no parity, and 1 stop bit
#define COM_CONFIG          _T("baud=115200 parity=N data=8 stop=1")
#define COM_CHECK_TIMEOUT   500         // interval between every COM port pull

// ReadTotalTimeoutConstant + ReadTotalTimeoutMultiplier*AIO_MAX_PACKET(1560)
#define READ_CONST          150         // read interval (in ms) -> 2048B / 115200bps   = 0.15  sec
#define READ_MULTI          0           // read multiply (bytes) -> 6.94e-5sec          = 0     sec

#define MAX_READ_BUFFER     2048        // 1560
#define MAX_WRITE_BUFFER    2048        // 1560

extern HANDLE hAppExitEvent;            // flag control app exit action

// *** Temp ***
extern BYTE readBuffer[MAX_READ_BUFFER];

/* PREDEFINE */

// scope_nt_io.cpp:
int scope_get_avail_com_port    (SERIAL_INFO *, int *);
int scope_open_com_port         (char *);
int scope_close_com_port        ();
int scope_read_port             (unsigned char *);
int scope_write_port            (unsigned char *, DWORD );
int scope_create_rw_threads     ();

// scope_nt_misc.cpp:
void scope_print_error              ();
void scope_print_error              (const char *);
void scope_print_error              (int , const char *);

void *scope_create_event            (bool, bool);
bool scope_set_event                (void *);
bool scope_reset_event              (void *);
unsigned long scope_get_last_error  ();
