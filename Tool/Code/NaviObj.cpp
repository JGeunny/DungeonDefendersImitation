#include "stdafx.h"
#include "NaviObj.h"

#include "Export_Function.h"

CNaviObj::CNaviObj(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CNaviObj::~CNaviObj(void)
{
}

HRESULT CNaviObj::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_iFlag = iFlag;
	
	return S_OK;
}

void CNaviObj::LateReady_Object(void)
{
	//Engine::Add_CollisionGroup(m_pColliderCom->Get_UsingCollision(), this);
}

_int CNaviObj::Update_Object(const _float& fTimeDelta)
{  
	CGameObject::LateReady_Object();
	Engine::CGameObject::Update_Object(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return m_iEvent;
}

void CNaviObj::LateUpdate_Object(void)
{               
}

void CNaviObj::Render_Object(void)
{
	//m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransCom->m_matWorld);
	m_pNaviMeshCom->Render_NaviMesh(&m_pTransCom->m_matWorld);
}

void CNaviObj::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
	m_eCollType = Engine::COLLTYPE::COL_TRUE;
}

std::vector<Engine::CCell*>* CNaviObj::Get_CellList()
{
	return m_pNaviMeshCom->Get_CellList();
}

HRESULT CNaviObj::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// ObejctKey
	pComponent = m_pObjectInfo = Engine::CObjectInfo::Create(L"NaviObj");
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_ObjectInfo", pComponent);

	//// Mesh
	//pComponent = m_pMeshAdminCom = Engine::CMeshAdmin::Create(&m_pMeshCom);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_MeshAdmin", pComponent);

	//m_pMeshAdminCom->Change_Mesh(RESOURCE_STAGE, L"Mesh_MapLast");

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	//// Calculator
	//pComponent = m_pCalculatorCom = Engine::CCalculator::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Calculator", pComponent);

	//// Collider
	//pComponent = m_pColliderCom = Engine::CCollider::Create(m_pGraphicDev, m_pMeshCom->Get_VtxPos(), m_pMeshCom->Get_VtxNum(), m_pMeshCom->Get_Stride());
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);
	//m_pColliderCom->Add_UsingCollision(COLLISION_BASE_DST);

	// NaviMesh
	
	pComponent = m_pNaviMeshCom = Engine::CNaviMesh::Create(m_pGraphicDev); //dynamic_cast<Engine::CNaviMesh*>(Engine::Clone(RESOURCE_STAGE, L"Mesh_Navi"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_NaviMesh", pComponent);

	return S_OK;
}

HRESULT CNaviObj::Add_Cell(Engine::CCell** ppCell, _vec3 vPointA, _vec3 vPointB, _vec3 vPointC)
{
	return m_pNaviMeshCom->Add_Cell(ppCell, vPointA, vPointB, vPointC);
}

HRESULT CNaviObj::Del_Cell(Engine::CCell * pCell)
{
	return m_pNaviMeshCom->Del_Cell(pCell);
}

CNaviObj * CNaviObj::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CNaviObj *	pInstance = new CNaviObj(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CNaviObj::Free(void)
{
	//Engine::Del_CollisionGroup(this);
	Engine::CGameObject::Free();
}
