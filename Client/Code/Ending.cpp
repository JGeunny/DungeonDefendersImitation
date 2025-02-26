#include "stdafx.h"
#include "Ending.h"

#include "Export_Function.h"

CEnding::CEnding(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
	m_bActivate = false;
	m_fViewZ = 9999999;
}

CEnding::~CEnding(void)
{
}

HRESULT CEnding::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);
	m_eRenderType = Engine::RENDERID::RENDER_UI;

	return S_OK;
}

_int CEnding::Update_Object(const _float& fTimeDelta)
{
	if (false == m_bActivate) return EVENT_DEFAULT;
	if(false == m_bLateStart)
	{
		m_bLateStart = true;
		Engine::Get_SoundMgr()->StopAll();
		Engine::Get_SoundMgr()->StartSound(L"Ending", Engine::CSoundMgr::BGM, true);
	}

	Engine::CGameObject::Update_Object(fTimeDelta);
	Update_Bounce(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(m_eRenderType, this);
	return 0;
}

void CEnding::LateUpdate_Object(void)
{
}

void CEnding::Render_Object(void)
{
	_matrix		matWorld, matView, matOriginView, matOriginProj;

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matOriginView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOriginProj);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	Render_UI(m_tUIInfo);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOriginView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOriginProj);	
}

HRESULT CEnding::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Ending"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	return S_OK;
}

HRESULT CEnding::Render_UI(UI_DESC& tUIInfo)
{
	_vec2 vPos = tUIInfo.vPos;
	_vec2 vSize = tUIInfo.vSize;
	_matrix matWorld;
	D3DXMatrixIdentity(&matWorld);

	matWorld._11 = vSize.x;
	matWorld._22 = vSize.y;
	matWorld._33 = 1.f;
	matWorld._41 = vPos.x - (WINCX >> 1);
	matWorld._42 = -vPos.y + (WINCY >> 1);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Render_Texture(0);
	m_pBufferCom->Render_Buffer();

	return S_OK;
}

HRESULT CEnding::Update_Bounce(float fTimeDelta)
{
	if (m_iBounceCount >= 3.f && m_tUIInfo.vPos.y >= WINCY * 0.5f)
	{
		m_tUIInfo.vPos.y = WINCY * 0.5f;
		return E_FAIL;
	}

	_vec2 vDir = _vec2(0.f, 0.f);
	vDir.y += (m_fBouncePower - 12.f * m_fJumpAccel) * fTimeDelta* 8.f;
	m_fJumpAccel += 8.f*fTimeDelta;
	m_tUIInfo.vPos -= vDir;

	if (m_iBounceCount == 3)
	{
		if (m_tUIInfo.vPos.y < WINCY * 0.4f)
		{
			++m_iBounceCount;
			m_fJumpAccel = 0.f;
		}
	}
	else
	{
		if (m_tUIInfo.vPos.y > WINCY * 0.5f)
		{
			Engine::Get_SoundMgr()->StartSound(L"Fall_TitleUI", Engine::CSoundMgr::ENV, false);
			++m_iBounceCount;
			switch (m_iBounceCount)
			{
			case 1:
				m_fBouncePower = 60.f;
				break;
			case 2:
				m_fBouncePower = 50.f;
				break;
			case 3:
				m_fBouncePower = 40.f;
				break;
			default:
				m_fBouncePower = 10.f;
				break;
			}
			m_fJumpAccel = 0.f;
		}
	}
	return S_OK;
}

CEnding * CEnding::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEnding *	pInstance = new CEnding(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CEnding::Free(void)
{
	Engine::CGameObject::Free();
}



