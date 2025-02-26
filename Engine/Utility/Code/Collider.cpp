#include "Collider.h"
#include "Export_System.h"
USING(Engine)

Engine::CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_dwUsingCollision(COLLISION_NONE)
	, m_pColliderMesh(nullptr)
	, m_eCollType(COLLTYPE::COL_FALSE)
	, m_eOldCollType(COLLTYPE::COL_END)
	, m_dwColor(COLOR_COLLTYPE_FALSE)
{
	m_pGraphicDev->AddRef();
}

Engine::CCollider::~CCollider(void)
{
}

HRESULT CCollider::Ready_Collider()
{
	return S_OK;
}

HRESULT CCollider::Change_Collider(const RANGE& tRange)
{
	m_tRange = tRange;
	Engine::Safe_Release(m_pColliderMesh);
	//auto vCenter = tRange.Get_Vertical();
	auto vCenter = tRange.Get_Center();

	switch (m_eCollMeshType)
	{
	case Engine::COLLMESHTYPE_BOX:
	{
		auto vVolume = tRange.Get_Volume();
		D3DXCreateBox(m_pGraphicDev, vVolume.x, vVolume.y, vVolume.z, &m_pColliderMesh, nullptr);
		break;
	}
	case Engine::COLLMESHTYPE_SPHERE:
	{
		auto fRadius_Sphere = tRange.Get_Radius();
		D3DXCreateSphere(m_pGraphicDev, fRadius_Sphere, 12, 6, &m_pColliderMesh, nullptr);
	break;
	}
	case Engine::COLLMESHTYPE_TRIANGLE:
		break;
	case Engine::COLLMESHTYPE_END:
		break;
	default:
		break;
	}	
	D3DXMatrixTranslation(&m_matWorld_Interval, vCenter.x, vCenter.y, vCenter.z);
	
	return S_OK;
}

void Engine::CCollider::Render_Collider(const _matrix* pColliderMatrix)
{
	if (!Engine::Is_DebugRender(DEBUG_RENDER_COLLIDER)) return;
	if (nullptr == m_pColliderMesh) return;

	m_matWorld = m_matWorld_Interval * *pColliderMatrix;
	//m_matWorld = *pColliderMatrix;

//#ifdef _DEBUG
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_matWorld));
	m_pGraphicDev->SetTexture(0, NULL);

	if (m_eOldCollType != m_eCollType)
	{
		m_eOldCollType = m_eCollType;
		m_dwColor = COLOR_COLLTYPE_FALSE;
		switch (m_eOldCollType)
		{
		case Engine::COL_FALSE:
			break;
		case Engine::COL_TRUE:
			m_dwColor = COLOR_COLLTYPE_TRUE;
			break;
		case Engine::COL_SELECT:
			m_dwColor = COLOR_COLLTYPE_SELECT;
			break;
		case Engine::COL_END:
			break;
		default:
			break;
		}
		Set_MeshColor(m_pColliderMesh, m_dwColor);
	}

	DWORD dwFillMode = 0;
	BOOL bLighting;
	m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, (DWORD*)&bLighting);
	m_pGraphicDev->GetRenderState(D3DRS_FILLMODE, &dwFillMode);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pColliderMesh->DrawSubset(0);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, dwFillMode);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, bLighting);
//#endif
}

HRESULT CCollider::Set_MeshColor(LPD3DXMESH& pOriMesh, DWORD dwColor)
{
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPD3DXMESH pMesh;
	pOriMesh->CloneMeshFVF(0, FVF_COL, m_pGraphicDev, &pMesh);
	if (SUCCEEDED(pMesh->GetVertexBuffer(&m_pVB)))
	{
		int iNumVertices = pMesh->GetNumVertices();
		VTXCOL* pVertex = nullptr;
		
		m_pVB->Lock(0, 0, (void**)&pVertex, 0);
		for (int i = 0; i < iNumVertices; ++i)
			pVertex[i].dwColor = dwColor;
		m_pVB->Unlock();
		m_pVB->Release();
	}
	pOriMesh->Release();
	pOriMesh = pMesh;
	return S_OK;
}

CCollider * CCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCollider*	pInstance = new CCollider(pGraphicDev);

	if (FAILED(pInstance->Ready_Collider()))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CCollider::Free(void)
{
	Safe_Release(m_pGraphicDev);

//#ifdef _DEBUG
	Engine::Safe_Release(m_pVB);
	Engine::Safe_Release(m_pIB);

	for (_uint i = 0; i < COL_END; ++i)
		Safe_Release(m_pTexture[i]);

	Engine::Safe_Release(m_pColliderMesh);
//#endif

}

