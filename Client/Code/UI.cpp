#include "stdafx.h"
#include "UI.h"

#include "Export_Function.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_iCurGold(0)
{
	m_eRenderType = Engine::RENDERID::RENDER_UI;
}

CUI::~CUI(void)
{
}

HRESULT CUI::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_fViewZ = 1000.f;
	return S_OK;
}

_int CUI::Update_Object(const _float& fTimeDelta)
{
	m_fAminingAngle += D3DXToRadian(720) * fTimeDelta;
	if (D3DXToRadian(m_fAminingAngle) >= D3DX_PI * 2)
		m_fAminingAngle = 0.f;
	Engine::CGameObject::Update_Object(fTimeDelta);
	Update_TargetInfo();
	m_pRendererCom->Add_RenderGroup(m_eRenderType, this);
	return 0;
}

void CUI::LateUpdate_Object(void)
{
}

void CUI::Render_Object(void)
{
	_matrix		matWorld, matView, matOriginView, matOriginProj;

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matOriginView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOriginProj);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	for (int i = 0; i < 2; ++i)
	{
		if (i == 0)
		{
			m_fSizeX = 500.f;
			m_fSizeY = 220.f;
			m_fX = WINCX - m_fSizeX * 0.5f;
			m_fY = m_fSizeY * 0.5f;// WINCY - m_fSizeY* 0.5f;
		}
		else
		{
			m_fSizeX = 220.f;
			m_fSizeY = 100.f;
			m_fX = m_fSizeX * 0.5f;
			m_fY = m_fSizeY* 0.5f;
		}

		matWorld._11 = m_fSizeX;
		matWorld._22 = m_fSizeY;
		matWorld._33 = 1.f;
		matWorld._41 = m_fX - (WINCX >> 1);
		matWorld._42 = -m_fY + (WINCY >> 1);

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
		
		m_pTextureCom->Render_Texture(i);
		m_pBufferCom->Render_Buffer();
	}
	Render_GoldInfo();
	Render_StateInfo();
	//Render_AminingPoint();
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOriginView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOriginProj);	
}

HRESULT CUI::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_UI"));
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

void CUI::Render_GoldInfo(void)
{
	TCHAR szGoldInfo[MIN_STR];
	ZeroMemory(szGoldInfo, sizeof(szGoldInfo));

	swprintf_s(szGoldInfo, L"x %00d", m_iCurGold);
	Engine::Render_Font(L"Font_Default", szGoldInfo, &_vec2(90.f, 35.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

void CUI::Render_StateInfo(void)
{
	_vec2 vPos;
	_vec2 vSize;
	_matrix matWorld;
	D3DXMatrixIdentity(&matWorld);
	
	vSize.x = 210.f;
	vSize.y = 165.f;
	vPos.x = vSize.x * 0.5f;
	vPos.y = vSize.y* 0.5f + 100.f;

	matWorld._11 = vSize.x;
	matWorld._22 = vSize.y;
	matWorld._33 = 1.f;
	matWorld._41 = vPos.x - (WINCX >> 1);
	matWorld._42 = -vPos.y + (WINCY >> 1);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	if (true == m_bCombat)
		m_pTextureCom->Render_Texture(3);
	else
		m_pTextureCom->Render_Texture(2);
	m_pBufferCom->Render_Buffer();
}

//void CUI::Render_AminingPoint(void)
//{
//	_vec2 vPos;
//	_vec2 vSize;
//	_matrix matWorld;
//	D3DXMatrixIdentity(&matWorld);
//
//	vSize.x = 80.f;
//	vSize.y = 80.f;
//	vPos.x = WINCX >> 1;
//	vPos.y = WINCY >> 1;
//
//	matWorld._11 = vSize.x;
//	matWorld._22 = vSize.y;
//	matWorld._33 = 1.f;
//	matWorld._41 = vPos.x - (WINCX >> 1);
//	matWorld._42 = -vPos.y + (WINCY >> 1);
//
//	_matrix matRotZ;
//	D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(m_fAminingAngle));
//	matWorld *= matRotZ;
//
//	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
//
//	m_pTextureCom->Render_Texture(4);
//	m_pBufferCom->Render_Buffer();
//}

HRESULT CUI::Update_TargetInfo(void)
{
	auto pTarget = Get_Target();
	if (nullptr == pTarget)
		return E_FAIL;
	auto pStatus = (Engine::CStatus*)pTarget->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
	m_iCurGold = pStatus->m_iGold;
}

CUI * CUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CUI *	pInstance = new CUI(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CUI::Free(void)
{
	Engine::CGameObject::Free();
}



