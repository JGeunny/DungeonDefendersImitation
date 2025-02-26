// ListTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "ListTool.h"
#include "afxdialogex.h"
#include "EditMgr.h"
#include "FileInfo.h"


// CListTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CListTool, CDialog)

CListTool::CListTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_LISTTOOL, pParent)
	, m_pEditMgr(CEditMgr::GetInstance())
{

}

CListTool::~CListTool()
{
	Release();
}

void CListTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_TextureListBox);
}


BEGIN_MESSAGE_MAP(CListTool, CDialog)
	ON_WM_DROPFILES()
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_BUTTON6, &CListTool::OnBnClickedDelete)
	ON_LBN_SELCHANGE(IDC_LIST1, &CListTool::OnLbnSelchangeTextureList)
	ON_BN_CLICKED(IDC_BUTTON1, &CListTool::OnBnClickedSave)
	ON_BN_CLICKED(IDC_BUTTON4, &CListTool::OnBnClickedLoad)
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CListTool 메시지 처리기입니다.
void CListTool::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDialog::OnDropFiles(hDropInfo);
	m_TextureListBox.ResetContent();

	int iCount = DragQueryFile(hDropInfo, -1, nullptr, 0);

	TCHAR szFullPath[MAX_STR] = L"";

	for (int i = 0; i < iCount; ++i)
	{
		DragQueryFile(hDropInfo, i, szFullPath, MAX_STR);
		CFileInfo::ExtractDirectory(szFullPath, m_lstPathInfo);
	}

	wstring wstrCombined = L"";
	TCHAR szCount[MIN_STR] = L"";

	for (auto& pPathInfo : m_lstPathInfo)
	{
		// 정수 -> 문자열로 치환하는 함수.
		_itot_s(pPathInfo->iImgCount, szCount, 10);
		
		wstrCombined = pPathInfo->wstrType + L"|" + pPathInfo->wstrObjectKey +
			L"|" + szCount + L"|" + pPathInfo->wstrRelativePath;

		int idx = m_TextureListBox.AddString(wstrCombined.c_str());
		m_TextureListBox.SetItemData(idx, (DWORD)pPathInfo);
	}

	CreateHorizontalScroll();
}

void CListTool::Release()
{
	for_each(m_lstPathInfo.begin(), m_lstPathInfo.end(), Engine::Safe_Delete<Engine::PATH_INFO*>);
	m_lstPathInfo.end();
}

void CListTool::CreateHorizontalScroll()
{
	CString strName = L"";
	CSize size;
	int iCX = 0;

	CDC* pDC = m_TextureListBox.GetDC();

	// CListBox::GetCount함수(리스트 박스 목록의 전체 개수)
	for (int i = 0; i < m_TextureListBox.GetCount(); ++i)
	{
		m_TextureListBox.GetText(i, strName);

		// CDC::GetTextExtent함수(현재 텍스트의 픽셀 단위 크기를 얻어오는 함수)
		size = pDC->GetTextExtent(strName);

		if (iCX < size.cx)
			iCX = size.cx;
	}

	// 현재 리스트박스의 수평 스크롤 최대 범위를 얻어오는 함수.
	if (iCX > m_TextureListBox.GetHorizontalExtent())
		m_TextureListBox.SetHorizontalExtent(iCX); // 수평 스크롤 최대범위 설정.

	m_TextureListBox.ReleaseDC(pDC);
}

BOOL CListTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CListTool::OnBnClickedDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int idx = m_TextureListBox.GetCurSel();
	if (idx == -1) return;
	
	auto pPathInfo = (Engine::PATH_INFO*)m_TextureListBox.GetItemData(idx);
	auto iter = find_if(m_lstPathInfo.begin(), m_lstPathInfo.end(), [&](auto& _pPathInfo) {
			return pPathInfo == _pPathInfo;
			});
	if (iter != m_lstPathInfo.end())
		m_lstPathInfo.erase(iter);

	m_TextureListBox.DeleteString(idx);
}


void CListTool::OnLbnSelchangeTextureList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CListTool::OnBnClickedSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(
		FALSE,	/* 열기 TRUE, 저장 FALSE */
		L"txt", /* 디폴트 확장자 */
		L"PathInfo.txt", /* 디폴트 파일이름 */
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Text Files(*.txt)|*.txt||", /* 파일 형식 */
		this /* 부모 윈도우 */);

	TCHAR szCurDir[MAX_STR] = L"";

	GetCurrentDirectory(MAX_STR, szCurDir);
	PathRemoveFileSpec(szCurDir);
	PathRemoveFileSpec(szCurDir);
	lstrcat(szCurDir, L"\\Client\\Bin\\Resources\\Data");

	Dlg.m_ofn.lpstrInitialDir = szCurDir; // 절대경로

	if (IDOK == Dlg.DoModal())
	{
		// 파일 출력 객체
		wofstream fout;

		// 파일 개방 멤버 함수
		fout.open(Dlg.GetPathName());

		// 개방에 실패하면 true 반환.
		if (fout.fail())
		{
			ERR_MSG(Dlg.GetPathName());
			ERR_MSG(L"저장 실패");

			return;
		}

		wstring wstrCombined = L"";
		TCHAR szCount[MIN_STR] = L"";

		for (auto& pPathInfo : m_lstPathInfo)
		{
			// 정수 -> 문자열로 치환하는 함수.
			_itot_s(pPathInfo->iImgCount, szCount, 10);
			
			wstrCombined = pPathInfo->wstrType + L"|" + pPathInfo->wstrObjectKey +
				L"|" + szCount + L"|" + pPathInfo->wstrRelativePath + L"|" + pPathInfo->wstrFileName;

			// 파일 출력
			fout << wstrCombined << endl;
		}

		fout.close();
	}
}


void CListTool::OnBnClickedLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog Dlg(
		TRUE,	/* 열기 TRUE, 저장 FALSE */
		L"txt", /* 디폴트 확장자 */
		L"PathInfo.txt", /* 디폴트 파일이름 */
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Text Files(*.txt)|*.txt||", /* 파일 형식 */
		this /* 부모 윈도우 */);

	TCHAR szCurDir[MAX_STR] = L"";

	GetCurrentDirectory(MAX_STR, szCurDir);
	PathRemoveFileSpec(szCurDir);
	PathRemoveFileSpec(szCurDir);
	lstrcat(szCurDir, L"\\Client\\Bin\\Resources\\Data");

	Dlg.m_ofn.lpstrInitialDir = szCurDir; // 절대경로

	if (IDOK == Dlg.DoModal())
	{
		// C++ 파일 입력 객체
		wifstream fin;

		fin.open(Dlg.GetPathName());

		if (fin.fail())
		{
			ERR_MSG(Dlg.GetPathName());
			ERR_MSG(L"불러오기 실패");

			return;
		}

		if (!m_lstPathInfo.empty())
		{
			for_each(m_lstPathInfo.begin(), m_lstPathInfo.end(), Engine::Safe_Delete<Engine::PATH_INFO*>);
			m_lstPathInfo.clear();
		}

		m_TextureListBox.ResetContent();

		Engine::PATH_INFO* pPathInfo = nullptr;
		TCHAR szType[MAX_STR] = L"";
		TCHAR szObjectKey[MAX_STR] = L"";
		TCHAR szStateKey[MAX_STR] = L"";
		TCHAR szCount[MIN_STR] = L"";
		TCHAR szRelativePath[MAX_STR] = L"";
		TCHAR szFileNamePath[MAX_STR] = L"";

		wstring wstrCombined = L"";

		while (true)
		{
			fin.getline(szType, MAX_STR, '|');
			fin.getline(szObjectKey, MAX_STR, '|');	// '|' 단위로 끊어서 읽어들임.
			fin.getline(szCount, MIN_STR, '|');
			fin.getline(szRelativePath, MAX_STR, '|'); // 개행 단위로 끊어서 읽어들임.
			fin.getline(szFileNamePath, MIN_STR);

												  // 더이상 읽어올 내용이 없으면 break
			if (fin.eof())
				break;

			pPathInfo = new Engine::PATH_INFO;
			pPathInfo->wstrType = szType;
			pPathInfo->wstrObjectKey = szObjectKey;
			pPathInfo->iImgCount = _ttoi(szCount);
			pPathInfo->wstrRelativePath = szRelativePath;
			pPathInfo->wstrFileName = szFileNamePath;

			m_lstPathInfo.push_back(pPathInfo);

			wstrCombined = wstring(szType) + L"|" + szObjectKey + L"|"
				+ szCount + L"|" + szRelativePath + L"|" + szFileNamePath;
			int idx = m_TextureListBox.AddString(wstrCombined.c_str());
			m_TextureListBox.SetItemData(idx, (DWORD)pPathInfo);
		}

		CreateHorizontalScroll();
		fin.close();
	}
}


void CListTool::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_pEditMgr->m_eFocus = FOCUSID_NONE;
	CDialog::OnClose();
}


void CListTool::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pEditMgr->m_eFocus = FOCUSID_LISTTOOL;
}
