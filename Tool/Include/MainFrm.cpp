
// MainFrm.cpp : CMainFrame 클래스의 구현
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
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
	:m_pEditMgr(CEditMgr::GetInstance())
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 메뉴바 없애기
	SetMenu(NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	// 타이틀 제목 변경
	cs.style &= ~FWS_ADDTOTITLE;
	cs.lpszName = L"Dungeon Defenders";

	cs.cx = FORM_WIDTH;
	cs.cy = FORM_HEIGHT;

	cs.style &= ~WS_MAXIMIZEBOX;
	cs.style &= ~WS_THICKFRAME;

	return TRUE;
}

// CMainFrame 진단

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


// CMainFrame 메시지 처리기
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_RightWnd.CreateStatic(this, 1, 2);
	m_LeftWnd.CreateStatic(&m_RightWnd, 2, 1, WS_CHILD | WS_VISIBLE, m_RightWnd.IdFromRowCol(0, 0));

	m_RightWnd.SetColumnInfo(0, FORM_MAIN_WIDTH + FORM_BORDER, 0);

	m_LeftWnd.CreateView(0, 0, RUNTIME_CLASS(CToolView), CSize(FORM_MAIN_WIDTH, FORM_MAIN_HEIGHT), pContext);
	m_LeftWnd.CreateView(1, 0, RUNTIME_CLASS(CConfirmFormView), CSize(FORM_BOTTOM_WIDTH, FORM_BOTTOM_HEIGHT), pContext);
	
	m_RightWnd.CreateView(0, 1, RUNTIME_CLASS(CEditFormView), CSize(FORM_RIGHT_WIDTH, FORM_RIGHT_HEIGHT), pContext);

	return TRUE;
}
