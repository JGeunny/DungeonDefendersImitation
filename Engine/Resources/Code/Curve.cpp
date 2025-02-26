#include "Curve.h"
#include "Export_Utility.h"
#include "Export_Resources.h"

USING(Engine)

Engine::CCurve::CCurve(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_pD3DXLine(nullptr)
	, m_pColliderCom(nullptr)
	, m_eCurveType(CAMERATYPE_EYE)
	, m_dwColor(0)
	, m_eCollType(Engine::COLLTYPE::COL_FALSE)
	, m_bClone(false)
	, m_vPoint(0.f, 0.f, 0.f)
	, m_fSpeed(0.5f)
{
	m_pGraphicDev->AddRef();
	ZeroMemory(m_pvPoint, sizeof(_vec3*) * POINT_END);
	m_pvPoint[POINT_C] = &m_vPoint;
}

CCurve::CCurve(const CCurve & rhs)
	:CBasePrototype(rhs)
	, m_pGraphicDev(rhs.m_pGraphicDev)
	, m_pD3DXLine(rhs.m_pD3DXLine)
	, m_pColliderCom(nullptr)
	, m_eCollType(rhs.m_eCollType)
	, m_eCurveType(rhs.m_eCurveType)
	, m_dwColor(rhs.m_dwColor)
	, m_bClone(true)
	, m_vPoint(0.f, 0.f, 0.f)
	, m_fSpeed(0.5f)
{
	ZeroMemory(m_pvPoint, sizeof(_vec3*) * POINT_END);
	m_pvPoint[POINT_A] = rhs.m_pvPoint[POINT_B];
	m_pvPoint[POINT_B] = rhs.m_pvPoint[POINT_C];
	m_pvPoint[POINT_C] = &m_vPoint;
	Create_Collider(m_eCurveType);
}

Engine::CCurve::~CCurve(void)
{
	
}

inline unsigned long CCurve::Release(void)
{
	Engine::Safe_Release(m_pColliderCom);
	return CBasePrototype::Release();
}

HRESULT CCurve::Ready_Curve(Engine::CAMERATYPE eType,
							const _vec3* pPoint)
{
	m_eCurveType = eType;
	m_vPoint = *pPoint;
	FAILED_CHECK_RETURN(D3DXCreateLine(m_pGraphicDev, &m_pD3DXLine), E_FAIL);
	Create_Collider(eType);
	return S_OK;
}

void Engine::CCurve::Render_Curve(const _matrix* pmatWorld)
{
	_matrix matWorld, matView, matProj, matTemp;
	D3DXMatrixIdentity(&matWorld);
	if (pmatWorld)
		matWorld = *pmatWorld;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	_vec3	vPoint[POINT_END];	
	for (_ulong i = 0; i < POINT_END; ++i)
	{
		vPoint[i] = (nullptr == m_pvPoint[i]) ? m_vPoint : *m_pvPoint[i];
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matWorld);
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matView);

		//if (vPoint[i].z <= 1.f)
		//	vPoint[i].z = 1.f;

		//D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matProj);
	}
	//_vec3 vPoint_Part[200];
	_float fInterval = 0.02f;
	_float fPlay = 0.f;
	std::vector<_vec3> vecPoint_Part;
	vecPoint_Part.reserve(50);

	for (int idx = 0; idx < 50; ++idx)
	{
		_vec3 vPoint_Part;
		D3DXVec3CatmullRom(&vPoint_Part, &vPoint[POINT_A], &vPoint[POINT_B], &vPoint[POINT_C],
			&vPoint[POINT_D], fPlay);
		fPlay += fInterval;
		if(vPoint_Part.z >= 0)
			vecPoint_Part.emplace_back(vPoint_Part);
	}
	for (auto& vPoint_Part : vecPoint_Part)
		D3DXVec3TransformCoord(&vPoint_Part, &vPoint_Part, &matProj);

	m_pD3DXLine->SetWidth(5.f); // 라인의 굵기를 결정하는 함수

	m_pGraphicDev->EndScene();
	m_pGraphicDev->BeginScene();

	m_pD3DXLine->Begin();

	_vec3* vPointArr = new _vec3[vecPoint_Part.size()];
	_vec3* pIter = vPointArr;
	for (auto& vPoint_Part : vecPoint_Part)
	{
		*pIter = vPoint_Part;
		++pIter;
	}
	
	m_pD3DXLine->DrawTransform(vPointArr, vecPoint_Part.size(), D3DXMatrixIdentity(&matTemp), m_dwColor);
	vecPoint_Part.clear();
	vecPoint_Part.shrink_to_fit();
	Engine::Safe_Delete_Array(vPointArr);

	m_pD3DXLine->End();

	////////////////// 콜라이더 랜더
	_matrix matWorld_Point;
	D3DXMatrixTranslation(&matWorld_Point, m_vPoint.x, m_vPoint.y, m_vPoint.z);
	matWorld_Point *= matWorld;
	m_pColliderCom->Render_Collider(&matWorld_Point);
}

HRESULT CCurve::Create_Collider(Engine::CAMERATYPE eType)
{
	Engine::RANGE tRange;
	tRange.vMin = _vec3(-0.3f, -0.3f, -0.3f);
	tRange.vMax = _vec3(0.3f, 0.3f, 0.3f);
	
	m_pColliderCom = Engine::CCollider::Create(m_pGraphicDev);
	switch (eType)
	{
	case Engine::CAMERATYPE_EYE:
		m_dwColor = COLOR_BLUE;
		m_pColliderCom->Set_CollMeshType(Engine::COLLMESHTYPE::COLLMESHTYPE_SPHERE);
		break;
	case Engine::CAMERATYPE_AT:
		m_dwColor = COLOR_RED;
		m_pColliderCom->Set_CollMeshType(Engine::COLLMESHTYPE::COLLMESHTYPE_BOX);
		break;
	default:
		break;
	}
	m_pColliderCom->Change_Collider(tRange);

	return S_OK;
}

CCurve * CCurve::Clone(_vec3 vPoint)
{
	CCurve* pClone = new CCurve(*this);
	pClone->Set_Point(vPoint);
	return pClone;
}

CCurve * CCurve::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CAMERATYPE eType, const _vec3 * pPoint)
{
	CCurve*	pInstance = new CCurve(pGraphicDev);

	if (FAILED(pInstance->Ready_Curve(eType, pPoint)))
		Safe_Release(pInstance);
	return pInstance;
}

void Engine::CCurve::Free(void)
{
	Safe_Release(m_pD3DXLine);
	Safe_Release(m_pGraphicDev);
}	