#include "stdafx.h"
#include "EditMgr.h"

#include "MainScene.h"
#include "ToolView.h"
#include "EditFormView.h"
#include "ConfirmFormView.h"
#include "MapTool.h"
#include "CameraTool.h"
#include "DirectionTool.h"

#include <iostream>

USING(Engine)

IMPLEMENT_SINGLETON(CEditMgr)
CEditMgr::CEditMgr(void)
	: m_pDeviceClass(nullptr)
	, m_pGraphicDev(nullptr)
	, m_pMainFrame(nullptr)
	, m_pToolView(nullptr)
	, m_pConfirmView(nullptr)
	, m_pEditFormView(nullptr)
	, m_pMainScene(nullptr)
	, m_eFocus(FOCUSID_NONE)
{
}

CEditMgr::~CEditMgr(void)
{
	Free();
}

void CEditMgr::Free(void)
{
	for (auto& iter : m_mapScene)
		Engine::Safe_Release(iter.second);
	m_mapScene.clear();

	Engine::Safe_Release(m_pCalculatorCom);
	Engine::Safe_Release(m_pGraphicDev);
	Engine::Safe_Release(m_pDeviceClass);

	Engine::Release_Resources();
	Engine::Release_Utility();
	Engine::Release_System();
}

HRESULT CEditMgr::Ready_EditMgr(void)
{
	FAILED_CHECK_RETURN(SetUp_DefaultSetting(&m_pGraphicDev), E_FAIL);
	Engine::Get_SoundMgr()->Initialize();
	m_Font_Default.CreateFont(20, // nHeight
		10, // nWidth
		0, // nEscapement
		0, // nOrientation
		1, // nWeight
		0, // bItalic
		0, // bUnderline 
		0, // cStrikeOut 
		0, // nCharSet
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		0,                              // nClipPrecision 
		DEFAULT_QUALITY,       // nQuality
		DEFAULT_PITCH | FF_DONTCARE,  // nPitchAndFamily 
		L"굴림"); // lpszFacename

	m_pCalculatorCom = Engine::CCalculator::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(m_pCalculatorCom, E_FAIL);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	FAILED_CHECK_RETURN(Ready_Scene(m_pGraphicDev), E_FAIL);

	m_eFocus = FOCUSID_NONE;

	LPDIRECT3DDEVICE9	pGraphicDev = m_pGraphicDev;
	CShader*			pShader = nullptr;

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Sample.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Shader_Sample", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Terrain.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Shader_Terrain", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Mesh.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Shader_Mesh", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Blend.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Shader_Blend", pShader), E_FAIL);

	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_LightAcc.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Shader_LightAcc", pShader), E_FAIL);

	Engine::Get_Renderer()->Ready_Renderer(m_pGraphicDev);
	return S_OK;
}

void CEditMgr::Progress_EditMgr(void)
{
	static bool bLateInit = false;
	if (!bLateInit)
	{
		bLateInit = true;
		FAILED_CHECK(Ready_EditMgr());
	}

	Engine::Set_TimeDelta(L"Timer_Immdiate");
	_float	fTime = Engine::Get_TimeDelta(L"Timer_Immdiate");
	
	if (Engine::IsPermit_Call(L"Frame60", fTime))
	{
		Engine::Set_TimeDelta(L"Timer_FPS60");
		_float	fTime60 = Engine::Get_TimeDelta(L"Timer_FPS60");

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
			dwDebugRender = (dwDebugRender == 0) ? DEBUG_RENDER_COLLIDER | DEBUG_RENDER_DIRECTION | DEBUG_RENDER_NAVI : 0;
			Engine::Get_FrameMgr()->Set_DebugRender(dwDebugRender);
		}
		for (auto& iter : m_mapScene)
		{
			iter.second->Update_Scene(fTime60);
			iter.second->LateUpdate_Scene();

			auto lstHwnd = iter.second->Get_HwndList();
			for (auto hWnd : lstHwnd)
			{
				if (hWnd)
					InvalidateRect(hWnd, nullptr, FALSE);
				m_pDeviceClass->Render_Begin(COLOR_SCREEN);
				
				Engine::Get_Renderer()->Render_GameObject(m_pGraphicDev, false);
				iter.second->Render_Scene();
				Render_FPS();
				m_pDeviceClass->Render_End(hWnd);
			}
			Engine::Get_Renderer()->Clear_RenderGroup();
		}
		SceneChange();
	}
	//std::cout << m_eFocus << std::endl;
	m_pEditFormView->m_pCameraTool->Update_CameraTool();
	m_pEditFormView->m_pDirectionTool->Update_DirectionTool();
	m_pConfirmView->Change_FocusText();
}

HRESULT CEditMgr::SetUp_DefaultSetting(LPDIRECT3DDEVICE9 * ppGraphicDev)
{
	g_hWnd = m_pToolView->GetSafeHwnd();
	//g_hInst;

	FAILED_CHECK_RETURN(Engine::Ready_GraphicDev(g_hWnd,
		Engine::MODE_WIN,
		WINCX,
		WINCY,
		&m_pDeviceClass),
		E_FAIL);

	m_pDeviceClass->AddRef();

	*ppGraphicDev = m_pDeviceClass->GetDevice();
	(*ppGraphicDev)->AddRef();
	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	//// dinput
	FAILED_CHECK_RETURN(Engine::Ready_InputDev(g_hInst, g_hWnd), E_FAIL);

	// font
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Font_Default", L"바탕", 15, 30, FW_HEAVY), E_FAIL);

	//CollisionMgr
	FAILED_CHECK_RETURN(Engine::Ready_CollisionMgr(m_pGraphicDev), E_FAIL);

	// timer
	FAILED_CHECK_RETURN(Engine::Ready_Timer(L"Timer_Immdiate"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Timer(L"Timer_FPS60"), E_FAIL);

	// frame
	//FAILED_CHECK_RETURN(Engine::Ready_Frame(L"Frame60", 60.f), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Frame(L"Frame60", 50000.f), E_FAIL);

	// resource
	FAILED_CHECK_RETURN(Engine::Reserve_ContainerSize(RESOURCEID::RESOURCE_END), E_FAIL);
	return S_OK;
}

HRESULT CEditMgr::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pMainScene = CMainScene::Create(m_pGraphicDev, m_pToolView->GetSafeHwnd());
	Add_Scene(L"ToolView", m_pMainScene);

	m_pEditFormView->m_pMapTool->Ready_MapTool();
	m_pEditFormView->m_pCameraTool->Ready_CameraTool();
	m_pEditFormView->m_pDirectionTool->Ready_DirectionTool();
	m_pConfirmView->Ready_ConfirmFormView();

	return S_OK;
}

void CEditMgr::Render_FPS(void)
{
	TCHAR	szBuf[MIN_STR];
	swprintf_s(szBuf, L"FPS: %d", Engine::Get_FPS(L"Frame60"));
	Engine::Render_Font(L"Font_Default", szBuf, &_vec2(5.f, 5.f), D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
}

void CEditMgr::SceneChange(void)
{
	if (m_mapDelayScene.size() == 0) return;
	auto pair_DelayScene = m_mapDelayScene.begin();
	while (pair_DelayScene != m_mapDelayScene.end())
	{
		auto pair_Scene = m_mapScene.find(pair_DelayScene->first);
		if (pair_Scene != m_mapScene.end())
			Engine::Safe_Release(pair_Scene->second);
		m_mapScene[pair_DelayScene->first] = pair_DelayScene->second;
		pair_DelayScene = m_mapDelayScene.erase(pair_DelayScene);
	}
}

// Scene 체인지 시 Render 이후에 추가 해야되기 때문에 추가 할시 예약을 검
void CEditMgr::Add_Scene(std::wstring _strKey, CToolScene * _pScene)
{
	auto pair = m_mapScene.find(_strKey);
	if (pair == m_mapScene.end())
	{
		m_mapScene.emplace(_strKey, _pScene);
		return;
	}

	//예약에 있을 경우 기존 예약 씬을 제거 하고 새로운걸로 바꿈
	auto pair_Deley = m_mapDelayScene.find(_strKey);
	if (pair_Deley != m_mapDelayScene.end())
	{
		ERR_MSG(L"AddScene 중복 씬 예약!!");
		Engine::Safe_Release(pair_Deley->second);
		m_mapDelayScene.erase(pair_Deley);
	}
	m_mapDelayScene.emplace(_strKey, _pScene);
}