#include "stdafx.h"
#include "MainApp.h"

#include "Export_Function.h"
#include "Logo.h"

CMainApp::CMainApp(void)
{
}

CMainApp::~CMainApp(void)
{

}

HRESULT CMainApp::Ready_MainApp(void)
{
	Engine::Get_SoundMgr()->Initialize();
	FAILED_CHECK_RETURN(SetUp_DefaultSetting(&m_pGraphicDev), E_FAIL);

	srand((unsigned)time(NULL));

	Engine::Get_FrameMgr()->Set_DebugRender(DEBUG_RENDER_NONE);
	Engine::Get_Renderer()->Ready_Renderer(m_pGraphicDev);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	FAILED_CHECK_RETURN(Ready_Scene(m_pGraphicDev, &m_pManagementClass), E_FAIL);

	return S_OK;
}

_int CMainApp::Update_MainApp(const _float& fTimeDelta)
{
	Engine::Update_InputDev();
	if (Engine::KeyDown(KEY_F1))
	{
		auto dwDebugRender = Engine::Get_FrameMgr()->Get_DebugRender();
		dwDebugRender = (dwDebugRender == 0) ? DEBUG_RENDER_ALL : 0;
		Engine::Get_FrameMgr()->Set_DebugRender(dwDebugRender);
	}
	else if (Engine::KeyDown(KEY_F2))
	{
		auto dwDebugRender = Engine::Get_FrameMgr()->Get_DebugRender();
		dwDebugRender = (dwDebugRender == 0) ? DEBUG_RENDER_COLLIDER | DEBUG_RENDER_DIRECTION : 0;
		Engine::Get_FrameMgr()->Set_DebugRender(dwDebugRender);
	}
	m_pManagementClass->Update_Scene(fTimeDelta);

	return 0;
}

void CMainApp::LateUdate_MainApp(void)
{
	m_pManagementClass->LateUpdate_Scene();
}

void CMainApp::Render_MainApp(void)
{
	Engine::Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));
	m_pManagementClass->Render_Scene(m_pGraphicDev);
	Render_FPS();
	Engine::Render_End();
}

void CMainApp::Render_FPS(void)
{
	if (true == Engine::Is_DebugRender(DEBUG_RENDER_FPS))
	{
		TCHAR	szBuf[MIN_STR];
		swprintf_s(szBuf, L"FPS: %d", Engine::Get_FPS(L"Frame60"));
		Engine::Render_Font(L"Font_Default", szBuf, &_vec2(WINCX * 0.5f, 5.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}
}

HRESULT CMainApp::SetUp_DefaultSetting(LPDIRECT3DDEVICE9 * ppGraphicDev)
{
	FAILED_CHECK_RETURN(Engine::Ready_GraphicDev(g_hWnd,
		//Engine::MODE_WIN,
		Engine::MODE_FULL,
		WINCX,
		WINCY,
		&m_pDeviceClass),
		E_FAIL);

	m_pDeviceClass->AddRef();

	*ppGraphicDev = m_pDeviceClass->GetDevice();
	(*ppGraphicDev)->AddRef();

	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	// dinput
	FAILED_CHECK_RETURN(Engine::Ready_InputDev(g_hInst, g_hWnd), E_FAIL);

	// font
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Font_Default", L"HY견명조", 15, 30, FW_HEAVY), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Font_Loading", L"HY견명조", 11, 22, FW_HEAVY), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Font_UI", L"HY견명조", 15, 30, FW_HEAVY), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Font_Damage", L"HY견명조", 15, 30, FW_HEAVY), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Font_Minigame", L"HY견명조", 30, 60, FW_HEAVY), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Font_Minigame_Combo", L"HY견명조", 15, 30, FW_HEAVY), E_FAIL);

	
	//CollisionMgr
	FAILED_CHECK_RETURN(Engine::Ready_CollisionMgr(m_pGraphicDev), E_FAIL);

	return S_OK;
}


HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement ** ppManagementClass)
{
	pGraphicDev->AddRef();


	Engine::CScene*		pScene = nullptr;

	pScene = CLogo::Create(pGraphicDev);
	NULL_CHECK_RETURN(pScene, E_FAIL);

	FAILED_CHECK_RETURN(Engine::Create_Management(pGraphicDev, ppManagementClass), E_FAIL);

	(*ppManagementClass)->AddRef();

	FAILED_CHECK_RETURN((*ppManagementClass)->SetUp_Scene(pScene), E_FAIL);

	Engine::Safe_Release(pGraphicDev);

	return S_OK;
}


CMainApp* CMainApp::Create(void)
{
	CMainApp*	pInstance = new CMainApp;

	if (FAILED(pInstance->Ready_MainApp()))
		Safe_Release(pInstance);

	return pInstance;
}

void CMainApp::Free(void)
{
	Engine::Safe_Release(m_pGraphicDev);                                                                                       
	Engine::Safe_Release(m_pDeviceClass);
	Engine::Safe_Release(m_pManagementClass);

	Engine::Release_Resources();
	Engine::Release_Utility();
	Engine::Release_System();
}
