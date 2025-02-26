
// MainFrm.cpp : CMainFrame Ŭ������ ����
//

#include "stdafx.h"
#include "Tool.h"

#include "MainFrm.h"
#include "ToolView.h"
#include "EditFormView.h"
#include "ConfirmFormView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
	:m_pEditMgr(CEditMgr::GetInstance())
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// �޴��� ���ֱ�
	SetMenu(NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	// Ÿ��Ʋ ���� ����
	cs.style &= ~FWS_ADDTOTITLE;
	cs.lpszName = L"Dungeon Defenders";

	cs.cx = FORM_WIDTH;
	cs.cy = FORM_HEIGHT;

	cs.style &= ~WS_MAXIMIZEBOX;
	cs.style &= ~WS_THICKFRAME;

	return TRUE;
}

// CMainFrame ����

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame �޽��� ó����
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_RightWnd.CreateStatic(this, 1, 2);
	m_LeftWnd.CreateStatic(&m_RightWnd, 2, 1, WS_CHILD | WS_VISIBLE, m_RightWnd.IdFromRowCol(0, 0));

	m_RightWnd.SetColumnInfo(0, FORM_MAIN_WIDTH + FORM_BORDER, 0);

	m_LeftWnd.CreateView(0, 0, RUNTIME_CLASS(CToolView), CSize(FORM_MAIN_WIDTH, FORM_MAIN_HEIGHT), pContext);
	m_LeftWnd.CreateView(1, 0, RUNTIME_CLASS(CConfirmFormView), CSize(FORM_BOTTOM_WIDTH, FORM_BOTTOM_HEIGHT), pContext);
	
	m_RightWnd.CreateView(0, 1, RUNTIME_CLASS(CEditFormView), CSize(FORM_RIGHT_WIDTH, FORM_RIGHT_HEIGHT), pContext);

	return TRUE;
}
