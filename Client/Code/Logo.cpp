#include "stdafx.h"
#include "Logo.h"
#include "TitleUI.h"
#include "HpBar.h"

#include "Export_Function.h"

#include "Stage.h"
#include "Stage2.h"
#include "MouseUI.h"

#include "Ending.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
	, m_pLoading(nullptr)
{

}

CLogo::~CLogo(void)
{

}

HRESULT CLogo::Ready_Scene(void)
{
	static bool bLoading = false;
	Engine::CLightMgr::GetInstance()->Clear_Light();
	if (false == bLoading)
	{
		FAILED_CHECK_RETURN(Ready_Resource(m_pGraphicDev, RESOURCE_END), E_FAIL);
	}
	FAILED_CHECK_RETURN(Ready_GameLogicLayer(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::CScene::Ready_Scene(), E_FAIL);

	if (false == bLoading)
	{
		bLoading = true;
		m_pLoading = CLoading::Create(m_pGraphicDev, LOADING_STAGE);
		NULL_CHECK_RETURN(m_pLoading, E_FAIL);
	}

	Engine::Get_SoundMgr()->StopAll();
	Engine::Get_SoundMgr()->StartSound(L"MainTheme", Engine::CSoundMgr::BGM, true);
	return S_OK;
}

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_int iEvent = Engine::CScene::Update_Scene(fTimeDelta);
	if (nullptr != m_pLoading)
	{
		m_fCurLoading = (float)m_pLoading->m_iCurLoading;
		m_fMaxLoading = (float)m_pLoading->m_iMaxLoading;
	}

	if(m_fCurLoading >= m_fMaxLoading)
		m_pHpBar->Set_Activate(false);

	return EVENT_DEFAULT;
}

void CLogo::LateUpdate_Scene(void)
{
	CScene::LateUpdate_Scene();
}

void CLogo::Render_Scene(void)
{
	if(nullptr != m_pLoading)
		if (m_fCurLoading < m_fMaxLoading)
			Engine::Render_Font(L"Font_Loading", m_pLoading->Get_String().c_str(), &_vec2((WINCX >> 1) - 300.f, (WINCY >> 1) + 185.f), 
				D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	bool bRun = false;
	bRun = (nullptr == m_pLoading);
	if (nullptr != m_pLoading)
		bRun = m_pLoading->Get_Finish();
	if (true == bRun)
	{
		m_pTitleUI->Set_Activate(true);
		if (true == m_pTitleUI->IsGameStart())
		{
			Engine::CScene*		pScene = nullptr;
			pScene = CStage::Create(m_pGraphicDev);
			//pScene = CStage2::Create(m_pGraphicDev);
			Engine::SetUp_Scene(pScene);
		}
		else if (true == m_pTitleUI->IsGameEnd())
		{
			PostQuitMessage(0);
		}
	}
}

HRESULT CLogo::Ready_GameLogicLayer(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CBackGround::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_Object(L"BackGround", pGameObject), E_FAIL);

	pGameObject = m_pTitleUI = CTitleUI::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_Object(L"TitleUI", pGameObject), E_FAIL);

	CHpBar::HPBAR_DESC tLoadingInfo;
	tLoadingInfo.vPos = _vec2(WINCX >> 1, (WINCY >> 1) + 240.f);
	tLoadingInfo.vSize = _vec2(600.f, 40.f);
	tLoadingInfo.pfCurHp = &m_fCurLoading;
	tLoadingInfo.pfMaxHp = &m_fMaxLoading;
	tLoadingInfo.dwColor = D3DCOLOR_ARGB(200, 55, 255, 55);
	pGameObject = m_pHpBar = CHpBar::Create(m_pGraphicDev, tLoadingInfo);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_Object(L"LoadingInfo", pGameObject), E_FAIL);

	CMouseUI::MOUSEUI_DESC tMouseInfo;
	tMouseInfo.eCurType = CMouseUI::MOUSE_TYPE::MOUSE_BASE;
	//tMouseInfo.eCurType = CMouseUI::MOUSE_TYPE::MOUSE_AIM;
	pGameObject = CMouseUI::Create(m_pGraphicDev, &tMouseInfo);
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_Object(L"Mouse", pGameObject), E_FAIL);
	

	//pGameObject = CEnding::Create(m_pGraphicDev);
	//pGameObject->Set_Activate(true);
	//NULL_CHECK_RETURN(pLayer, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_Object(L"Ending", pGameObject), E_FAIL);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;	
}

void CLogo::Free(void)
{
	Safe_Release(m_pLoading);
	Engine::CScene::Free();
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLogo*	pInstance = new CLogo(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

HRESULT CLogo::Ready_Resource(LPDIRECT3DDEVICE9 pGraphicDev, RESOURCEID eMax)
{
	pGraphicDev->AddRef();

	Engine::Reserve_ContainerSize(eMax);

	Engine::CShader* pShader = Engine::CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Sample.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Shader_Sample", pShader), E_FAIL);

	pShader = Engine::CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Texture_Effect.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Shader_Texture_Effect", pShader), E_FAIL);

	pShader = Engine::CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Texture_UI.fx");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Prototype(L"Shader_Texture_UI", pShader), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Buffers(pGraphicDev, RESOURCE_STATIC, L"Buffer_RcTex", Engine::BUFFERID::BUFFER_RCTEX), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(pGraphicDev, RESOURCE_STAGE, L"Texture_Logo", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/RandomImage/Level%d.tga", 12), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(pGraphicDev, RESOURCE_STAGE, L"Texture_TitleUI", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Title/Title%d.png", 3), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(pGraphicDev, RESOURCE_STAGE, L"Texture_UI_HpBar", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/HUD/HpBar/HUD_HpBar%d.tga", 2), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(pGraphicDev, RESOURCE_STAGE, L"Texture_MouseUI", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/HUD/MouseUI/MouseUI%d.png", 2), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, RESOURCE_STAGE, L"Texture_Ending", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ending.png"), E_FAIL);

	Engine::Get_SoundMgr()->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/DungeonDefense_MainTheme.ogg"),
		L"MainTheme");
	Engine::Get_SoundMgr()->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/Menu_Highlight.ogg"),
		L"ButtonMouseOver");
	Engine::Get_SoundMgr()->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/StartGame2D.ogg"),
		L"ButtonClick");
	Engine::Get_SoundMgr()->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/Action_Guard_Success_delia.wav"),
		L"Fall_TitleUI");
	Engine::Get_SoundMgr()->AddSoundFile(
		SRC_PATH(L"../Bin/Resource/Sound/Jonas Blue - I See Love (Feat. Joe Jonas) [From Hotel Transylvania 3].mp3"),
		L"Ending");


	
	
	Engine::Safe_Release(pGraphicDev);

	return S_OK;
}

