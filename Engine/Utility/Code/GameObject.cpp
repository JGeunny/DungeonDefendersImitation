#include "GameObject.h"
#include "Transform.h"

USING(Engine)

Engine::CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_iEvent(EVENT_DEFAULT)
	, m_bLateReady(false)
	, m_bSelected(false)
	, m_pTarget(nullptr)
	, m_bActivate(true)
	, m_fTimeDelta(0.f)
	, m_eTeam(Engine::TEAMTYPE::TEAMTYPE_NONE)
	, m_eRenderType(Engine::RENDERID::RENDER_NONALPHA)
	, m_eCollType(Engine::COLLTYPE::COL_FALSE)
{
	m_pGraphicDev->AddRef();
}

Engine::CGameObject::~CGameObject(void)
{
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag, COMPONENTID eID)
{
	auto	iter = find_if(m_mapComponent[eID].begin(), m_mapComponent[eID].end(), CTag_Finder(pComponentTag));

	if (iter == m_mapComponent[eID].end())
		return nullptr;
	
	return iter->second;
}

CComponent * CGameObject::Get_Component(const _tchar * pComponentTag, COMPONENTID eID)
{
	CComponent*		pComponent = Find_Component(pComponentTag, eID);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

void CGameObject::LateReady_Object(void)
{
	if (!m_bLateReady)
	{
		m_bLateReady = true;
		this->LateReady_Object();
	}
}

_int Engine::CGameObject::Update_Object(const _float& fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;
	if(m_eCollType != Engine::COLLTYPE::COL_SELECT)
		m_eCollType = Engine::COLLTYPE::COL_FALSE;
	for (auto& iter : m_mapComponent[ID_DYNAMIC])
		iter.second->Update_Component(fTimeDelta);

	return 0;
}

void CGameObject::Render_Object(void)
{
	_D3DFILLMODE eFillMode;
	m_pGraphicDev->GetRenderState(D3DRS_FILLMODE, (DWORD*)&eFillMode);
	if (m_bSelected)
		m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	this->Render_Object();

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, eFillMode);

	//_matrix matView, matProj;
	//_vec3 vPos;
	//auto pComponent = Get_Component(L"Com_Transform", Engine::ID_DYNAMIC);
	//if (pComponent)
	//{
	//	auto pTransCom = (Engine::CTransform*)pComponent;
	//	m_pGraphicDev->GetTransform(D3DTRANSFORMSTATETYPE::D3DTS_VIEW, &matView);
	//	m_pGraphicDev->GetTransform(D3DTRANSFORMSTATETYPE::D3DTS_PROJECTION, &matProj);

	//	matProj = matView * matProj;
	//	pTransCom->Get_Info(Engine::INFO::INFO_POS, &vPos);

	//	D3DXVec3TransformCoord(&vPos, &vPos, &matProj);

	//	RECT rcTemp;

	//	if (vPos.x > -1 && vPos.x < 1 && vPos.y > -1 && vPos.y < 1)
	//	{
	//		this->Render_Object();
	//	}
	//}
}

void CGameObject::Compare_ViewZ(const _vec3 * pPos)
{
	_matrix		matView;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);

	_vec3	vCamPos;
	memcpy(&vCamPos, &matView.m[3][0], sizeof(_vec3));

	m_fViewZ = D3DXVec3Length(&(vCamPos - *pPos));
}

void CGameObject::Set_Target(CGameObject * pTarget, bool bTargetRef)
{
	if (m_pTarget == pTarget) return;

	if (nullptr != m_pTarget && true == m_bTargetRef)
		Safe_Release(m_pTarget);
	m_bTargetRef = bTargetRef;
	m_pTarget = pTarget;
	if(nullptr != m_pTarget && true == m_bTargetRef)
		m_pTarget->AddRef();
}

CGameObject * CGameObject::Get_Target()
{
	if (nullptr != m_pTarget)
	{
		if (EVENT_DEAD == m_pTarget->Get_Event())
		{
			if(true == m_bTargetRef)
				Safe_Release(m_pTarget);
			m_pTarget = nullptr;
		}
	}
	return m_pTarget;
}

void Engine::CGameObject::Free(void)
{
	if (nullptr != m_pTarget && true == m_bTargetRef)
		Safe_Release(m_pTarget);
	m_pTarget = nullptr;

	for (_uint i = 0; i < ID_END; ++i)
	{
		for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CDeleteMap());
		m_mapComponent[i].clear();
	}

	Safe_Release(m_pGraphicDev);
}

