#include "stdafx.h"
#include "Gold.h"

#include "Export_Function.h"

CGold::CGold(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_iCurrentImageCount(0)
	, m_fTimeDelta(0)
	, m_fCurHp(0.f)
	, m_vColorHp(1.f, 1.f, 1.f, 1.f)
{
}

CGold::~CGold(void)
{
}

HRESULT CGold::Ready_Object(Engine::CGameObject* pTarget)
{
	NULL_CHECK_RETURN(pTarget, E_FAIL);
	Set_Target(pTarget);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	return S_OK;
}

void CGold::LateReady_Object(void)
{
	m_vBeginPos = m_pTransCom->Get_Pos();
	m_vBeginPos.y += 1.f;
	m_pTransCom->Set_Pos(m_vBeginPos);
}

_int CGold::Update_Object(const _float& fTimeDelta)
{
	CGameObject::LateReady_Object();
	auto pTarget = Get_Target();
	if (nullptr == pTarget)
	{
		Engine::Get_SoundMgr()->StartSound(L"Ok_MiniGame", Engine::CSoundMgr::SE, false);
		m_iEvent = EVENT_DEAD;
		return m_iEvent;
	}

	Movement(fTimeDelta);
	Engine::CGameObject::Update_Object(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);

	auto fDist = D3DXVec3Length(&(m_vBeginPos - m_pTransCom->Get_Pos()));
	if (fDist > 5.f)
		m_bFollowPlayer = true;
	return m_iEvent;
}

void CGold::LateUpdate_Object(void)
{
}

void CGold::Render_Object(void)
{
	//BillBoard();
	//_matrix matWorld;
	//m_pTransCom->Get_WorldMatrix(&matWorld);
	//m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);

	SetUp_ConstantTable(pEffect);
	pEffect->AddRef();

	_uint iPassCnt = 0;
	pEffect->Begin(&iPassCnt, 0);			// 1인자 : 현재 쉐이더 파일의 최대 패스의 개수, 2인자 : 시작하는 방식에 대한 플래그 거의 0
	pEffect->BeginPass(0);					// 0패스의 쉐이더를 적용

	m_pMeshCom->Render_Meshes(pEffect);

	pEffect->EndPass();
	pEffect->End();
	Safe_Release(pEffect);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransCom->m_matWorld);
	_matrix		matWorld;
	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pColliderCom->Set_CollType(m_eCollType);
	m_pColliderCom->Render_Collider(&matWorld);
}

void CGold::Movement(const _float & fTimeDelta)
{
	_vec3 vPos, vLook;
	vPos = m_pTransCom->Get_Pos();
	if (false == m_bFollowPlayer)
	{
		m_pTransCom->Get_Info(Engine::INFO::INFO_LOOK, &vLook);
		D3DXVec3Normalize(&vLook, &vLook);
		m_pTransCom->Set_Pos(vPos + vLook * m_fSpeed * fTimeDelta);
	}
	else
	{
		auto pTarget = Get_Target();
		auto pTrans_Target = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
		if (nullptr != pTrans_Target)
		{
			auto vDir = pTrans_Target->Get_Pos() - vPos;
			vDir.y += 1.f;
			D3DXVec3Normalize(&vLook, &vDir);
			auto fDist = D3DXVec3Length(&vDir);
			if (fDist < 2.f)
			{
				auto pStatus_Target = (Engine::CStatus*)pTarget->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
				pStatus_Target->m_iGold += 1;
				Set_Target(nullptr);
			}
		}
		m_pTransCom->Set_Pos(vPos + vLook * m_fSpeed * fTimeDelta * 4.f);
	}
}

HRESULT CGold::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	//// Buffer
	//pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	//// Texture
	//pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Gold"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);
	m_pTransCom->Set_Scale(_vec3(0.005f, 0.005f, 0.005f));

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	// Collider
	pComponent = m_pColliderCom = Engine::CCollider::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);
	
	pComponent = m_pMeshAdminCom = Engine::CMeshAdmin::Create(&m_pMeshCom, &m_pColliderCom);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_MeshAdmin", pComponent);
	m_pMeshAdminCom->Change_Mesh(RESOURCEID::RESOURCE_STAGE, L"EnergyBall");
	//m_pColliderCom->Add_UsingCollision(COLLISION_BASE_DST);

	return S_OK;
}

HRESULT CGold::FollowTarget()
{
	auto pTarget = Get_Target();
	NULL_CHECK_RETURN(pTarget, E_FAIL);
	auto pTrans_Target = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	auto pCollider_Target = (Engine::CCollider*)pTarget->Get_Component(L"Com_Collider", Engine::COMPONENTID::ID_STATIC);
	auto pStatus = (Engine::CStatus*)pTarget->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
	auto vPos = pTrans_Target->Get_Pos();
	auto vVertical = pCollider_Target->Get_Range()->Get_Vertical() * 2.f + pTrans_Target->m_vInterval;
	vPos.y += vVertical.y * pTrans_Target->Get_Scale().y;
	m_pTransCom->Set_Pos(vPos);

	return S_OK;
}

HRESULT CGold::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix		matWorld, matView, matProj;

	m_pTransCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	//const D3DLIGHT9*		pLightInfo = Engine::Get_LightInfo();
	//NULL_CHECK_RETURN(pLightInfo, E_FAIL);

	//pEffect->SetVector("g_vLightDir", &_vec4(pLightInfo->Direction, 0.f));
	//pEffect->SetVector("g_vLightDiffuse", (_vec4*)&pLightInfo->Diffuse);
	//pEffect->SetVector("g_vLightAmbient", (_vec4*)&pLightInfo->Ambient);
	//pEffect->SetVector("g_vLightSpecular", (_vec4*)&pLightInfo->Specular);
	pEffect->SetBool("g_bGrayScale", Engine::Get_FrameMgr()->Get_GayScale());

	D3DMATERIAL9			tMtrlInfo;
	ZeroMemory(&tMtrlInfo, sizeof(D3DMATERIAL9));

	tMtrlInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrlInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrlInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrlInfo.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrlInfo.Power = 10.f;

	pEffect->SetVector("g_vMtrlDiffuse", (_vec4*)&tMtrlInfo.Diffuse);
	pEffect->SetVector("g_vMtrlAmbient", (_vec4*)&tMtrlInfo.Ambient);
	pEffect->SetVector("g_vMtrlSpecular", (_vec4*)&tMtrlInfo.Specular);
	pEffect->SetFloat("g_fPower", tMtrlInfo.Power);
	pEffect->SetFloat("g_fType", 4.f);

	D3DXMatrixInverse(&matView, NULL, &matView);
	pEffect->SetVector("g_vCamPos", (_vec4*)&matView._41);

	Safe_Release(pEffect);
	return S_OK;
}

CGold * CGold::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pTarget)
{
	CGold *	pInstance = new CGold(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pTarget)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CGold::Free(void)
{
	Engine::CGameObject::Free();
}

void CGold::BillBoard(void)
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
	*(_vec4*)&matBill.m[3] = _vec4(0.f, 0.f, 0.f, 1.f);

	D3DXMatrixInverse(&matBill, 0, &matBill);

	m_pTransCom->Set_BillBoardMatrix(&(matBill * matWorld));
}
