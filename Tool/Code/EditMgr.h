#pragma once

#include "Engine_Defines.h"
#include "Base.h"

class CToolScene;
class CMainScene;

class CMainFrame;			// MAIN
class CToolView;			// LEFT_UP
class CConfirmFormView;		// LEFT_DOWN
class CEditFormView;		// RIGHT
class CFont;

class CEditMgr : public CBase
{
	DECLARE_SINGLETON(CEditMgr)
private:
	explicit					CEditMgr(void);
	virtual						~CEditMgr(void);

public:
	HRESULT						Ready_EditMgr(void);
	void						Progress_EditMgr(void);

private:
	HRESULT						SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
	HRESULT						Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev);
	void						Render_FPS(void);

private:
	void						SceneChange(void);

public:
	void						Add_Scene(std::wstring _strKey, CToolScene* _pScene);

public:
	Engine::CGraphicDev*		m_pDeviceClass;
	LPDIRECT3DDEVICE9			m_pGraphicDev;

public:
	CMainFrame*					m_pMainFrame;
	CToolView*					m_pToolView;
	CConfirmFormView*			m_pConfirmView;
	CEditFormView*				m_pEditFormView;


public:
	CMainScene*					m_pMainScene;
	std::map<std::wstring, CToolScene*>	m_mapScene;
	Engine::CCalculator*		m_pCalculatorCom;
	CFont						m_Font_Default;

	FOCUSID						m_eFocus;

protected:
	std::map<std::wstring, CToolScene*>::iterator	m_iterScene;

private:
	std::map<std::wstring, CToolScene*>	m_mapDelayScene; // Scene 체인지 시 Render 이후에 추가 해야되기 때문에 추가 할시 예약을 검

	
public:
	void virtual Free(void);
};

