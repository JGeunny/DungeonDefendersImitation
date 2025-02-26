#include "stdafx.h"
#include "Damage.h"

#include "Export_Function.h"

CDamage::CDamage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_fMoveY(0.f)
	, m_fJumpAccel(0.f)
	, m_fJumpPower(3.f)
	, m_vMovePos(0.f, 0.f, 0.f)
{
}

CDamage::~CDamage(void)
{
}

HRESULT CDamage::Ready_Object(Engine::CGameObject* pTarget, _float fDamage)
{
	NULL_CHECK_RETURN(pTarget, E_FAIL);
	Set_Target(pTarget);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_fDamage = fDamage;
	if (pTarget->Get_Team() == Engine::TEAMTYPE_TEAM1)
	{
		m_dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
		m_dwColorBorder = D3DCOLOR_ARGB(255, 36, 15, 116);
	}
	else
	{
		m_dwColor = D3DCOLOR_ARGB(255, 255, 132, 0);
		m_dwColorBorder = D3DCOLOR_ARGB(255, 36, 15, 116);
	}
	FollowTarget();
	return S_OK;
}

void CDamage::LateReady_Object(void)
{
	_matrix matRotY;
	D3DXMatrixRotationY(&matRotY,D3DXToRadian(rand()%360));
	m_vDir = _vec3(1.f, 0.f, 0.f);
	D3DXVec3TransformNormal(&m_vDir, &m_vDir, &matRotY);
	D3DXVec3Normalize(&m_vDir, &m_vDir);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);
}

_int CDamage::Update_Object(const _float& fTimeDelta)
{
	Engine::CGameObject::LateReady_Object();
	if (m_vMovePos.y < -3.f)
	{
		m_iEvent = EVENT_DEAD;
		return m_iEvent;
	}
	FollowTarget();

	auto vPos = m_vTargetPos;
	vPos.y += m_fMoveY;
	m_pTransCom->Set_Pos(vPos);
	
	_vec3 vDir = m_vDir;
	vDir *= 4.f * fTimeDelta;
	m_fMoveY = (m_fJumpPower - 4.5f * m_fJumpAccel) * fTimeDelta*0.8f;
	vDir.y += m_fMoveY;
	m_vMovePos += vDir;
	m_pTransCom->Move_Pos(&m_vMovePos);
	m_fJumpAccel += 2.f*fTimeDelta;

	Engine::CGameObject::Update_Object(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_UI, this);
	return m_iEvent;
}

void CDamage::LateUpdate_Object(void)
{
}

void CDamage::Render_Object(void)
{	
	_matrix		matWorld, matView, matBill;

	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);
	matBill = matView;
	*((_vec4*)&matBill.m[3]) = _vec4(0.f, 0.f, 0.f, 1.f);

	D3DXMatrixInverse(&matBill, 0, &matBill);
	auto& vPos = *((_vec3*)&matWorld.m[3]);
	auto vPos_Begine = vPos;
	int number = (int)m_fDamage;
	
	list<int> lstNumber;
	while (number != 0)
	{
		lstNumber.emplace_front(number % 10);
		number /= 10;
	}
	_vec3 vRight = *(_vec3*)&matBill.m[0];
	float fSize = -1 * (float)lstNumber.size() * 0.5f;
	for (auto& iNumber : lstNumber)
	{
		vPos = vPos_Begine + vRight * fSize * 0.5f;
		++fSize;

		LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
		NULL_CHECK(pEffect);
		
		SetUp_ConstantTable(pEffect, m_pTextureCom, iNumber, matBill*matWorld);
		pEffect->AddRef();

		_uint iPassCnt = 0;
		pEffect->Begin(&iPassCnt, 0);
		pEffect->BeginPass(0);

		m_pBufferCom->Render_Buffer();

		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);
	}
}

HRESULT CDamage::SetUp_ConstantTable(LPD3DXEFFECT pEffect, Engine::CTexture * pTexture, int iTextureIndex, _matrix matWorld)
{
	pEffect->AddRef();
	_vec4 vColor = D3DXCOLOR(m_dwColor);
	_vec4 vColorBorder = D3DXCOLOR(m_dwColorBorder);
	_matrix		matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fCurHp", 1.f);
	
	pEffect->SetInt("g_iType", 2);
	pEffect->SetFloatArray("g_vColorHp", (float*)&vColor, 4);
	pEffect->SetFloatArray("g_vColorBorder", (float*)&vColorBorder, 4);
	pEffect->SetBool("g_bGrayScale", Engine::Get_FrameMgr()->Get_GayScale());
	pEffect->SetBool("g_bVertical", false);
	pTexture->Render_Texture(pEffect, "g_BaseTexture", iTextureIndex);

	Safe_Release(pEffect);
	return S_OK;
}

HRESULT CDamage::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);
	m_pTransCom->Set_Scale(_vec3(0.5f, 0.5f, 0.5f));

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Sample"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Number"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	return S_OK;
}

HRESULT CDamage::FollowTarget()
{
	auto pTarget = Get_Target();
	if (nullptr == pTarget)
		return E_FAIL;
	auto pTrans_Target = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	auto pCollider_Target = (Engine::CCollider*)pTarget->Get_Component(L"Com_Collider", Engine::COMPONENTID::ID_STATIC);
	auto pStatus = (Engine::CStatus*)pTarget->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
	auto vPos = pTrans_Target->Get_Pos();
	auto vVertical = pCollider_Target->Get_Range()->Get_Vertical() * 2.f + pTrans_Target->m_vInterval;
	//vPos.y += (vVertical.y + m_fMoveY) * pTrans_Target->Get_Scale().y;
	//m_pTransCom->Set_Pos(vPos);
	vPos.y += vVertical.y* pTrans_Target->Get_Scale().y;
	m_vTargetPos = vPos;
	
	return S_OK;
}

CDamage * CDamage::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pTarget, _float fDamage)
{
	CDamage *	pInstance = new CDamage(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pTarget, fDamage)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CDamage::Free(void)
{
	Engine::CGameObject::Free();
}

void CDamage::BillBoard(void)
{
	_matrix		matWorld, matView, matBill;

	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixIdentity(&matBill);

	//matBill._11 = matView._11;
	//matBill._13 = matView._13;
	//matBill._31 = matView._31;
	//matBill._33 = matView._33;
	matBill = matView;
	*((_vec4*)&matBill.m[3]) = _vec4(0.f, 0.f, 0.f, 1.f);

	D3DXMatrixInverse(&matBill, 0, &matBill);

	m_pTransCom->Set_BillBoardMatrix(&(matBill * matWorld));
}
