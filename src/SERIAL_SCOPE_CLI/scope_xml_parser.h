// scope_xml_parser.h : AIO-XML-Script parser predefinition & configures
//
#pragma once

#include "scope_conf.h"

// NODE NAME
#define AIO_XML_NAME_ROOT               "AIO_SCRIPT"

#define AIO_XML_NAME_BLOCK_CONFIG       "CONFIG"
#define AIO_XML_NAME_BLOCK_SCRIPT       "MSGs"
#define AIO_XML_NAME_BLOCK_EVENT        "EVENTs"

#define AIO_XML_NAME_SCRIPT             "MSG"
#define AIO_XML_NAME_EVENT              "EVENT"

// ATTRIBUTE NAME
#define AIO_XML_ATTR_WATCH                  "watch"
#define AIO_XML_ATTR_WAIT                   "wait"

#define AIO_XML_ATTR_CONFIG_PORT            "port"
#define AIO_XML_ATTR_CONFIG_DEFINE          "def"
#define AIO_XML_ATTR_SCRIPT_ID              "id"
#define AIO_XML_ATTR_SCRIPT_TIMEOUT         "timeout"
#define AIO_XML_ATTR_SCRIPT_RETRY           "retry"
#define AIO_XML_ATTR_SCRIPT_RETRY_DELAY     "retrydelay"
#define AIO_XML_ATTR_SCRIPT_ON_ERROR        "onerror"
#define AIO_XML_ATTR_EVENT_ID               "id"
#define AIO_XML_ATTR_EVENT_IND_ID           "id"
#define AIO_XML_ATTR_EVENT_IND_STATE_ID     "id"


int cli_xml_parse_sect_config       (SERIAL_INFO *portInfo);
int cli_xml_parse_sect_event        ();
int cli_xml_parse_sect_script       ();
int cli_xml_load_file               (char *);

// ERROR NUMBER
enum AIO_XML_ERROR
{ // Success == 1
    FILE_PARSE_ERROR                    = 0x2   ,

    NAME_NOTFOUND_ROOT                  = 0x10  ,

    NAME_NOTFOUND_CONFIG                = 0x30  ,
    ATTR_NOTFOUND_CONFIG_PORT                   ,
    ATTR_NOTFOUND_CONFIG_DEFINE                 ,
    
    NAME_NOTFOUND_SCRIPT                = 0x60  ,
    NAME_NOTFOUND_SCRIPT_ID                     ,

    NAME_NOTFOUND_EVENT                 = 0x60  ,
    NAME_NOTFOUND_EVENT_ID
};
