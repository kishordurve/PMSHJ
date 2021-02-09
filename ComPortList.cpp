// ComPortList.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "ComPortList.h"


// CComPortList

IMPLEMENT_DYNAMIC(CComPortList, CListBox)
CComPortList::CComPortList()
{
	m_bNoneItem = TRUE;								// First item is "<None>"
	m_bOnlyPhysical = FALSE;						// Include virtual COM ports
	m_bOnlyPresent = TRUE;							// Only present ports
	m_strNone = _T("<None>");
}

CComPortList::~CComPortList()
{
}


BEGIN_MESSAGE_MAP(CComPortList, CListBox)
END_MESSAGE_MAP()



// CComPortList-Meldungshandler

// Get item data for current selection (port number with additional flags)
int CComPortList::GetCurData() const
{
	int nSel = GetCurSel();
	return nSel >= 0 ? static_cast<int>(GetItemData(nSel)) : 0;
}

// Fill the list.
BOOL CComPortList::InitList(int nDefPort /*= -1*/)
{
	// ListBox should use single selection
	ASSERT(!(GetStyle() & (LBS_EXTENDEDSEL | LBS_MULTIPLESEL | LBS_NOSEL)));

	m_nDefPort = nDefPort;							// use member var for access by call back function
	if (m_nDefPort < 0)								// when no default port specified
		m_nDefPort = GetPortNum();					//  use the currently selected one upon re-loading
	ResetContent();
	if (m_bNoneItem)
	{
		int nItem = AddString(m_strNone.GetString());
		SetItemData(nItem, 0);
	}

	CEnumDevices Enum;
	BOOL bResult = Enum.EnumSerialPorts(this, CallbackWrapper, m_bOnlyPresent);

	// If no port pre-selected and none item used or only one port present, select first item.
	if (bResult && GetCurSel() < 0 && (m_bNoneItem || GetCount() == 1))
		SetCurSel(0);
	return bResult;
}

// Static wrapper function to add list items.
// Called by the CEnumDevices enumeration function.
/*static*/ void CComPortList::CallbackWrapper(CObject* pObject, const CEnumDevInfo* pInfo)
{
	ASSERT(pObject != NULL);
	CComPortList* pThis = reinterpret_cast<CComPortList*>(pObject);
	ASSERT(pThis->IsKindOf(RUNTIME_CLASS(CComPortList)));
	pThis->AddItem(pInfo);
}

void CComPortList::AddItem(const CEnumDevInfo* pInfo)
{
	ASSERT(pInfo != NULL);
	//if (pInfo->m_strName.Find(L"Silicon") == -1)// This hides the Silicon Labs ports (Excell) 
	if (pInfo->m_nPortNum > 0 &&
		(!m_bOnlyPhysical || !(pInfo->m_nPortNum & DATA_VIRTUAL_MASK)))
	{
		int nItem = AddString(pInfo->m_strName.GetString());
		SetItemData(nItem, static_cast<DWORD>(pInfo->m_nPortNum));
		if ((pInfo->m_nPortNum & DATA_PORT_MASK) == m_nDefPort)
			SetCurSel(nItem);
	}
}
