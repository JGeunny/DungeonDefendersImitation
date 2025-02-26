#include "stdafx.h"
#include "Sword.h"

#include "Export_Function.h"

CSword::CSword(LPDIRECT3DDEVICE9 pGraphicDev)
	//: Engine::CGameObject(pGraphicDev)
	: CMeshObj(pGraphicDev)
{
}

CSword::~CSword(void)
{
}

HRESULT CSword::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(CMeshObj::Add_Component(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransCom->Set_Radian(_vec3(D3DXToRadian(-90.f), D3DXToRadian(180.f), D3DXToRadian(90.f)));

	m_iFlag = iFlag;
	
	return S_OK;
}

void CSword::LateReady_Object()
{
	auto pair = Engine::Get_ObjectMultimap(L"GameLogic")->equal_range(L"Player");
	if (pair.first == pair.second) return;
	Set_Target(pair.first->second);
}

_int CSword::Update_Object(const _float& fTimeDelta)
{
	CGameObject::LateReady_Object();
	auto pTarget = Get_Target();
	if (nullptr == pTarget) return EVENT_DEAD;

	if (nullptr == m_pParentBoneMatrix)
	{
		auto pPlayerMeshCom = (Engine::CMeshAdmin*)pTarget->Get_Component(L"Com_MeshAdmin", Engine::ID_STATIC);
		NULL_CHECK_RETURN(pPlayerMeshCom, -1);

		const Engine::D3DXFRAME_DERIVED*		pFrame = pPlayerMeshCom->Get_FrameByName("b_FingersR");

		m_pParentBoneMatrix = &pFrame->CombinedTransformationMatrix;

		auto pPlayerTransCom = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC);
		NULL_CHECK_RETURN(pPlayerTransCom, -1);

		m_pParentWorldMatrix = pPlayerTransCom->Get_WorldMatrixPointer();
	}

	CMeshObj::Update_Object(fTimeDelta);

	m_pTransCom->Set_ParentMatrix(&((*m_pParentBoneMatrix) * (*m_pParentWorldMatrix)));
	m_pRendererCom->Add_RenderGroup(m_eRenderType, this);

	return m_iEvent;
}

void CSword::LateUpdate_Object(void)
{
	CMeshObj::LateUpdate_Object();
}

void CSword::Render_Object(void)
{
	
	CMeshObj::Render_Object();
	//m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransCom->m_matWorld);
	
	//m_pMeshCom->Render_Meshes();

	//_matrix		matWorld;
	// 
	//// aabbÀÏ¶§
	//if (0 == m_iFlag)
	//	m_pTransCom->Get_NRotWorldMatrix(&matWorld);
	//else
	//	m_pTransCom->Get_WorldMatrix(&matWorld);

	//m_pColliderCom->Set_CollType(m_eCollType);
	//m_pColliderCom->Render_Collider(&matWorld);
}

HRESULT CSword::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	//// Mesh
	//pComponent = m_pMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(RESOURCE_STAGE, L"Staff"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	//// Transform
	//pComponent = m_pTransCom = Engine::CTransform::Create();
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);
	//m_pTransCom->Set_Scale(0.2f, 0.2f, 0.2f);

	//// Renderer
	//pComponent = m_pRendererCom = Engine::Get_Renderer();
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//pComponent->AddRef();
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	//// Calculator
	//pComponent = m_pCalculatorCom = Engine::CCalculator::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Calculator", pComponent);

	//// Collider
	//pComponent = m_pColliderCom = Engine::CCollider::Create(m_pGraphicDev);
	//m_pColliderCom->Change_Collider(m_pMeshCom->Get_Range());
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);

	m_pMeshAdminCom->Change_Mesh(RESOURCE_STAGE, L"Staff");
	m_pTransCom->Set_Scale(0.2f, 0.2f, 0.2f);

	return S_OK;
}

CSword * CSword::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CSword *	pInstance = new CSword(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CSword::Free(void)
{
	Engine::CGameObject::Free();
}

//_bool CSword::Collision_ToObject(const _tchar* pLayerTag, const _tchar* pObjTag)
//{
//	//Engine::CCollider*	pPlayerColliderCom = dynamic_cast<Engine::CCollider*>(Engine::Get_Component(pLayerTag, pObjTag, L"Com_Collider", Engine::ID_STATIC));
//	//NULL_CHECK_RETURN(pPlayerColliderCom, false);
//
//	//return m_pCalculatorCom->Collision_AABB(pPlayerColliderCom->Get_Min(), pPlayerColliderCom->Get_Max(), pPlayerColliderCom->Get_ColliderWorld(),
//	//										m_pColliderCom->Get_Min(), m_pColliderCom->Get_Max(), m_pColliderCom->Get_ColliderWorld());
//	return true;
//}

