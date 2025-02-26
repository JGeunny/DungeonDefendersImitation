#include "stdafx.h"
#include "HpBar.h"

#include "Export_Function.h"

#include "MonsterGate.h"

CHpBar::CHpBar(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
	m_eRenderType = Engine::RENDERID::RENDER_UI;
}

CHpBar::~CHpBar(void)
{
}

HRESULT CHpBar::Ready_Object(HPBAR_DESC tArg)
{
	m_tInfo = tArg;
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);
	return S_OK;
}

_int CHpBar::Update_Object(const _float& fTimeDelta)
{
	if (false == m_bActivate) return m_iEvent;
	Engine::CGameObject::Update_Object(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(m_eRenderType, this);
	return m_iEvent;
}

void CHpBar::LateUpdate_Object(void)
{
}

void CHpBar::Render_Object(void)
{
	LPD3DXEFFECT	pEffect = nullptr;

	pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);

	for (int i = 0; i < 2; ++i)//IMAGE_END; ++i)
	{
		SetUp_ConstantTable(pEffect, i);
		pEffect->AddRef();

		_uint iPassCnt = 0;
		pEffect->Begin(&iPassCnt, 0);			// 1인자 : 현재 쉐이더 파일의 최대 패스의 개수, 2인자 : 시작하는 방식에 대한 플래그 거의 0
		pEffect->BeginPass(0);					// 0패스의 쉐이더를 적용

		m_pBufferCom->Render_Buffer();

		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);
	}
	if (m_tInfo.strContent != L"")
	{
		auto vFontPos = m_tInfo.vPos;
		vFontPos.x -= 40.f;
		vFontPos.y -= 15.f;
		Engine::Render_Font(L"Font_Default", m_tInfo.strContent.c_str(), &vFontPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}
}

HRESULT CHpBar::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_UI_HpBar"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);
	//m_pTransCom->Set_Scale(_vec3(2.f, 0.3f, 2.f));

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Sample"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CHpBar::SetUp_ConstantTable(LPD3DXEFFECT pEffect, int iImageId)
{
	_float	fCurGage = 1.f;
	if (iImageId == 1)
		fCurGage = *m_tInfo.pfCurHp / *m_tInfo.pfMaxHp;
	_vec4	vColor = D3DXCOLOR(m_tInfo.dwColor);
	_vec3	vPos = m_tInfo.vPos;
	_vec3	vSize = m_tInfo.vSize;

	pEffect->AddRef();

	_matrix	matWorld, matView, matProj;
	m_pTransCom->Get_WorldMatrix(&matWorld);
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);	

	matWorld._11 = vSize.x;
	matWorld._22 = vSize.y;
	matWorld._33 = 1.f;
	matWorld._41 = vPos.x - (WINCX >> 1);
	matWorld._42 = -vPos.y + (WINCY >> 1);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &m_matProj);
	pEffect->SetInt("g_iType", 0);
	pEffect->SetFloat("g_fCurHp", fCurGage);

	pEffect->SetFloatArray("g_vColorHp", (float*)&vColor, 4);
	pEffect->SetBool("g_bGrayScale", Engine::Get_FrameMgr()->Get_GayScale());
	pEffect->SetBool("g_bVertical", false);

	m_pTextureCom->Render_Texture(pEffect, "g_BaseTexture", iImageId);

	Safe_Release(pEffect);
	return S_OK;
}

CHpBar * CHpBar::Create(LPDIRECT3DDEVICE9 pGraphicDev, HPBAR_DESC tArg)
{
	CHpBar *	pInstance = new CHpBar(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(tArg)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CHpBar::Free(void)
{
	Engine::CGameObject::Free();
}