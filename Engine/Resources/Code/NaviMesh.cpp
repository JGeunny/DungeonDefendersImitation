#include "NaviMesh.h"
#include "Export_System.h"

USING(Engine)

Engine::CNaviMesh::CNaviMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMesh(pGraphicDev)
	, m_dwIndex(0)
{

}

Engine::CNaviMesh::CNaviMesh(const CNaviMesh& rhs)
	: CMesh(rhs)
	, m_dwIndex(rhs.m_dwIndex)
	, m_vecCell(rhs.m_vecCell)
{
	for (_ulong i = 0; i < m_vecCell.size(); ++i)
		m_vecCell[i]->AddRef();
}

Engine::CNaviMesh::~CNaviMesh(void)
{

}

DWORD CNaviMesh::Get_Index(_vec3 vPos)
{
	float fU, fV, fDist;
	vPos.y += 1.f;
	_vec3 vDown = _vec3(0.f, -1.f, 0.f);
	for (int idx = 0; idx < (int)m_vecCell.size(); ++idx)
	{
		auto pCell = m_vecCell[idx];
		if(TRUE == D3DXIntersectTri(&pCell->m_vPoint[0], &pCell->m_vPoint[1], &pCell->m_vPoint[2],
			&vPos, &vDown, &fU, &fV, &fDist))
			return idx;
	}
	return 0;
}

HRESULT Engine::CNaviMesh::Ready_NaviMesh(void)
{
	m_vecCell.reserve(20);

	CCell*		pCell = nullptr;
	
	/*Add_Cell(&pCell,
		_vec3(0.f, 0.f, 0.f),
		_vec3(2.f, 0.f, 0.f),
		_vec3(0.f, 0.f, 2.f));*/
	//// 0
	//pCell = CCell::Create(m_pGraphicDev,
	//						m_vecCell.size(),
	//						&_vec3(0.f, 0.f, 2.f),
	//						&_vec3(2.f, 0.f, 0.f),
	//						&_vec3(0.f, 0.f, 0.f));
	//NULL_CHECK_RETURN(pCell, E_FAIL);
	//m_vecCell.push_back(pCell);

	//// 1
	//pCell = CCell::Create(m_pGraphicDev,
	//						m_vecCell.size(),
	//						&_vec3(0.f, 0.f, 2.f),
	//						&_vec3(2.f, 0.f, 2.f),
	//						&_vec3(2.f, 0.f, 0.f));
	//NULL_CHECK_RETURN(pCell, E_FAIL);
	//m_vecCell.push_back(pCell);

	//// 2
	//pCell = CCell::Create(m_pGraphicDev,
	//						m_vecCell.size(),
	//						&_vec3(0.f, 0.f, 4.f),
	//						&_vec3(2.f, 0.f, 2.f),
	//						&_vec3(0.f, 0.f, 2.f));
	//NULL_CHECK_RETURN(pCell, E_FAIL);
	//m_vecCell.push_back(pCell);

	//// 3
	//pCell = CCell::Create(m_pGraphicDev,
	//						m_vecCell.size(),
	//						&_vec3(2.f, 0.f, 2.f),
	//						&_vec3(4.f, 0.f, 0.f),
	//						&_vec3(2.f, 0.f, 0.f));
	//NULL_CHECK_RETURN(pCell, E_FAIL);
	//m_vecCell.push_back(pCell);

	//FAILED_CHECK_RETURN(Link_Cell(), E_FAIL);

	return S_OK;
}

void CNaviMesh::Render_NaviMesh(const _matrix* pmatWorld)
{
	if (!Engine::Is_DebugRender(DEBUG_RENDER_NAVI)) return;
	for (auto& iter : m_vecCell)
		iter->Render_Cell(pmatWorld);
}
//
//_vec3 CNaviMesh::MoveOn_NaviMesh(const _vec3 * pTargetPos, const _vec3 * pTargetDir, bool bComputtingHeight, bool* pbUpGround)
//{
//	_vec3		vEndPos = *pTargetPos + *pTargetDir;
//
//	if (CCell::COMPARE_MOVE == m_vecCell[m_dwIndex]->Compare(&vEndPos, &m_dwIndex))
//	{	
//		_float fHeight = Compute_HeightCell(&vEndPos, m_vecCell[m_dwIndex]->m_vPoint);
//
//		if (nullptr != pbUpGround)
//			*pbUpGround = (vEndPos.y >= fHeight);
//		if (true == bComputtingHeight)
//			vEndPos.y = fHeight;
//		return vEndPos;
//	}
//
//	else if (CCell::COMPARE_STOP == m_vecCell[m_dwIndex]->Compare(&vEndPos, &m_dwIndex))
//	{
//		_vec3 vUp = { 0.f, 1.f, 0.f };
//		_vec3 vSliding;
//		bool bMove = false;
//		for (int idx_Point = 0; idx_Point < Engine::CCell::POINT_END && bMove == false; ++idx_Point)
//		{
//			//꼭지점에서 이동시 탈출하는 버그 있음...
//			if (CLine::COMPARE::COMPARE_LEFT == m_vecCell[m_dwIndex]->Compare_Line((CCell::LINE)idx_Point, &vEndPos))
//			{
//				_vec3 vNormal = m_vecCell[m_dwIndex]->m_vPoint[idx_Point] - m_vecCell[m_dwIndex]->m_vPoint[(idx_Point + 1) % Engine::CCell::POINT_END];
//				D3DXVec3Normalize(&vNormal, &vNormal);
//				D3DXVec3Cross(&vNormal, &vNormal, &vUp);
//				vSliding = *pTargetDir - D3DXVec3Dot(pTargetDir, &vNormal) * vNormal;
//				vEndPos = *pTargetPos + vSliding;
//				
//				if (CCell::COMPARE_MOVE == m_vecCell[m_dwIndex]->Compare(&vEndPos, &m_dwIndex))
//					bMove = true;
//			}
//		}
//		_float fHeight = Compute_HeightCell(&vEndPos, m_vecCell[m_dwIndex]->m_vPoint);
//
//		if (nullptr != pbUpGround)
//			*pbUpGround = (vEndPos.y >= fHeight);
//		if (true == bComputtingHeight)
//			vEndPos.y = fHeight;
//
//		if (bMove)
//			return vEndPos;
//		else
//		{
//			//auto dwIndex = Get_Index(vEndPos);
//			//if (m_vecCell.size() > dwIndex)
//			//{
//			//	Set_Index(dwIndex);
//			//	return vEndPos;
//			//}
//			//else
//				return *pTargetPos;
//		}
//
//		//슬라이딩 벡터 해야함
//		//공식 *pTargetDir - D3DXVec3Dot(pTargetDir, &vNormal) * vNormal;
//		//return *pTargetPos;
//	}
//
//	return _vec3(0.f, 0.f, 0.f);
//}

_vec3 CNaviMesh::MoveOn_NaviMesh(const _vec3 * pTargetPos, const _vec3 * pTargetDir, bool bComputtingHeight, bool* pbUpGround)
{
	_vec3		vEndPos = *pTargetPos + *pTargetDir;

	if (vEndPos == *pTargetPos)
	{
		if (nullptr != pbUpGround)
			*pbUpGround = true;
		return vEndPos;
	}

	CCell::COMPARE eCompare = m_vecCell[m_dwIndex]->Compare(&vEndPos, &m_dwIndex);

	switch (eCompare)
	{
	case Engine::CCell::COMPARE_MOVE:
	{
		_float fHeight = Compute_HeightCell(&vEndPos, m_vecCell[m_dwIndex]->Get_Points());

		if (nullptr != pbUpGround)
			*pbUpGround = (vEndPos.y >= fHeight);
		if (true == bComputtingHeight)
			vEndPos.y = fHeight;
		return vEndPos;
	}
	break;
	case Engine::CCell::COMPARE_STOP:
	{
		vEndPos = Compute_SlidingPos(*pTargetPos, *pTargetDir);

		_float fHeight = Compute_HeightCell(&vEndPos, m_vecCell[m_dwIndex]->Get_Points());

		if (nullptr != pbUpGround)
			*pbUpGround = (vEndPos.y >= fHeight);
		if (true == bComputtingHeight)
				vEndPos.y = fHeight;
		return vEndPos;
	}
	break;
	default:
		break;
	}

	return _vec3(0.f, 0.f, 0.f);
}

_vec3 CNaviMesh::Compute_SlidingPos(_vec3 vStartPos, _vec3 vMovePos)
{
	_vec3 vUp = m_vecCell[m_dwIndex]->Get_Up();
	_vec3 vSliding;
	_vec3 vEndPos = vStartPos + vMovePos;

	int iCount_Sliding = 0;
	vUp = m_vecCell[m_dwIndex]->Get_Up();
	for (int idx_Point = 0; idx_Point < Engine::CCell::POINT_END; ++idx_Point)
	{
		//꼭지점에서 이동시 탈출하는 버그 있음...
		if (CLine::COMPARE::COMPARE_LEFT == m_vecCell[m_dwIndex]->Compare_Line((CCell::LINE)idx_Point, &vEndPos))
		{
			_vec3 vNormal = *m_vecCell[m_dwIndex]->Get_Point((Engine::CCell::POINT)idx_Point) - *m_vecCell[m_dwIndex]->Get_Point((Engine::CCell::POINT)((idx_Point + 1) % Engine::CCell::POINT_END));
			D3DXVec3Normalize(&vNormal, &vNormal);
			D3DXVec3Cross(&vNormal, &vNormal, &vUp);
			vSliding = vMovePos - D3DXVec3Dot(&vMovePos, &vNormal) * vNormal;
			vEndPos = vStartPos + vSliding;

			m_vecCell[m_dwIndex]->Compare(&vEndPos, &m_dwIndex);
			++iCount_Sliding;
		}
	}

	if (iCount_Sliding == 0)
	{
		return vStartPos;
	}
	else if (iCount_Sliding == 1)
	{
		return vEndPos;
	}
	else
	{
		//마지막 까지 못찾았을 경우 슬라이딩 방향으로 전체 셀을 탐색해서 인덱스를 찾음
		_vec3 vDir;
		D3DXVec3Normalize(&vDir, &vMovePos);
		DWORD dwIndex = 0;
		_vec3 vPointPos = vEndPos;
		vector<CCell*> vecCell;
		vector<float>	 vecDot;
		for (auto pCell : m_vecCell)
		{
			bool bExcute = false;
			auto arrPoints = pCell->Get_Points();
			_vec3 vDir_Point = _vec3(0.f, 0.f, 0.f);
			for (int i = 0; i < CCell::POINT_END; ++i)
			{
				vDir_Point += arrPoints[i];
				if (0.1f > D3DXVec3Length(&(arrPoints[i] - vPointPos)))
				{
					vPointPos = arrPoints[i];
					bExcute = true;
				}
			}
			if (false == bExcute)
				continue;
			D3DXVec3Normalize(&vDir_Point, &((vDir_Point / 3) - vPointPos));
			vecDot.emplace_back(D3DXVec3Dot(&vDir_Point, &vDir));
			vecCell.emplace_back(pCell);
		}
		int angle = 0;
		int index = -1;
		for (int i = 0; i < vecDot.size(); ++i)
		{
			if (vecDot[i] > angle)
			{
				angle = vecDot[i];
				index = i;
			}
		}
		if (-1 != index)
		{
			m_dwIndex = *vecCell[index]->Get_Index();
			return vPointPos;
		}
		return vStartPos;
	}
}


HRESULT CNaviMesh::Add_Cell(Engine::CCell **ppCell, _vec3 vPointA, _vec3 vPointB, _vec3 vPointC)
{
	auto iCheck_CCW = Check_CCW(vPointA, vPointB, vPointC);

	if (0 == iCheck_CCW)
	{
		ERR_MSG(L"추가 실패!, 세점이 일직선상에 있습니다.");
		return iCheck_CCW;
	}
	else if (1 == iCheck_CCW) //역방향인 경우
	{
		_vec3 vTemp = vPointB;
		vPointB = vPointC;
		vPointC = vTemp;
	}

	auto pCell = CCell::Create(m_pGraphicDev,
		m_vecCell.size(),
		&vPointA,
		&vPointB,
		&vPointC);

	m_vecCell.push_back(pCell);
	*ppCell = pCell;
	return iCheck_CCW;
}

HRESULT CNaviMesh::Del_Cell(Engine::CCell * pCell)
{
	auto plstCell = Get_CellList();
	auto iter_cell = find_if(plstCell->begin(), plstCell->end(), [&](auto pCell_This)
	{
		return (pCell_This == pCell);
	});
	if (iter_cell == plstCell->end())
		return E_FAIL;
	Engine::Safe_Release(*iter_cell);
	plstCell->erase(iter_cell);
	return S_OK;
}

int CNaviMesh::Check_CCW(_vec3 vPointA, _vec3 vPointB, _vec3 vPointC)
{
	//_matrix matView;
	//m_pGraphicDev->GetTransform(D3DTRANSFORMSTATETYPE::D3DTS_VIEW, &matView);
	//D3DXVec3TransformCoord(&vPointA, &vPointA, &matView);
	//D3DXVec3TransformCoord(&vPointB, &vPointB, &matView);
	//D3DXVec3TransformCoord(&vPointC, &vPointC, &matView);

	int iCheck = int(vPointA.x * vPointB.z + vPointB.x * vPointC.z + vPointC.x * vPointA.z);
	iCheck -= int((vPointA.z * vPointB.x + vPointB.z * vPointC.x + vPointC.z * vPointA.x));

	if (iCheck < 0)
		return -1;

	if (iCheck > 0)
		return 1;
	return 0;
}

HRESULT Engine::CNaviMesh::Link_Cell(void)
{
	for (_ulong i = 0; i < m_vecCell.size(); ++i)
	{
		for (_ulong j = 0; j < m_vecCell.size(); ++j)
		{
			if(i == j)
				continue;

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_AB) &&
				true    == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_A), 
													m_vecCell[i]->Get_Point(CCell::POINT_B), 
													m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_AB, m_vecCell[j]);
				continue;
			}

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_BC) &&
				true    == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_B),
														m_vecCell[i]->Get_Point(CCell::POINT_C),
														m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_BC, m_vecCell[j]);
				continue;
			}

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_CA) &&
				true	== m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_C),
														m_vecCell[i]->Get_Point(CCell::POINT_A),
														m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_CA, m_vecCell[j]);
				continue;
			}

		}
	}

	return S_OK;
}

_float CNaviMesh::Compute_HeightCell(const _vec3 * pPos, const _vec3 * pvPoint)
{
	D3DXPLANE plane;
	D3DXPlaneFromPoints(&plane, &pvPoint[CCell::POINT_A], &pvPoint[CCell::POINT_B], &pvPoint[CCell::POINT_C]);
	_float fHeight = -(plane.a * pPos->x + plane.c * pPos->z + plane.d ) / plane.b;
	return fHeight;
}

CNaviMesh* Engine::CNaviMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CNaviMesh*	pInstance = new CNaviMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_NaviMesh()))
		Safe_Release(pInstance);

	return pInstance;
}

CResources* Engine::CNaviMesh::Clone(void)
{
	return new CNaviMesh(*this);
}

void Engine::CNaviMesh::Free(void)
{
	CMesh::Free();

	for_each(m_vecCell.begin(), m_vecCell.end(), CDeleteObj());
	m_vecCell.clear();

}

