#include "stdafx.h"
#include "WaveInfo.h"

#include "Export_Function.h"

CWaveInfo::CWaveInfo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CWaveInfo::~CWaveInfo(void)
{
}

HRESULT CWaveInfo::Ready_Object(WAVEINFO_DESC* pArg)
{
	if (nullptr != pArg)
	{
		m_tWaveInfo = *pArg;
	}
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_eRenderType = Engine::RENDERID::RENDER_UI;
	
	//Ready_UI(1, true);
	m_fViewZ = 99999.f;
	return S_OK;
}

void CWaveInfo::LateReady_Object(void)
{
}

_int CWaveInfo::Update_Object(const _float& fTimeDelta)
{
	Engine::CGameObject::LateReady_Object();
	if (m_eCurUI == WAVEINFO_NONE)
	{
		auto iter = m_vecUI.begin();
		if (iter != m_vecUI.end())
		{
			m_eCurUI = *iter;
			m_vecUI.erase(iter);
		}
		if (m_eCurUI != WAVEINFO_NONE)
		{
			//초기 세팅
			m_vPos = m_tWaveInfo.vBeginPos;
			m_vSize = m_tWaveInfo.vSize;
			m_fDelayTime = 0.f;
		}
	}
	
	if(m_vPos.y <= (WINCY >> 1) + m_fIntervalY)
		m_vPos.y += fTimeDelta* m_tWaveInfo.fSpeed;
	else
	{
		m_fDelayTime += fTimeDelta;
		if(m_fDelayTime >= 2.f)
			m_vPos.y += fTimeDelta* m_tWaveInfo.fSpeed * 1.5f;
	}
	if (m_vPos.y > (WINCY >> 1) + 50 + m_fIntervalY)
		m_eCurUI = WAVEINFO_NONE;
	
	Engine::CGameObject::Update_Object(fTimeDelta);

	if(m_eCurUI != WAVEINFO_NONE)
 		m_pRendererCom->Add_RenderGroup(m_eRenderType, this);
	return m_iEvent;
}

void CWaveInfo::LateUpdate_Object(void)
{
}

void CWaveInfo::Render_Object(void)
{
	Ortho();
	m_pShaderCom->Begin_Shader(nullptr);
	if (SUCCEEDED(Ready_Shader(m_pShaderCom)))
	{
		m_pShaderCom->Begin_Pass(0);
		m_pBufferCom->Render_Buffer();
		m_pShaderCom->End_Pass();
	}
	m_pShaderCom->End_Shader();
}

void CWaveInfo::Ready_UI(int iLevel, bool bBuild)
{
	m_vecUI.clear();
	if (true == bBuild)
	{
		switch (iLevel)
		{
		case 1:
			m_vecUI.emplace_back(WAVEINFO_WAVE1);
			m_vecUI.emplace_back(WAVEINFO_BUILD);
			break;
		case 2:
			m_vecUI.emplace_back(WAVEINFO_COMPLETE);
			m_vecUI.emplace_back(WAVEINFO_WAVE2);
			m_vecUI.emplace_back(WAVEINFO_BUILD);
			break;
		case 3:
			m_vecUI.emplace_back(WAVEINFO_COMPLETE);
			m_vecUI.emplace_back(WAVEINFO_WAVE3);
			m_vecUI.emplace_back(WAVEINFO_BUILD);
			break;
		case 4:
			m_vecUI.emplace_back(WAVEINFO_COMPLETE);
			m_vecUI.emplace_back(WAVEINFO_VICTORY);
			m_vecUI.emplace_back(WAVEINFO_COMEBACKHOME);
			break;
		}
	}
	else
	{
		m_vecUI.emplace_back(WAVEINFO_COMBAT);
	}
}

HRESULT CWaveInfo::Ready_Shader(Engine::CShader* pShader)
{
	if(nullptr == pShader)
		return E_FAIL;
	_matrix matView;
	D3DXMatrixIdentity(&matView);
	pShader->Set_Value("g_matWorld", &m_pTransCom->m_matWorld, sizeof(_matrix));
	pShader->Set_Value("g_matView", &matView, sizeof(_matrix));
	pShader->Set_Value("g_matProj", &m_matProj, sizeof(_matrix));
	pShader->Set_Texture("g_DiffuseTexture", m_pTextureCom->Get_Texture((int)m_eCurUI));
	pShader->Commit_Changes();

	return S_OK;
}

HRESULT CWaveInfo::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureAdminCom = Engine::CTextureAdmin::Create(&m_pTextureCom, RESOURCE_STAGE, m_tWaveInfo.strImageKey.c_str());
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_TextureAdmin", pComponent);
	
	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Texture_UI"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

void CWaveInfo::Ortho(void)
{
	_matrix		matWorld;
	D3DXMatrixIdentity(&matWorld);
	matWorld._11 = m_vSize.x;
	matWorld._22 = m_vSize.y;
	matWorld._33 = 1.f;
	matWorld._41 = m_vPos.x - (WINCX >> 1);
	matWorld._42 = -m_vPos.y + (WINCY >> 1);

	m_pTransCom->m_matWorld = matWorld;
}

CWaveInfo * CWaveInfo::Create(LPDIRECT3DDEVICE9 pGraphicDev, WAVEINFO_DESC* pArg)
{
	CWaveInfo *	pInstance = new CWaveInfo(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pArg)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CWaveInfo::Free(void)
{
	Engine::CGameObject::Free();
}
