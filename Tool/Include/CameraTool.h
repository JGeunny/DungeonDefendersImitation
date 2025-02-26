#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CCameraTool 대화 상자입니다.
class CEditMgr;
class CCameraObj;
BEGIN(Engine)
class CCurve;
END

class CCameraTool : public CDialog
{
	DECLARE_DYNAMIC(CCameraTool)

public:
	CCameraTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCameraTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMERATOOL };
#endif

protected://Virtual Funtion
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public://Message Function
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnBnClicked_New();
	afx_msg void OnBnClicked_Save();
	afx_msg void OnBnClicked_Load();
	afx_msg void OnBnClicked_Play();
	afx_msg void OnBnClicked_Pause();
	afx_msg void OnBnClicked_Stop();
	afx_msg void OnBnClickedAdd_Camera();
	afx_msg void OnBnClickedAdjust_Camera();
	afx_msg void OnBnClickedDel_Camera();
	afx_msg void OnBnClickedAdd_Node();
	afx_msg void OnBnClickedAdjust_Node();
	afx_msg void OnBnClickedDel_Node();
	afx_msg void OnTvnSelchangedCameraTree(NMHDR *pNMHDR, LRESULT *pResult);

public:
	HRESULT	Ready_CameraTool();
	void	Update_CameraTool();
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
	HRESULT	  Add_Node(HTREEITEM hItem_Parents, CCameraObj* pCameraObj, Engine::CAMERATYPE eType, _vec3 vPos_Node);

	//Object
	void Set_ObjectData(CCameraObj *pGameObject);
	void Get_ObjectData(CCameraObj *pGameObject);
	//Node
	void Set_NodeData(Engine::CCurve *pNode, _bool bChange_Pos = true);
	void Get_NodeData(Engine::CCurve *pNode);

public:
	CEditMgr*	m_pEditMgr;

	CButton m_btnPlay;
	CButton m_btnPause;
	CButton m_btnStop;
	
	_vec3 m_vPos_Camera;
	CString m_strName_Camera;
	DWORD m_dwOption_Camera;

	_vec3 m_vPos_Node;
	_vec3 m_vPos_Node_Interval;
	CString m_strName_Node;
	DWORD m_dwOption_Node;
	_float	m_fSpeed_Node;

	_vec3 m_vPos_Load;
	CSliderCtrl m_PlayBar;
	int m_iPlay;
	BOOL m_bPlay;

	CTreeCtrl m_CameraTree;

	std::list<Engine::CCurve*> m_lstSelectedNode;
};
