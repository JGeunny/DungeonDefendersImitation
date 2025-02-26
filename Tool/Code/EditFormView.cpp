// EditFormView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "EditFormView.h"

#include "EditMgr.h"

#include "MapTool.h"
#include "NaviTool.h"
#include "CameraTool.h"
#include "ListTool.h"
#include "DirectionTool.h"
//#include "EffectTool.h"

// CEditFormView

IMPLEMENT_DYNCREATE(CEditFormView, CFormView)

CEditFormView::CEditFormView()
	: CFormView(IDD_EDITFORMVIEW)
{
	m_pMapTool = new CMapTool();
	m_pMapTool->Create(IDD_MAPTOOL, CWnd::GetDesktopWindow());

	m_pNaviTool = new CNaviTool();
	m_pNaviTool->Create(IDD_NAVITOOL, CWnd::GetDesktopWindow());

	m_pCameraTool = new CCameraTool();
	m_pCameraTool->Create(IDD_CAMERATOOL, CWnd::GetDesktopWindow());

	m_pListTool = new CListTool();
	m_pListTool->Create(IDD_LISTTOOL, CWnd::GetDesktopWindow());
	
	//m_pEffectTool = new CEffectTool();
	//m_pEffectTool->Create(IDD_EFFECTTOOL, CWnd::GetDesktopWindow());

	m_pDirectionTool = new CDirectionTool();
	m_pDirectionTool->Create(IDD_DIRECTIONTOOL, CWnd::GetDesktopWindow());
}

CEditFormView::~CEditFormView()
{
	Release();
}

void CEditFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEditFormView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CEditFormView::OnBnClicked_MapTool)
	ON_BN_CLICKED(IDC_BUTTON3, &CEditFormView::OnBnClicked_CameraTool)
	ON_BN_CLICKED(IDC_BUTTON2, &CEditFormView::OnBnClicked_NaviTool)
	ON_BN_CLICKED(IDC_BUTTON5, &CEditFormView::OnBnClicked_ListTool)
	ON_BN_CLICKED(IDC_BUTTON7, &CEditFormView::OnBnClicked_DirectionTool)
END_MESSAGE_MAP()


// CEditFormView �����Դϴ�.

#ifdef _DEBUG
void CEditFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CEditFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CEditFormView �޽��� ó�����Դϴ�.

void CEditFormView::Release()
{
	Engine::Safe_Delete(m_pMapTool);
	Engine::Safe_Delete(m_pNaviTool);
	Engine::Safe_Delete(m_pCameraTool);
	Engine::Safe_Delete(m_pListTool);
	//Engine::Safe_Delete(m_pEffectTool);
	Engine::Safe_Delete(m_pDirectionTool);
}

void CEditFormView::OnBnClicked_MapTool()
{
	//TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	RECT rtTool, rtWin;
	m_pMapTool->GetWindowRect(&rtTool);
	this->GetWindowRect(&rtWin);
	m_pMapTool->SetWindowPos(nullptr, rtWin.left, rtWin.top, -rtTool.left + rtTool.right, -rtTool.top + rtTool.bottom, 0);
	m_pMapTool->ShowWindow(SW_SHOW);
}


//void CEditFormView::OnBnClicked_EffectTool()
//{
//	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	//m_pEffectTool->ShowWindow(SW_SHOW);
//}


void CEditFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	CScrollView::SetScrollSizes(MM_TEXT, CSize(0, 0));

	CEditMgr::GetInstance()->m_pEditFormView = this;
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}


void CEditFormView::OnBnClicked_CameraTool()
{
	//TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	RECT rtTool, rtWin;
	m_pCameraTool->GetWindowRect(&rtTool);
	this->GetWindowRect(&rtWin);
	m_pCameraTool->SetWindowPos(nullptr, rtWin.left, rtWin.top, -rtTool.left + rtTool.right, -rtTool.top + rtTool.bottom, 0);
	m_pCameraTool->ShowWindow(SW_SHOW);
}


void CEditFormView::OnBnClicked_NaviTool()
{
	//TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	RECT rtTool, rtWin;
	m_pNaviTool->GetWindowRect(&rtTool);
	this->GetWindowRect(&rtWin);
	m_pNaviTool->SetWindowPos(nullptr, rtWin.left, rtWin.top, -rtTool.left + rtTool.right, -rtTool.top + rtTool.bottom, 0);
	m_pNaviTool->ShowWindow(SW_SHOW);
}


void CEditFormView::OnBnClicked_ListTool()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	RECT rtTool, rtWin;
	m_pListTool->GetWindowRect(&rtTool);
	this->GetWindowRect(&rtWin);
	m_pListTool->SetWindowPos(nullptr, rtWin.left, rtWin.top, -rtTool.left + rtTool.right, -rtTool.top + rtTool.bottom, 0);
	m_pListTool->ShowWindow(SW_SHOW);
}


void CEditFormView::OnBnClicked_DirectionTool()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	RECT rtTool, rtWin;
	m_pDirectionTool->GetWindowRect(&rtTool);
	this->GetWindowRect(&rtWin);
	m_pDirectionTool->SetWindowPos(nullptr, rtWin.left, rtWin.top, -rtTool.left + rtTool.right, -rtTool.top + rtTool.bottom, 0);
	m_pDirectionTool->ShowWindow(SW_SHOW);
}
