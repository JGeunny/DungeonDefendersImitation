#pragma once
#include "afxwin.h"


// CNaviTool 대화 상자입니다.
class CEditMgr;
class CNaviObj;
class CPickPoint;
class CNaviTool : public CDialog
{
	DECLARE_DYNAMIC(CNaviTool)

public:
	CNaviTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNaviTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NAVITOOL };
#endif

protected://Virtual Funtion
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	
public://Message Function
	DECLARE_MESSAGE_MAP()
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	afx_msg void OnBnClickedChkNoUnit();
	afx_msg void OnBnClickedChkNoBuild();
	afx_msg void OnBnClickedChkNoCam();
	afx_msg void OnBnClickedAdd_Navi();
	afx_msg void OnBnClickedAdjust_Navi();
	afx_msg void OnBnClickedDel_Navi();
	afx_msg void OnLbnSelchangeNaviList();
	afx_msg void OnLbnSelchangeCellList();
	afx_msg void OnBnClickedAdd_Cell();
	afx_msg void OnBnClickedAdjust_Cell();
	afx_msg void OnBnClickedDel_Cell();
	afx_msg void OnBnClicked_New();
	afx_msg void OnBnClicked_Save();
	afx_msg void OnBnClicked_Load();

public:
	HRESULT	Ready_NaviTool();
	void	Free();

public:
	void Event_LButton();
	void Event_MouseMove();
	void Event_RButton();
	void Event_QButton();

	bool Put_Vertex();
	bool Select_Vertex();
	void Update_Option_Cell();
	void Set_Option_Cell_CheckBox();
	void Change_PreView();

	//SelectedObject
	bool Select_Cell(Engine::CCell * pCell);
	void Add_SelectedCell(Engine::CCell* pCell);
	void Clear_SelectedCell();
public:
	//Object
	void Set_ObjectData(Engine::CGameObject *pGameObject);
	void Get_ObjectData(Engine::CGameObject *pGameObject);
	//Cell
	void Set_CellData(Engine::CCell *pCell);
	void Get_CellData(Engine::CCell *pCell);

	HRESULT Ready_CellList(CNaviObj* pNaviObj);

	//PickPoint
	void Add_PickPoint(CPickPoint* pPickPoint);
	BOOL CheckAndClear_PickPoint();
	void Clear_PickPoint();

public://Event
	void BnClickedXYZ(Engine::POSITION ePos, _float fControlSpeed);

public:
	CEditMgr* m_pEditMgr;

	_vec3 m_vPos_Navi;
	DWORD m_dwOption_Navi;
	CString m_strName_Navi;

	_vec3 m_vPos_Vertex1;
	_vec3 m_vPos_Vertex2;
	_vec3 m_vPos_Vertex3;
	CString m_strName_Cell;
	DWORD m_dwOption_Cell;
	
	BOOL m_bNoUnit;
	BOOL m_bNoBuild;
	BOOL m_bNoCam;
	BOOL m_bYesFish;

	_vec3 m_vPos_Load;

	BOOL m_bPutVertex;
	int m_iSelectType;	// 라디오 박스 0:Multi 1:Single 2:Cell

	CListBox m_NaviListBox;
	CListBox m_CellListBox;

	int m_iMaxCount_PickPoint = 3;
	std::list<CPickPoint*> m_lstPickPoint;
	

	std::list<Engine::CCell*> m_lstSelectedCell;
	afx_msg void OnBnClickedChkYesFish();
};
