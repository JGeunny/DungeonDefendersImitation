// ListTool.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "ListTool.h"
#include "afxdialogex.h"
#include "EditMgr.h"
#include "FileInfo.h"


// CListTool ��ȭ �����Դϴ�.

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


// CListTool �޽��� ó�����Դϴ�.
void CListTool::OnDropFiles(HDROP hDropInfo)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
		// ���� -> ���ڿ��� ġȯ�ϴ� �Լ�.
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

	// CListBox::GetCount�Լ�(����Ʈ �ڽ� ����� ��ü ����)
	for (int i = 0; i < m_TextureListBox.GetCount(); ++i)
	{
		m_TextureListBox.GetText(i, strName);

		// CDC::GetTextExtent�Լ�(���� �ؽ�Ʈ�� �ȼ� ���� ũ�⸦ ������ �Լ�)
		size = pDC->GetTextExtent(strName);

		if (iCX < size.cx)
			iCX = size.cx;
	}

	// ���� ����Ʈ�ڽ��� ���� ��ũ�� �ִ� ������ ������ �Լ�.
	if (iCX > m_TextureListBox.GetHorizontalExtent())
		m_TextureListBox.SetHorizontalExtent(iCX); // ���� ��ũ�� �ִ���� ����.

	m_TextureListBox.ReleaseDC(pDC);
}

BOOL CListTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CListTool::OnBnClickedDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CListTool::OnBnClickedSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CFileDialog Dlg(
		FALSE,	/* ���� TRUE, ���� FALSE */
		L"txt", /* ����Ʈ Ȯ���� */
		L"PathInfo.txt", /* ����Ʈ �����̸� */
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Text Files(*.txt)|*.txt||", /* ���� ���� */
		this /* �θ� ������ */);

	TCHAR szCurDir[MAX_STR] = L"";

	GetCurrentDirectory(MAX_STR, szCurDir);
	PathRemoveFileSpec(szCurDir);
	PathRemoveFileSpec(szCurDir);
	lstrcat(szCurDir, L"\\Client\\Bin\\Resources\\Data");

	Dlg.m_ofn.lpstrInitialDir = szCurDir; // ������

	if (IDOK == Dlg.DoModal())
	{
		// ���� ��� ��ü
		wofstream fout;

		// ���� ���� ��� �Լ�
		fout.open(Dlg.GetPathName());

		// ���濡 �����ϸ� true ��ȯ.
		if (fout.fail())
		{
			ERR_MSG(Dlg.GetPathName());
			ERR_MSG(L"���� ����");

			return;
		}

		wstring wstrCombined = L"";
		TCHAR szCount[MIN_STR] = L"";

		for (auto& pPathInfo : m_lstPathInfo)
		{
			// ���� -> ���ڿ��� ġȯ�ϴ� �Լ�.
			_itot_s(pPathInfo->iImgCount, szCount, 10);
			
			wstrCombined = pPathInfo->wstrType + L"|" + pPathInfo->wstrObjectKey +
				L"|" + szCount + L"|" + pPathInfo->wstrRelativePath + L"|" + pPathInfo->wstrFileName;

			// ���� ���
			fout << wstrCombined << endl;
		}

		fout.close();
	}
}


void CListTool::OnBnClickedLoad()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CFileDialog Dlg(
		TRUE,	/* ���� TRUE, ���� FALSE */
		L"txt", /* ����Ʈ Ȯ���� */
		L"PathInfo.txt", /* ����Ʈ �����̸� */
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Text Files(*.txt)|*.txt||", /* ���� ���� */
		this /* �θ� ������ */);

	TCHAR szCurDir[MAX_STR] = L"";

	GetCurrentDirectory(MAX_STR, szCurDir);
	PathRemoveFileSpec(szCurDir);
	PathRemoveFileSpec(szCurDir);
	lstrcat(szCurDir, L"\\Client\\Bin\\Resources\\Data");

	Dlg.m_ofn.lpstrInitialDir = szCurDir; // ������

	if (IDOK == Dlg.DoModal())
	{
		// C++ ���� �Է� ��ü
		wifstream fin;

		fin.open(Dlg.GetPathName());

		if (fin.fail())
		{
			ERR_MSG(Dlg.GetPathName());
			ERR_MSG(L"�ҷ����� ����");

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
			fin.getline(szObjectKey, MAX_STR, '|');	// '|' ������ ��� �о����.
			fin.getline(szCount, MIN_STR, '|');
			fin.getline(szRelativePath, MAX_STR, '|'); // ���� ������ ��� �о����.
			fin.getline(szFileNamePath, MIN_STR);

												  // ���̻� �о�� ������ ������ break
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	m_pEditMgr->m_eFocus = FOCUSID_NONE;
	CDialog::OnClose();
}


void CListTool::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_pEditMgr->m_eFocus = FOCUSID_LISTTOOL;
}
