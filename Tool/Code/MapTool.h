#pragma once
#include "afxwin.h"

#include "Export_Function.h"

#include "PreViewScene.h"
// CMapTool 대화 상자입니다.
class CEditMgr;

class CMapTool : public CDialog
{
	DECLARE_DYNAMIC(CMapTool)

public:
	CMapTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMapTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAPTOOL };
#endif

protected://Virtual Funtion
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

public://Message Function
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedAdjust();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnLbnSelchangeCubeList();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnLbnSelchangeTexture();
	afx_msg void OnBnClickedMultiAdd();
	afx_msg void OnCbnSelchangeTextureCbo();
	afx_msg void OnBnClickedRndAdd();
	afx_msg void OnBnClickedNew();
	afx_msg void OnBnClickedDelName();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();

public:
	HRESULT	Ready_MapTool();
	void Free();
	void Ready_HorizontalScroll();
	void Change_PreView();

public:
	void Event_LButton();
	void Event_MouseMove();
	void Event_RButton();
	void Event_QButton();

	//Object
	void Set_ObjectData(Engine::CGameObject *pGameObject, bool bTransPos = true);
	void Get_ObjectData(Engine::CGameObject *pGameObject);
	void Remove_Object(CString strName);
	void Clear_Object();
	bool Pick_Object();
	bool Put_Object();

	//SelectedObject
	bool Select_Object(Engine::CGameObject *pGameObject);
	void Add_SelectedObject(Engine::CGameObject *pGameObject);
	void Clear_SelectedObject();

	//Camera
	void Ready_Camera();

	//Etc
	void Ready_TextureCboList();
	void Change_TextureList(RESOURCEID eType);

public://Event
	void BnClickedXYZ(Engine::POSITION ePos, _float fControlSpeed);
public:
	CEditMgr*	m_pEditMgr;

public:
	CListBox m_CubeListBox;
	CListBox m_TextureListBox;
	CStatic m_PictureTexture;
	CComboBox m_TextureCbo;

public:
	CPreViewScene*	m_pPreViewScene;

public:
	_vec3 m_vPos_Object;
	_vec3 m_vAngle_Object;
	_vec3 m_vScale_Object;
	DWORD m_dwOption;
	CString m_strName;
	
	_vec3 m_vMultiAdd_Object;
	_vec3 m_vPos_Load;

	int m_iRndPercentage;
	
	BOOL m_bChkRot;
	BOOL m_bChkScale;
	
	std::map<std::wstring, Engine::CTexture*> m_mapTextureInfo;
	std::list<Engine::CGameObject*> m_lstSelectedObject;
	CString m_strPreviewScale;
};
