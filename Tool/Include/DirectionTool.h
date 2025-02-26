#pragma once


// CDirectionTool 대화 상자입니다.
class CDirectionObj;
class CDirectionTool : public CDialog
{
	DECLARE_DYNAMIC(CDirectionTool)

public:
	CDirectionTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDirectionTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIRECTIONTOOL };
#endif

protected://Virtual Funtion
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public://Message Function
	DECLARE_MESSAGE_MAP()
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	afx_msg void OnBnClicked_New();
	afx_msg void OnBnClicked_Save();
	afx_msg void OnBnClicked_Load();
	afx_msg void OnBnClicked_Add_Direction();
	afx_msg void OnBnClicked_Adjust_Direction();
	afx_msg void OnBnClicked_Del_Direction();
	afx_msg void OnBnClicked_Add_Node();
	afx_msg void OnBnClicked_Adjust_Node();
	afx_msg void OnBnClicked_Del_Node();

public:
	HRESULT	Ready_DirectionTool();
	void	Update_DirectionTool();
	void	Free();

public://Event
	void BnClickedXYZ(Engine::POSITION ePos, _float fControlSpeed);

public:
	void Event_LButton();
	void Event_MouseMove();
	void Event_RButton();
	void Event_QButton();

	bool Put_Vertex();
	bool Select_Vertex();

	//SelectedObject
	bool Select_Node(Engine::CCurve * pNode);
	void Add_SelectedNode(Engine::CCurve* pNode);
	void Clear_SelectedNode();

	//TreeCtr
	HTREEITEM Get_RootItem(_int* pOut_Level, CTreeCtrl* pTreeCtr, HTREEITEM hItem);
	HRESULT	  Add_Node(HTREEITEM hItem_Parents, CDirectionObj* pDirectionObj, _vec3 vPos_Node);

	//Object
	void Set_ObjectData(CDirectionObj *pGameObject);
	void Get_ObjectData(CDirectionObj *pGameObject);
	//Node
	void Set_NodeData(Engine::CCurve *pNode, _bool bChange_Pos = true);
	void Get_NodeData(Engine::CCurve *pNode);

public:
	CEditMgr*	m_pEditMgr;

	_vec3 m_vPos_Direction;
	CString m_strName_Direction;
	DWORD m_dwOption_Direction;

	_vec3 m_vPos_Node;
	CString m_strName_Node;
	DWORD m_dwOption_Node;

	_vec3 m_vPos_Load;

	CTreeCtrl m_DirectionTree;

	std::list<Engine::CCurve*> m_lstSelectedNode;

	afx_msg void OnTvnSelchanged_DirectionTree(NMHDR *pNMHDR, LRESULT *pResult);
};
