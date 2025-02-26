#include "Cell.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Export_Resources.h"

USING(Engine)

Engine::CCell::CCell(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_eCollType(Engine::COLLTYPE::COL_FALSE)
{
	ZeroMemory(m_vPoint, sizeof(_vec3) * POINT_END);
	ZeroMemory(m_pNeighbor, sizeof(CCell*) * NEIGHBOR_END);
	ZeroMemory(m_pLine, sizeof(CLine*) * LINE_END);

	m_pGraphicDev->AddRef();
}

Engine::CCell::~CCell(void)
{
	Engine::Safe_Release(m_pObjectInfo);
	for (auto idx = 0; idx < POINT_END; ++idx)
	{
		Engine::Safe_Release(m_pColliderCom_Point[idx]);
	}
	Engine::Safe_Release(m_pTriCol);
}


CCell* Engine::CCell::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _ulong& dwIndex, const _vec3* pPointA, const _vec3* pPointB, const _vec3* pPointC)
{
	pGraphicDev->AddRef();

	CCell*	pInstance = new CCell(pGraphicDev);

	if (FAILED(pInstance->Ready_Cell(dwIndex, pPointA, pPointB, pPointC)))
		Safe_Release(pInstance);

	Safe_Release(pGraphicDev);

	return pInstance;
}


_vec3 CCell::Get_Pos()
{
	_vec3 vResult = {0.f, 0.f, 0.f};
	for (auto vPos : m_vPoint)
		vResult += vPos;
	vResult /= POINT_END;
	return vResult;
}

_vec3 CCell::Get_Up()
{
	_vec3 vNormal1 = m_vPoint[POINT_B] - m_vPoint[POINT_A];
	_vec3 vNormal2 = m_vPoint[POINT_C] - m_vPoint[POINT_A];
	_vec3 vUp;
	D3DXVec3Normalize(&vNormal1, &vNormal1);
	D3DXVec3Normalize(&vNormal2, &vNormal2);
	D3DXVec3Cross(&vUp, &vNormal1, &vNormal2);
	D3DXVec3Normalize(&vUp, &vUp);
	return vUp;
}

HRESULT Engine::CCell::Ready_Cell(const _ulong& dwIndex,
									const _vec3* pPointA,
									const _vec3* pPointB, 
									const _vec3* pPointC)
{
	// ObejctKey
	m_pObjectInfo = Engine::CObjectInfo::Create(L"Cell");

	m_dwIndex = dwIndex;

	m_vPoint[POINT_A] = *pPointA;
	m_vPoint[POINT_B] = *pPointB;
	m_vPoint[POINT_C] = *pPointC;

	m_pLine[LINE_AB] = CLine::Create(&_vec2(m_vPoint[POINT_A].x, m_vPoint[POINT_A].z), &_vec2(m_vPoint[POINT_B].x, m_vPoint[POINT_B].z));

	m_pLine[LINE_BC] = CLine::Create(&_vec2(m_vPoint[POINT_B].x, m_vPoint[POINT_B].z), &_vec2(m_vPoint[POINT_C].x, m_vPoint[POINT_C].z));

	m_pLine[LINE_CA] = CLine::Create(&_vec2(m_vPoint[POINT_C].x, m_vPoint[POINT_C].z), &_vec2(m_vPoint[POINT_A].x, m_vPoint[POINT_A].z));

	Engine::RANGE tRange;
	tRange.vMin = _vec3(-0.3f, -0.3f, -0.3f);
	tRange.vMax = _vec3(0.3f, 0.3f, 0.3f);
	for (auto idx = 0; idx < POINT_END; ++idx)
	{
		m_pColliderCom_Point[idx] = Engine::CCollider::Create(m_pGraphicDev);
		m_pColliderCom_Point[idx]->Set_CollMeshType(Engine::COLLMESHTYPE::COLLMESHTYPE_SPHERE);
		m_pColliderCom_Point[idx]->Change_Collider(tRange);
	}
	m_pTriCol = Engine::CTriCol::Create(m_pGraphicDev);
	m_pTriCol->Set_Property(m_vPoint, m_dwColor);
//#ifdef _DEBUG
	FAILED_CHECK_RETURN(D3DXCreateLine(m_pGraphicDev, &m_pD3DXLine), E_FAIL);
//#endif

	return S_OK;
}

_bool Engine::CCell::Compare_Point(const _vec3* pFirstPoint, const _vec3* pSecondPoint, CCell* pCell)
{
	if (m_vPoint[POINT_A] == *pFirstPoint)
	{
		if (m_vPoint[POINT_B] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_AB] = pCell;
			return true;
		}
		if (m_vPoint[POINT_C] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_CA] = pCell;
			return true;
		}
	}

	if (m_vPoint[POINT_B] == *pFirstPoint)
	{
		if (m_vPoint[POINT_A] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_AB] = pCell;
			return true;
		}
		if (m_vPoint[POINT_C] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_BC] = pCell;
			return true;
		}
	}

	if (m_vPoint[POINT_C] == *pFirstPoint)
	{
		if (m_vPoint[POINT_A] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_CA] = pCell;
			return true;
		}
		if (m_vPoint[POINT_B] == *pSecondPoint)
		{
			m_pNeighbor[NEIGHBOR_BC] = pCell;
			return true;
		}
	}

	return false;
}

void Engine::CCell::Render_Cell(const _matrix* pmatWorld)
{
	// m_pD3DXLine->Draw(); // 2d라인을 그리는 함수

	// m_pD3DXLine->DrawTransform(); // 3D라인을 그리는 함수

	_vec3	vPoint[4];

	vPoint[0] = m_vPoint[POINT_A];
	vPoint[1] = m_vPoint[POINT_B];
	vPoint[2] = m_vPoint[POINT_C];
	vPoint[3] = m_vPoint[POINT_A];

	_matrix		matView, matProj, matTemp;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	_matrix matWorld;
	D3DXMatrixIdentity(&matWorld);
	if (pmatWorld)
		matWorld = *pmatWorld;

	for (_ulong i = 0; i < 4; ++i)
	{
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matWorld);
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matView);
		
		if (vPoint[i].z <= 0.1f)
			vPoint[i].z = 0.1f;

		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matProj);
	}

	m_pD3DXLine->SetWidth(5.f); // 라인의 굵기를 결정하는 함수
	
	m_pGraphicDev->EndScene();
	m_pGraphicDev->BeginScene();

	m_pD3DXLine->Begin();

	m_pD3DXLine->DrawTransform(vPoint, 4, D3DXMatrixIdentity(&matTemp), D3DXCOLOR(0.f, 1.f, 0.f, 1.f));
	//m_pD3DXLine->DrawTransform(vPoint, 4, &(matWorld * matView * matProj), D3DXCOLOR(1.f, 0.f, 0.f, 1.f));

	m_pD3DXLine->End();

	_matrix matWorld_Point;
	for (auto idx = 0; idx < POINT_END; ++idx)
	{
		Engine::Render_Texture(RESOURCE_STATIC, L"Texture_Default", 0);
		D3DXMatrixTranslation(&matWorld_Point, m_vPoint[idx].x, m_vPoint[idx].y, m_vPoint[idx].z);
		matWorld_Point *= matWorld;
		m_pColliderCom_Point[idx]->Render_Collider(&matWorld_Point);
	}

	auto dwOption = m_pObjectInfo->Get_Option();
	if (dwOption == CELL_NONE)
		m_dwColor = COLOR_CELL_NONE;
	if (dwOption & CELL_NOCAM)
		m_dwColor = COLOR_CELL_NOCAM;
	if (dwOption & CELL_NOUNIT)
		m_dwColor = COLOR_CELL_NOUNIT;
	if (dwOption & CELL_NOBUILD)
		m_dwColor = COLOR_CELL_NOBUILD;
	if (dwOption & CELL_YESFISH)
		m_dwColor = COLOR_CELL_YESFISH;
	if (m_eCollType == Engine::COLLTYPE::COL_SELECT)
		m_dwColor = COLOR_CELL_SELECT;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pTriCol->Set_Property(m_vPoint, m_dwColor);

	DWORD dwFillMode = 0;
	BOOL bLighting;
	m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, (DWORD*)&bLighting);
	m_pGraphicDev->GetRenderState(D3DRS_FILLMODE, &dwFillMode);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphicDev->SetTexture(0, NULL);
	m_pTriCol->Render_Buffer();
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, dwFillMode);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, bLighting);

	_vec3 vPos_Cell = Get_Pos();
	D3DXVec3TransformCoord(&vPos_Cell, &vPos_Cell, &matView);
	if (vPos_Cell.z >= 0.f && vPos_Cell.z <= 25.f)
	{
		TCHAR szInfo[MID_STR];
		swprintf_s(szInfo, L"%d", m_dwIndex);
		D3DXVec3TransformCoord(&vPos_Cell, &vPos_Cell, &matProj);
		vPos_Cell.x = (1 + vPos_Cell.x) * WINCX * 0.5f;
		vPos_Cell.y = (1 - vPos_Cell.y) * WINCY * 0.5f;
		Engine::Render_Font(L"Font_Default", szInfo, &_vec2(vPos_Cell.x, vPos_Cell.y), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}
}

void Engine::CCell::Free(void)
{
	//#ifdef _DEBUG
	Safe_Release(m_pD3DXLine);
//#endif

	for (_ulong i = 0; i < LINE_END; ++i)
		Safe_Release(m_pLine[i]);

	Safe_Release(m_pGraphicDev);

}

Engine::CCell::COMPARE Engine::CCell::Compare(const _vec3* pEndPos, _ulong* pCellIndex)
{
	for (_ulong i = 0; i < LINE_END; ++i)
	{
		if (CLine::COMPARE_LEFT == m_pLine[i]->Compare(&_vec2(pEndPos->x, pEndPos->z)))
		{
			if (nullptr == m_pNeighbor[i])
				return CCell::COMPARE_STOP;
			else
			{
				*pCellIndex = *m_pNeighbor[i]->Get_Index();
				return CCell::COMPARE_MOVE;
			}
		}
	}
	*pCellIndex = *Get_Index();
	return CCell::COMPARE_MOVE;
}

CLine::COMPARE CCell::Compare_Line(LINE eLine, const _vec3 * pEndPos)
{
	return m_pLine[eLine]->Compare(&_vec2(pEndPos->x, pEndPos->z));
}
