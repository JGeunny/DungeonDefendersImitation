#include "stdafx.h"
#include "DirectionObj.h"
#include "Export_Function.h"

CDirectionObj::CDirectionObj(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CDirectionObj::~CDirectionObj(void)
{
}

HRESULT CDirectionObj::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_iFlag = iFlag;
	
	return S_OK;
}

void CDirectionObj::LateReady_Object(void)
{
	//Engine::Add_CollisionGroup(m_pColliderCom->Get_UsingCollision(), this);
}

_int CDirectionObj::Update_Object(const _float& fTimeDelta)
{  
	CGameObject::LateReady_Object();
	Engine::CGameObject::Update_Object(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	return m_iEvent;
}

void CDirectionObj::LateUpdate_Object(void)
{               
}

void CDirectionObj::Render_Object(void)
{
	if (!Engine::Is_DebugRender(DEBUG_RENDER_DIRECTION)) return;
	for (auto& pNode : m_vecNode)
	{
		pNode->Render_Curve(&m_pTransCom->m_matWorld);
	}
}

void CDirectionObj::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
}

_vec3 CDirectionObj::Get_DirectPos(_int iDirectionIdx)
{
	int idx = iDirectionIdx / 10;
	if ((int)m_vecNode.size() <= idx || 0 > idx)
		return _vec3(0.f, 0.f, 0.f);
	_float fPlay = (iDirectionIdx - idx * 10) * 0.1f;
	_vec3 vPos;
	auto pNode = m_vecNode[idx];
	D3DXVec3CatmullRom(&vPos, &pNode->Get_Point(Engine::CCurve::POINT_A),
		&pNode->Get_Point(Engine::CCurve::POINT_B), &pNode->Get_Point(Engine::CCurve::POINT_C),
		&pNode->Get_Point(Engine::CCurve::POINT_D), fPlay);

	return vPos;
}

_bool CDirectionObj::Is_DirectionIdx(_int iDirectionIdx)
{
	int idx = iDirectionIdx / 10;
	if ((int)m_vecNode.size() <= idx || 0 > idx)
		return false;
	return true;
}

std::vector<Engine::CCurve*>* CDirectionObj::Get_NodeList()
{
	return &m_vecNode;
}

HRESULT CDirectionObj::Add_Node(Engine::CCurve** ppNode, _vec3 vPoint)
{
	if (m_vecNode.size() > 0)
	{
		*ppNode = m_vecNode.back()->Clone(vPoint);
		(*ppNode)->AddRef();
	}
	else
		*ppNode = Engine::CCurve::Create(m_pGraphicDev, Engine::CAMERATYPE::CAMERATYPE_EYE, &vPoint);

	m_vecNode.emplace_back(*ppNode);
	return S_OK;
}

HRESULT CDirectionObj::Del_Node(Engine::CCurve * pNode)
{
	auto& iter = find_if(m_vecNode.begin(), m_vecNode.end(), [&](auto _pNode) {
							return _pNode == pNode;
						});
	if (iter != m_vecNode.end())
	{
		Engine::Safe_Release(*iter);
		m_vecNode.erase(iter);
	}
	return S_OK;
}

HRESULT CDirectionObj::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// ObejctKey
	pComponent = m_pObjectInfo = Engine::CObjectInfo::Create(L"NaviObj");
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_ObjectInfo", pComponent);

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

CDirectionObj * CDirectionObj::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CDirectionObj *	pInstance = new CDirectionObj(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
		Engine::Safe_Release(pInstance);
	return pInstance;
}

void CDirectionObj::Free(void)
{
	Engine::CGameObject::Free();
	for_each(m_vecNode.begin(), m_vecNode.end(), Engine::Safe_Release<Engine::CCurve*>);
}

