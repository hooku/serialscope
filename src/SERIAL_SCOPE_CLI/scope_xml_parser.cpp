// scope_xml_parser.cpp : AIO-XML-Script parser
//
#include "stdafx.h" 

#ifdef WIN32
#include "scope_nt.h"
#else
#include "scope_linux.h"
#endif

#include <iostream>
#include <fstream>
#include <vector>

#include "Library/rapidxml.hpp"
#include "Library/rapidxml_print.hpp"
#include "Library/rapidxml_utils.hpp"

#include "scope_xml_parser.h"

rapidxml::xml_document  <>      *xmlDoc;
rapidxml::file          <>      *xmlFile;

// parse XML:
//
// Sec == Section
// Sen == Statement / Sentence
// Fun == Function
//
// 1. set the initial configure vars
// 2. verify the correctness of XML script
//
int cli_xml_parse_sect_config(SERIAL_INFO *portInfo)
{
    rapidxml::xml_node<> *secConfig = 
        xmlDoc->first_node(AIO_XML_NAME_ROOT)->first_node(AIO_XML_NAME_BLOCK_CONFIG);

    // check the existence of "CONFIG" node:
    if (!secConfig)
    {
        scope_print_error(NAME_NOTFOUND_CONFIG, xmlDoc->first_node(AIO_XML_NAME_ROOT)->name());
        return 0;
    }

    rapidxml::xml_attribute<> 
        *attrPort   = secConfig->first_attribute(AIO_XML_ATTR_CONFIG_PORT),
        *attrDefine = secConfig->first_attribute(AIO_XML_ATTR_CONFIG_DEFINE);

    if (attrPort)
    {
        strcpy_s(portInfo->portName, attrPort->value());
    }
    else
    {
        scope_print_error(ATTR_NOTFOUND_CONFIG_PORT, secConfig->name());
        return 0;
    }

    if (attrDefine)
    {
        strcpy_s(portInfo->portConfig, attrDefine->value());
    }
    else
    {
        scope_print_error(ATTR_NOTFOUND_CONFIG_DEFINE, secConfig->name());
        return 0;
    }

    return 1;
}

int cli_xml_parse_sect_event()
{
    rapidxml::xml_node<>*secEvents =
        xmlDoc->first_node(AIO_XML_NAME_ROOT)->first_node(AIO_XML_NAME_BLOCK_EVENT);

    // check the existence of "EVENTs" node:
    if (secEvents)
    {
        // check the existence of "id" attribute:
        for (rapidxml::xml_node<> *funEvent = secEvents->first_node();
            funEvent; funEvent = funEvent->next_sibling())
        {
            if (!funEvent->first_attribute(AIO_XML_ATTR_EVENT_ID))
            {
                scope_print_error(NAME_NOTFOUND_SCRIPT_ID, funEvent->name());
                return 0;
            }
        }
    }
    else
    {
        scope_print_error(NAME_NOTFOUND_EVENT, xmlDoc->first_node(AIO_XML_NAME_ROOT)->name());
        return 0;
    }

    return 1;
}

int cli_xml_parse_sect_script()
{
    rapidxml::xml_node<> *secScripts =
        xmlDoc->first_node(AIO_XML_NAME_ROOT)->first_node(AIO_XML_NAME_BLOCK_SCRIPT);

    // check the existence of "SCRIPT" node:
    if (secScripts)
    {
        // check the existence of "id" attribute:
        for (rapidxml::xml_node<> *senScript = secScripts->first_node();
            senScript; senScript = senScript->next_sibling())
        {
            if (!senScript->first_attribute(AIO_XML_ATTR_SCRIPT_ID))
            {
                scope_print_error(NAME_NOTFOUND_SCRIPT_ID, senScript->name());
                return 0;
            }
        }
    }
    else
    {
        scope_print_error(NAME_NOTFOUND_SCRIPT, xmlDoc->first_node(AIO_XML_NAME_ROOT)->name());
        return 0;
    }

    return 1;
}

int cli_xml_get_next_statement()
{

    return 0;
}

// call this func first to read file into XML doc
int cli_xml_load_file(char *xmlPath)
{
    xmlDoc  = new rapidxml::xml_document<>;
    xmlFile = new rapidxml::file<>(xmlPath);

    try
    {
        xmlDoc->parse<0>(xmlFile->data());
    }
    catch(rapidxml::parse_error &e)
    {
        scope_print_error(FILE_PARSE_ERROR, e.what());
        return 0;
    }

    // check root first
    if (!xmlDoc->first_node(AIO_XML_NAME_ROOT))
    {
        scope_print_error(NAME_NOTFOUND_ROOT, xmlDoc->first_node()->name());
        return 0;
    }

    // Insert something
    // rapidxml::xml_node<> *node = doc.allocate_node(rapidxml::node_element, "author", "John Doe");
    // doc.first_node()->append_node(node);

//  std::stringstream ss;
//  ss <<*doc.first_node();
//  std::string result_xml = ss.str();
//  std::cout <<result_xml<<std::endl;

    return 1;
}
