#include "stdafx.h"
#include "CameraObj.h"
#include "ToolCamera.h"
#include "Export_Function.h"

CCameraObj::CCameraObj(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
	D3DXMatrixIdentity(&m_matView);
	m_vEye = { 0.f, 0.f, 0.f };
	m_vAt = { 0.f, 0.f, 0.f };
	m_vUp = { 0.f, 1.f, 0.f };
	m_iPlay_Index = 0;
	m_fPlay = 0.f;
}

CCameraObj::~CCameraObj(void)
{
}

HRESULT CCameraObj::Ready_Object(const _uint& iFlag)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_iFlag = iFlag;
	
	return S_OK;
}

void CCameraObj::LateReady_Object(void)
{
	//Engine::Add_CollisionGroup(m_pColliderCom->Get_UsingCollision(), this);
}

_int CCameraObj::Update_Object(const _float& fTimeDelta)
{  
	CGameObject::LateReady_Object();
	Engine::CGameObject::Update_Object(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	if (m_bPlay)
	{
		if (m_fPlay > 1.f)
		{
			m_fPlay = m_fPlay - int(m_fPlay);
			++m_iPlay_Index;
		}
		if (SUCCEEDED(Set_PlayIndex(&m_iPlay_Index)))
		{
			float fSpeed = 0.f;
			for (int iType = 0; iType < Engine::CAMERATYPE::CAMERATYPE_END; ++iType)
			{
				auto& pNode = m_vecNode[iType][m_iPlay_Index];
				_vec3 vPoint;
				D3DXVec3CatmullRom(&vPoint, &pNode->Get_Point(Engine::CCurve::POINT_A),
					&pNode->Get_Point(Engine::CCurve::POINT_B), &pNode->Get_Point(Engine::CCurve::POINT_C),
					&pNode->Get_Point(Engine::CCurve::POINT_D), m_fPlay);
				if (iType == 0)
					m_vEye = vPoint;
				else
					m_vAt = vPoint;
				if(iType == Engine::CAMERATYPE::CAMERATYPE_EYE)
					fSpeed = pNode->Get_Speed();
			}
			m_fPlay += fSpeed * fTimeDelta;

			D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
			m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);
		}
		else
		{
			m_iPlay_Index = 0;
			m_fPlay = 0.f;
			m_bPlay = false;
		}
	}

	return m_iEvent;
}

void CCameraObj::LateUpdate_Object(void)
{               
}

void CCameraObj::Render_Object(void)
{
	for (int iType = 0; iType < Engine::CAMERATYPE::CAMERATYPE_END; ++iType)
	{
		for (auto& pNode : m_vecNode[iType])
		{
			pNode->Render_Curve(&m_pTransCom->m_matWorld);
		}
	}
}

void CCameraObj::BeCollision(_ulong dwUsingCollision, Engine::CGameObject * pGameObject)
{
	m_eCollType = Engine::COLLTYPE::COL_TRUE;
}

void CCameraObj::Play_Camera()
{
	m_bPlay = true;
}

void CCameraObj::Stop_Camera()
{
	m_iPlay_Index = 0;
	m_fPlay = 0.f;
	m_bPlay = false;
}

void CCameraObj::Pause_Camera()
{
	m_bPlay = false;
}

HRESULT CCameraObj::Set_PlayIndex(_int * piOut_Index)
{
	auto iSize = (int)m_vecNode[Engine::CAMERATYPE::CAMERATYPE_EYE].size();
	if (iSize <= *piOut_Index) return E_FAIL;
	auto pNode = m_vecNode[Engine::CAMERATYPE::CAMERATYPE_EYE][*piOut_Index];
	if (false == pNode->IsClone())
	{
		++(*piOut_Index);
		if (iSize <= *piOut_Index) return E_FAIL;
	}
	return S_OK;
}

std::vector<Engine::CCurve*>* CCameraObj::Get_NodeList(Engine::CAMERATYPE eType)
{
	return &m_vecNode[eType];
}

HRESULT CCameraObj::Add_Node(Engine::CAMERATYPE eType, Engine::CCurve** ppNode, _vec3 vPoint)
{
	if (m_vecNode[eType].size() > 0)
	{
		*ppNode = m_vecNode[eType].back()->Clone(vPoint);
		(*ppNode)->AddRef();
	}
	else
		*ppNode = Engine::CCurve::Create(m_pGraphicDev, eType, &vPoint);

	m_vecNode[eType].emplace_back(*ppNode);
	return S_OK;
}

HRESULT CCameraObj::Del_Node(Engine::CAMERATYPE eType, Engine::CCurve * pNode)
{
	auto& iter = find_if(m_vecNode[eType].begin(), m_vecNode[eType].end(), [&](auto _pNode) {
							return _pNode == pNode;
						});
	if (iter != m_vecNode[eType].end())
	{
		Engine::Safe_Release(*iter);
		m_vecNode[eType].erase(iter);
	}
	return S_OK;
}

HRESULT CCameraObj::Add_Component(void)
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

	return S_OK;
}

CCameraObj * CCameraObj::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag)
{
	CCameraObj *	pInstance = new CCameraObj(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(iFlag)))
		Engine::Safe_Release(pInstance);
	return pInstance;
}

void CCameraObj::Free(void)
{
	//Engine::Del_CollisionGroup(this);
	Engine::CGameObject::Free();

	for (auto& vecNode : m_vecNode)
		for_each(vecNode.begin(), vecNode.end(), Engine::Safe_Release<Engine::CCurve*>);
}

