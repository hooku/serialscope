// scope_conf.h : Configs & params for AIO-Play
//
#pragma once

#define MAX_GENERIC_LENGTH      250
#define MAX_AIO_PACKET          1560

/* STRUCT */
struct SERIAL_INFO
{
    char deviceName [MAX_GENERIC_LENGTH];       // \Device\Serial0  *unused in XML parse
    char portName   [MAX_GENERIC_LENGTH];       // COM1
    char portConfig [MAX_GENERIC_LENGTH];       // baud=115200 parity=N data=8 stop=1
};
