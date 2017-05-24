// scope_ui_xml_parser.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "scope_ui_main.h"
#include "scope_ui_xml_parser.h"

#include <atlbase.h>
#include <xmllite.h>

//byte xmlScriptRichBuffer[MAX_XML_SCRIPT_RICH_BUFFER];

HRESULT WriteAttributes(IXmlReader* pReader)
{
    const WCHAR* pwszPrefix;
    const WCHAR* pwszLocalName;
    const WCHAR* pwszValue;
    HRESULT hr = pReader->MoveToFirstAttribute();

    if (S_FALSE == hr)
        return hr;
    if (S_OK != hr)
    {
        scope_print_error(hr, _T("moving to first attribute failed"));
        return -1;
    }
    else
    {
        while (TRUE)
        {
            if (!pReader->IsDefault())
            {
                UINT cwchPrefix;
                if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
                {
                    scope_print_error(hr, _T("getting prefix failed"));
                    return -1;
                }

                if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
                {
                    scope_print_error(hr, _T("getting local name failed"));
                    return -1;
                }

                if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
                {
                    scope_print_error(hr, _T("getting value failed"));
                    return -1;
                }

                if (cwchPrefix > 0)
                    scope_print(_T("Attr: %s:%s=\"%s\" \n"), pwszPrefix, pwszLocalName, pwszValue);
                else
                    scope_print(_T("Attr: %s=\"%s\" \n"), pwszLocalName, pwszValue);
            }

            if (S_OK != pReader->MoveToNextAttribute())
                break;
        }
    }
    return hr;
}

int analysis_xml(TCHAR *xmlPath)
{
    HRESULT hr;
    CComPtr<IStream> pFileStream;
    CComPtr<IXmlReader> pReader;

    XmlNodeType nodeType;

    const WCHAR* pwszPrefix;
    const WCHAR* pwszLocalName;
    const WCHAR* pwszValue;
    
    UINT cwchPrefix;
    
    // open read-only input stream:
//  if (FAILED(IStream_Write(&pFileStream, ))
//  {
// 
//  }

//  if (FAILED(hr = SHCreateStreamOnFile(xmlPath, STGM_READ, &pFileStream)))
//  {
//      scope_prompt_error(hr, _T("creating file reader failed"));
//      return -1;
//  }

    if (FAILED(hr = CreateXmlReader(__uuidof(IXmlReader), (void**) &pReader, NULL)))
    {
        scope_prompt_error(hr, _T("creating XML reader failed"));
        return -1;
    }

    if (FAILED(hr = pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit)))
    {
        scope_prompt_error(hr, _T("setting XmlReaderProperty_DtdProcessing failed"));
        return -1;
    }

    if (FAILED(hr = pReader->SetInput(pFileStream)))
    {
        scope_prompt_error(hr, _T("setting input for reader failed"));
        return -1;
    }

    // read until there are no more nodes
    while (S_OK == (hr = pReader->Read(&nodeType)))
    {
        switch (nodeType)
        {

        case XmlNodeType_XmlDeclaration:
            scope_print(_T("XmlDeclaration\n"));
            if (FAILED(hr = WriteAttributes(pReader)))
            {
                scope_print_error(hr, _T("writing attributes failed"));
                return -1;
            }
            break;

        case XmlNodeType_Element:
            if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
            {
                scope_print_error(hr, _T("getting prefix failed"));
                return -1;
            }
            if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
            {
                scope_print_error(hr, _T("getting local name failed"));
                return -1;
            }
            if (cwchPrefix > 0)
                scope_print(_T("Element: %s:%s\n"), pwszPrefix, pwszLocalName);
            else
                scope_print(_T("Element: %s\n"), pwszLocalName);

            if (FAILED(hr = WriteAttributes(pReader)))
            {
                scope_print_error(hr, _T("writing attributes failed"));
                return -1;
            }

            if (FAILED(hr = pReader->MoveToElement()))
            {
                scope_print_error(hr, _T("moving to the element that owns the current attribute node"));
                return -1;
            }

            if (pReader->IsEmptyElement() )
                scope_print(_T("(empty element)\n"));
            break;

        case XmlNodeType_EndElement:
            if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
            {
                scope_print_error(hr, _T("getting prefix failed"));
                return -1;
            }
            if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
            {
                scope_print_error(hr, _T("getting local name failed"));
                return -1;
            }
            if (cwchPrefix > 0)
                scope_print(_T("End Element: %s:%s\n"), pwszPrefix, pwszLocalName);
            else
                scope_print(_T("End Element: %s\n"), pwszLocalName);
            break;

        case XmlNodeType_Text:

        case XmlNodeType_Whitespace:
            if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
            {
                scope_print_error(hr, _T("getting value failed"));
                return -1;
            }
            scope_print(_T("Whitespace text: >%s<\n"), pwszValue);
            break;

        case XmlNodeType_CDATA:
            if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
            {
                scope_print_error(hr, _T("getting value failed"));
                return -1;
            }
            scope_print(_T("CDATA: %s\n"), pwszValue);
            break;

        case XmlNodeType_ProcessingInstruction:
            if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
            {
                scope_print_error(hr, _T("getting name failed"));
                return -1;
            }
            if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
            {
                scope_print_error(hr, _T("getting value failed"));
                return -1;
            }
            scope_print(_T("Processing Instruction name:%S value:%S\n"), pwszLocalName, pwszValue);
            break;

        case XmlNodeType_Comment:
            if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
            {
                scope_print_error(hr, _T("getting value failed"));
                return -1;
            }
            scope_print(_T("Comment: %s\n"), pwszValue);
            break;

        case XmlNodeType_DocumentType:
            scope_print(_T("DOCTYPE is not printed\n"));
            break;
        }
    }

    return 1;
}
