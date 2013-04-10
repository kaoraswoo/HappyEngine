//-------------------------------------------------------------------------------------
// 해골책 참조한 XML클래스(현재 쓰이지 않음)
// 
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#ifndef _ZXML_H_
#define _ZXML_H_

/** //XML 파서용 MSXML 라이브러리 필요
 *
 */

#include <windows.h>
#include <tchar.h>
#include <msxml2.h>
#include <comdef.h>
#include "define.h"

#pragma comment(lib,"msxml2")

class ZXML
{
	IXMLDOMDocument*	m_pXMLDoc;

public:
							ZXML();
							ZXML( LPCTSTR url );
							~ZXML();

	int						Open( LPCTSTR url );
	int						Close();
	IXMLDOMNodeList*		FindElement( LPCTSTR strElement );
	IXMLDOMNodeList*		FindElement( IXMLDOMNode* pNode, LPCTSTR strElement );
	int						GetElementText( IXMLDOMNode* pNode, LPSTR strRet );
	int						GetAttributeText( IXMLDOMNode* pNode, LPSTR strAttrName, LPSTR strRet );
};

#endif // _ZXML_H_