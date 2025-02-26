#include "stdafx.h"
#include "Stone.h"

#include "Export_Function.h"

CStone::CStone(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CStone::~CStone(void)
{
}

HRESULT CStone::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->Rotation(Engine::ROT_Y, 45.f);

	m_pTransCom->Set_Pos(rand() % 5 + 1.f, 0.f, rand() % 10 + 1.f);

	m_iFlag = iFlag;
	
	return S_OK;
}

void CStone::LateReady_Object(void)
{
	Engine::Add_CollisionGroup(m_pColliderCom->Get_UsingCollision(), this);
}

_int CStone::Update_Object(const _float& fTimeDelta)
{  
	CGameObject::LateReady_Object();
	Engine::CGameObject::Update_Object(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	
	SetUp_OnTerrain();


	return 0;
}

void CStone::LateUpdate_Object(void)
{               
}

void CStone::Render_Object(void)
{
	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransCom->m_matWorld);
	
	m_pMeshCom->Render_Meshes();

	_matrix		matWorld;
	 
	// aabbÀÏ¶§
	//if (0 == m_iFlag)
	//	m_pTransCom->Get_NRotWorldMatrix(&matWorld);
	//else
	m_pTransCom->Get_WorldMatrix(&matWorld);

	m_pColliderCom->Set_CollType(m_eCollType);
	m_pColliderCom->Render_Collider(&matWorld);
}

void CStone::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
	m_eCollType = Engine::COLLTYPE::COL_TRUE;
}

HRESULT CStone::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(RESOURCE_STAGE, L"Mesh_Stone"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Calculator
	pComponent = m_pCalculatorCom = Engine::CCalculator::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Calculator", pComponent);

	// Collider
	pComponent = m_pColliderCom = Engine::CCollider::Create(m_pGraphicDev);
	m_pColliderCom->Change_Collider(m_pMeshCom->Get_Range());
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);
	m_pColliderCom->Add_UsingCollision(COLLISION_BASE_DST);

	return S_OK;
}

CStone * CStone::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CStone *	pInstance = new CStone(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CStone::Free(void)
{
	Engine::Del_CollisionGroup(this);
	Engine::CGameObject::Free();
}

void CStone::Key_Input(const _float& fTimeDelta)
{
	if (Engine::KeyPressing(KEY_UP))
		m_pTransCom->m_vInfo[Engine::INFO_POS] += m_pTransCom->m_vInfo[Engine::INFO_LOOK] * m_fSpeed * fTimeDelta;

	if (Engine::KeyPressing(KEY_DOWN))
		m_pTransCom->m_vInfo[Engine::INFO_POS] -= m_pTransCom->m_vInfo[Engine::INFO_LOOK] * m_fSpeed * fTimeDelta;


	if (Engine::KeyPressing(KEY_LEFT))
	{
		m_pTransCom->Rotation(Engine::ROT_Y, D3DXToRadian(90.f * fTimeDelta));
	}

	if (Engine::KeyPressing(KEY_RIGHT))
	{
		m_pTransCom->Rotation(Engine::ROT_Y, D3DXToRadian(-90.f * fTimeDelta));
	}

	if (Engine::KeyPressing(KEY_LBUTTON))
	{
		_vec3		vPickPos = PickUp_OnTerrain();

		_vec3		vDir = vPickPos - m_pTransCom->m_vInfo[Engine::INFO_POS];
		D3DXVec3Normalize(&vDir, &vDir);
		m_pTransCom->Move_Pos(&(vDir * m_fSpeed * fTimeDelta));
	}


}

void CStone::SetUp_OnTerrain(void)
{
	_vec3	vPos;

	m_pTransCom->Get_Info(Engine::INFO_POS, &vPos);

	Engine::CTerrainTex*	pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>
		(Engine::Get_Component(L"GameLogic", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK(pTerrainBufferCom);

	_float	fHeight = m_pCalculatorCom->Compute_HeightTerrain(&vPos, 
																pTerrainBufferCom->Get_VtxPos(), 
																129, 
																129);

	m_pTransCom->Set_Pos(vPos.x, fHeight, vPos.z);
}

_vec3 CStone::PickUp_OnTerrain(void)
{
	Engine::CTerrainTex*	pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>
		(Engine::Get_Component(L"GameLogic", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK_RETURN(pTerrainBufferCom, _vec3(0.f, 0.f, 0.f));

	Engine::CTransform*		pTerrainTransCom = dynamic_cast<Engine::CTransform*>
		(Engine::Get_Component(L"GameLogic", L"Terrain", L"Com_Transform", Engine::ID_DYNAMIC));
	NULL_CHECK_RETURN(pTerrainTransCom, _vec3(0.f, 0.f, 0.f));

	return m_pCalculatorCom->Pick_OnTerrain(g_hWnd, pTerrainBufferCom, pTerrainTransCom);
}


