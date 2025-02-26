#include "stdafx.h"
#include "TitleUI.h"

#include "Export_Function.h"

CTitleUI::CTitleUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
	m_bActivate = false;
}

CTitleUI::~CTitleUI(void)
{
}

HRESULT CTitleUI::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);
	m_eRenderType = Engine::RENDERID::RENDER_UI;
	
	Ready_UI();

	m_vPos.x = WINCX >> 1;
	m_vPos.y = 0.f;

	return S_OK;
}

_int CTitleUI::Update_Object(const _float& fTimeDelta)
{
	if (false == m_bActivate) return EVENT_DEFAULT;

	Engine::CGameObject::Update_Object(fTimeDelta);
	Update_Bounce(fTimeDelta);
	for (auto& tUIInfo : m_vecUI)
		Update_UI(tUIInfo);

	m_pRendererCom->Add_RenderGroup(m_eRenderType, this);
	return 0;
}

void CTitleUI::LateUpdate_Object(void)
{
}

void CTitleUI::Render_Object(void)
{
	_matrix		matWorld, matView, matOriginView, matOriginProj;

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matOriginView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOriginProj);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	for (auto& tUIInfo : m_vecUI)
		Render_UI(tUIInfo);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOriginView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOriginProj);	
}

HRESULT CTitleUI::Ready_UI(void)
{
	UI_DESC tUI;
	m_vecUI.clear();
	m_vecUI.reserve(UI_END);

	ZeroMemory(&tUI, sizeof(tUI));
	tUI.vSize = _vec2(500.f, 300.f);
	tUI.vPos = _vec2(m_vPos.x, m_vPos.y);
	tUI.iTextureIndex = UI_TITLE;
	m_vecUI.emplace_back(tUI);

	ZeroMemory(&tUI, sizeof(tUI));
	tUI.vSize = _vec2(250.f, 80.f);
	tUI.vPos = _vec2(m_vPos.x, m_vPos.y + 200.f);
	tUI.iTextureIndex = UI_GAMESTART;
	m_vecUI.emplace_back(tUI);

	ZeroMemory(&tUI, sizeof(tUI));
	tUI.vSize = _vec2(250.f, 80.f);
	tUI.vPos = _vec2(m_vPos.x, m_vPos.y + 300);
	tUI.iTextureIndex = UI_GAMEEND;
	m_vecUI.emplace_back(tUI);

	return S_OK;
}

HRESULT CTitleUI::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_TitleUI"));
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

HRESULT CTitleUI::Update_UI(UI_DESC & tUIInfo)
{
	switch (tUIInfo.iTextureIndex)
	{
	case UI_TITLE:
		tUIInfo.vPos = m_vPos;
		break;
	case UI_GAMESTART:
		tUIInfo.vPos = m_vPos;
		tUIInfo.vPos.y += 240.f;
		break;
	case UI_GAMEEND:
		tUIInfo.vPos = m_vPos;
		tUIInfo.vPos.y += 335.f;
		break;
	}

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	RECT rcSize;
	rcSize.left = tUIInfo.vPos.x - (tUIInfo.vSize.x * 0.5f);
	rcSize.right = tUIInfo.vPos.x + (tUIInfo.vSize.x * 0.5f);
	rcSize.top = tUIInfo.vPos.y - (tUIInfo.vSize.y * 0.5f);
	rcSize.bottom = tUIInfo.vPos.y + (tUIInfo.vSize.y * 0.5f);
	
	BOOL bOk = PtInRect(&rcSize, ptMouse);

	if (bOk == TRUE)
	{
		if (false == tUIInfo.m_bMouseOver)
			Engine::Get_SoundMgr()->StartSound(L"ButtonMouseOver", Engine::CSoundMgr::ENV, false);
		tUIInfo.m_bMouseOver = true;
		if (Engine::KeyDown(KEY_LBUTTON))
		{
			Engine::Get_SoundMgr()->StartSound(L"ButtonClick", Engine::CSoundMgr::ENV, false);
			switch (tUIInfo.iTextureIndex)
			{
			case UI_TITLE:
				break;
			case UI_GAMESTART:
				m_bGameStart = true;
				break;
			case UI_GAMEEND:
				m_bGameEnd = true;
				break;
			}
		}
	}
	else
		tUIInfo.m_bMouseOver = false;

	return S_OK;
}

HRESULT CTitleUI::Render_UI(UI_DESC& tUIInfo)
{
	_vec2 vPos = tUIInfo.vPos;
	_vec2 vSize = tUIInfo.vSize;
	_matrix matWorld;
	D3DXMatrixIdentity(&matWorld);

	if (true == tUIInfo.m_bMouseOver)
	{
		switch (tUIInfo.iTextureIndex)
		{
		case UI_GAMESTART:
			vSize *= 1.1f;
			break;
		case UI_GAMEEND:
			vSize *= 1.1f;
			break;
		}
	}
	matWorld._11 = vSize.x;
	matWorld._22 = vSize.y;
	matWorld._33 = 1.f;
	matWorld._41 = vPos.x - (WINCX >> 1);
	matWorld._42 = -vPos.y + (WINCY >> 1);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Render_Texture(tUIInfo.iTextureIndex);
	m_pBufferCom->Render_Buffer();

	return S_OK;
}

HRESULT CTitleUI::Update_Bounce(float fTimeDelta)
{
	if (m_iBounceCount > 3.f)
		return E_FAIL;

	_vec2 vDir = _vec2(0.f, 0.f);
	vDir.y += (m_fBouncePower - 12.f * m_fJumpAccel) * fTimeDelta* 8.f;
	m_fJumpAccel += 8.f*fTimeDelta;
	m_vPos -= vDir;

	if (m_iBounceCount == 3)
	{
		if (m_vPos.y < WINCY * 0.4f)
		{
			++m_iBounceCount;
			m_fJumpAccel = 0.f;
		}
	}
	else
	{
		if (m_vPos.y > WINCY * 0.5f)
		{
			Engine::Get_SoundMgr()->StartSound(L"Fall_TitleUI", Engine::CSoundMgr::ENV, false);
			++m_iBounceCount;
			switch (m_iBounceCount)
			{
			case 1:
				m_fBouncePower = 70.f;
				break;
			case 2:
				m_fBouncePower = 60.f;
				break;
			case 3:
				m_fBouncePower = 50.f;
				break;
			default:
				m_fBouncePower = 50.f;
				break;
			}
			m_fJumpAccel = 0.f;
		}
	}
	return S_OK;
}

CTitleUI * CTitleUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTitleUI *	pInstance = new CTitleUI(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CTitleUI::Free(void)
{
	Engine::CGameObject::Free();
}



