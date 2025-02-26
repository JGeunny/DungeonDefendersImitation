#include "stdafx.h"
#include "PickPoint.h"

#include "Export_Function.h"

CPickPoint::CPickPoint(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CPickPoint::~CPickPoint(void)
{
}

HRESULT CPickPoint::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_iFlag = iFlag;

	return S_OK;
}

void CPickPoint::LateReady_Object(void)
{
	//Engine::Add_CollisionGroup(m_pColliderCom->Get_UsingCollision(), this);
}

_int CPickPoint::Update_Object(const _float& fTimeDelta)
{
	CGameObject::LateReady_Object();
	Engine::CGameObject::Update_Object(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return m_iEvent;
}

void CPickPoint::LateUpdate_Object(void)
{
}

void CPickPoint::Render_Object(void)
{
	Engine::Render_Texture(RESOURCE_STATIC, L"Texture_Default", 0);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransCom->m_matWorld);

	_matrix		matWorld;
	m_pTransCom->Get_WorldMatrix(&matWorld);

	m_pColliderCom->Set_CollType(m_eCollType);
	m_pColliderCom->Render_Collider(&matWorld);

	TCHAR	szBuf[MIN_STR];
	swprintf_s(szBuf, L"ÀÎµ¦½º: %d", m_iFlag);
	float fY = 100.f;
	fY += 50 * m_iFlag;
	Engine::Render_Font(L"Font_Default", szBuf, &_vec2(5.f, fY), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

void CPickPoint::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
	m_eCollType = Engine::COLLTYPE::COL_TRUE;
}

HRESULT CPickPoint::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// ObejctKey
	pComponent = m_pObjectInfo = Engine::CObjectInfo::Create(L"MeshObj");
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_ObjectInfo", pComponent);

	// Collider
	pComponent = m_pColliderCom = Engine::CCollider::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);
	//m_pColliderCom->Add_UsingCollision(COLLISION_BASE_DST);
	m_pColliderCom->Set_CollMeshType(Engine::COLLMESHTYPE_SPHERE);
	Engine::RANGE vRange;
	vRange.vMin = _vec3(-0.3f, -0.3f, -0.3f);
	vRange.vMax = _vec3(0.3f, 0.3f, 0.3f);
	m_pColliderCom->Change_Collider(vRange);

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

	return S_OK;
}

CPickPoint * CPickPoint::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CPickPoint *	pInstance = new CPickPoint(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CPickPoint::Free(void)
{
	//Engine::Del_CollisionGroup(this);
	Engine::CGameObject::Free();
}
