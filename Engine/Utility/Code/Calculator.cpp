#include "Calculator.h"

USING(Engine)

Engine::CCalculator::CCalculator(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

Engine::CCalculator::~CCalculator(void)
{

}

HRESULT Engine::CCalculator::Ready_Calculator(void)
{
	return S_OK;
}

_float Engine::CCalculator::Compute_HeightTerrain(const _vec3* pPos,
												const _vec3* pTerrainVtxPos,
												const _ulong& dwCntX,
												const _ulong& dwCntZ)
{
	_ulong	dwIndex = _ulong(pPos->z / 1.f) * _ulong(dwCntX) + _ulong(pPos->x / 1.f);

	_float	fRatioX = (pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x) / 1.f;
	
	_float  fRatioZ = (pTerrainVtxPos[dwIndex + dwCntX].z - pPos->z) / 1.f;

	_float		fHeight[4] = { 
		pTerrainVtxPos[dwIndex + dwCntX].y, 
		pTerrainVtxPos[dwIndex + dwCntX + 1].y,
		pTerrainVtxPos[dwIndex + 1].y,
		pTerrainVtxPos[dwIndex].y
	};
	
	// 오른쪽 위
	if (fRatioX > fRatioZ)
	{
		return fHeight[0] + (fHeight[1] - fHeight[0]) * fRatioX + (fHeight[2] - fHeight[1]) * fRatioZ;
	}
	// 왼쪽 아래
	else
	{
		return fHeight[0] + (fHeight[2] - fHeight[3]) * fRatioX + (fHeight[3] - fHeight[0]) * fRatioZ;
	}
}

CCalculator* Engine::CCalculator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCalculator*	pInstance = new CCalculator(pGraphicDev);

	if (FAILED(pInstance->Ready_Calculator()))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CCalculator::Free(void)
{
	Safe_Release(m_pGraphicDev);
}

_vec3 Engine::CCalculator::Pick_OnTerrain(HWND hWnd,
										const CTerrainTex* pTerrainBufferCom,
										const CTransform* pTerrainTransform)
{
	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vec3		vMousePos;
	
	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);

	// 투영 영역의 마우스로 변환
	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / (ViewPort.Height * -0.5f) + 1.f;
	vMousePos.z = 0.f;

	// 뷰스페이스의 마우스로 변환
	_matrix		matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	_vec3		vRayDir, vRayPos;		// 뷰 스페이스 상의 좌표라는 가정 하에 사용
	
	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	// 월드 스페이스로 vRayDir, vRayPos를 변형

	_matrix		matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	// 로컬 스페이스로 vRayDir, vRayPos를 변형

	_matrix		matWorld;
	pTerrainTransform->Get_WorldMatrix(&matWorld);

	D3DXMatrixInverse(&matWorld, NULL, &matWorld);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);


	_ulong	dwVtxCntX = pTerrainBufferCom->Get_VtxCntX();
	_ulong	dwVtxCntZ = pTerrainBufferCom->Get_VtxCntZ();

	const _vec3*	pTerrainVtx = pTerrainBufferCom->Get_VtxPos();

	_ulong	dwVtxIdx[3];

	_float	fU, fV, fDist;

	for (_ulong z = 0; z < dwVtxCntZ - 1; ++z)
	{
		for (_ulong x = 0; x < dwVtxCntX - 1; ++x)
		{
			_ulong	dwIndex = z * dwVtxCntX + x;

			// 오른쪽 위
			dwVtxIdx[0] = dwIndex + dwVtxCntX;
			dwVtxIdx[1] = dwIndex + dwVtxCntX + 1;
			dwVtxIdx[2] = dwIndex + 1;

			if (D3DXIntersectTri(&pTerrainVtx[dwVtxIdx[1]],
				&pTerrainVtx[dwVtxIdx[2]],
				&pTerrainVtx[dwVtxIdx[0]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				return _vec3(pTerrainVtx[dwVtxIdx[1]].x + (pTerrainVtx[dwVtxIdx[0]].x - pTerrainVtx[dwVtxIdx[1]].x)* fV,
					0.f, 
					pTerrainVtx[dwVtxIdx[1]].z + (pTerrainVtx[dwVtxIdx[2]].z - pTerrainVtx[dwVtxIdx[1]].z)* fU);
			}

			// 왼쪽 아래
			dwVtxIdx[0] = dwIndex + dwVtxCntX;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			if (D3DXIntersectTri(&pTerrainVtx[dwVtxIdx[2]],
				&pTerrainVtx[dwVtxIdx[0]],
				&pTerrainVtx[dwVtxIdx[1]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				return _vec3(pTerrainVtx[dwVtxIdx[2]].x + (pTerrainVtx[dwVtxIdx[1]].x - pTerrainVtx[dwVtxIdx[2]].x)* fV,
					0.f,
					pTerrainVtx[dwVtxIdx[2]].z + (pTerrainVtx[dwVtxIdx[0]].z - pTerrainVtx[dwVtxIdx[2]].z)* fU);
			}

		}
	}

	return _vec3(0.f, 0.f, 0.f);
}

_bool CCalculator::Collision_Sphere(const RANGE * pDstRange, const _matrix * pmatDstWorld, const RANGE * pSrcRange, const _matrix * pmatSrcWorld)
{
	RANGE		vDestRange, vSrcRange;

	if (nullptr != pmatDstWorld)
	{
		D3DXVec3TransformCoord(&vDestRange.vMin, &pDstRange->vMin, pmatDstWorld);
		D3DXVec3TransformCoord(&vDestRange.vMax, &pDstRange->vMax, pmatDstWorld);
	}
	if (nullptr != pmatSrcWorld)
	{
		D3DXVec3TransformCoord(&vSrcRange.vMin, &pSrcRange->vMin, pmatSrcWorld);
		D3DXVec3TransformCoord(&vSrcRange.vMax, &pSrcRange->vMax, pmatSrcWorld);
	}
	auto fDstRadius = pDstRange->Get_Radius();
	auto fSrcRadius = pSrcRange->Get_Radius();

	auto vDstCenterPos = pDstRange->Get_Center();
	auto vSrcCenterPos = pSrcRange->Get_Center();

	auto fDist = D3DXVec3Length(&(vDstCenterPos - vSrcCenterPos));

	return (fDist <= fSrcRadius + fDstRadius);
}

_bool Engine::CCalculator::Collision_AABB(const RANGE* pDstRange, const _matrix* pmatDstWorld,
											const RANGE* pSrcRange, const _matrix* pmatSrcWorld)
{
	_vec3		vDestMin, vDestMax, vSourMin, vSourMax;
	_float		fMin, fMax;

	D3DXVec3TransformCoord(&vDestMin, &pDstRange->vMin, pmatDstWorld);
	D3DXVec3TransformCoord(&vDestMax, &pDstRange->vMax, pmatDstWorld);

	D3DXVec3TransformCoord(&vSourMin, &pSrcRange->vMin, pmatSrcWorld);
	D3DXVec3TransformCoord(&vSourMax, &pSrcRange->vMax, pmatSrcWorld);

	// x축에서 바라봤을 때

	fMin = max(vDestMin.x, vSourMin.x);
	fMax = min(vDestMax.x, vSourMax.x);

	if (fMax < fMin)
		return false;

	// y축에서 바라봤을 때

	fMin = max(vDestMin.y, vSourMin.y);
	fMax = min(vDestMax.y, vSourMax.y);

	if (fMax < fMin)
		return false;

	// z축에서 바라봤을 때

	fMin = max(vDestMin.z, vSourMin.z);
	fMax = min(vDestMax.z, vSourMax.z);

	if (fMax < fMin)
		return false;

	return true;
}

_bool CCalculator::InterSectAABB(RANGE * pOut_Range, const RANGE * pDstRange, const _matrix * pmatDstWorld, const RANGE * pSrcRange, const _matrix * pmatSrcWorld)
{
	RANGE		vDestRange, vSrcRange;
	_float		fMin, fMax;

	D3DXVec3TransformCoord(&vDestRange.vMin, &pDstRange->vMin, pmatDstWorld);
	D3DXVec3TransformCoord(&vDestRange.vMax, &pDstRange->vMax, pmatDstWorld);

	D3DXVec3TransformCoord(&vSrcRange.vMin, &pSrcRange->vMin, pmatSrcWorld);
	D3DXVec3TransformCoord(&vSrcRange.vMax, &pSrcRange->vMax, pmatSrcWorld);

	if (false == Collision_Sphere(&vDestRange, nullptr, &vSrcRange, nullptr))
		return false;
	// x축에서 바라봤을 때

	fMin = max(vDestRange.vMin.x, vSrcRange.vMin.x);
	fMax = min(vDestRange.vMax.x, vSrcRange.vMax.x);

	if (fMax < fMin)
		return false;

	// y축에서 바라봤을 때

	fMin = max(vDestRange.vMin.y, vSrcRange.vMin.y);
	fMax = min(vDestRange.vMax.y, vSrcRange.vMax.y);

	if (fMax < fMin)
		return false;

	// z축에서 바라봤을 때

	fMin = max(vDestRange.vMin.z, vSrcRange.vMin.z);
	fMax = min(vDestRange.vMax.z, vSrcRange.vMax.z);

	if (fMax < fMin)
		return false;

	GetShortLine(&pOut_Range->vMin.x, &pOut_Range->vMax.x, vDestRange.vMin.x, vDestRange.vMax.x, vSrcRange.vMin.x, vSrcRange.vMax.x);
	GetShortLine(&pOut_Range->vMin.y, &pOut_Range->vMax.y, vDestRange.vMin.y, vDestRange.vMax.y, vSrcRange.vMin.y, vSrcRange.vMax.y);
	GetShortLine(&pOut_Range->vMin.z, &pOut_Range->vMax.z, vDestRange.vMin.z, vDestRange.vMax.z, vSrcRange.vMin.z, vSrcRange.vMax.z);

	return true;
}

void Engine::CCalculator::GetShortLine(float * pDstMin, float * pDstMax, float fSrcMin1, float fSrcMax1, float fSrcMin2, float fSrcMax2)
{
	if (fSrcMin1 < fSrcMin2)
	{
		*pDstMin = fSrcMin2;
		//Src1 기준 left
		if (fSrcMax1 < fSrcMax2)
		{
			//밖에 있음
			*pDstMax = fSrcMax1;
		}
		else
		{
			//안에있음
			*pDstMax = fSrcMax2;
		}
	}
	else
	{
		*pDstMin = fSrcMin1;
		//Src1 기준 right
		if (fSrcMax1 < fSrcMax2)
		{
			//안에있음
			*pDstMax = fSrcMax1;
		}
		else
		{
			//밖에 있음
			*pDstMax = fSrcMax2;
		}
	}
}

_bool Engine::CCalculator::Collision_OBB(const RANGE* pDstRange, const _matrix* pmatDstWorld,
											const RANGE* pSrcRange, const _matrix* pmatSrcWorld)
{
	OBB		tObb[2];
	ZeroMemory(&tObb, sizeof(OBB) * 2);

	Set_Point(&tObb[0], pDstRange);
	Set_Point(&tObb[1], pSrcRange);

	for (_uint i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&tObb[0].vPoint[i], &tObb[0].vPoint[i], pmatDstWorld);
		D3DXVec3TransformCoord(&tObb[1].vPoint[i], &tObb[1].vPoint[i], pmatSrcWorld);
	}

	D3DXVec3TransformCoord(&tObb[0].vCenter, &tObb[0].vCenter, pmatDstWorld);
	D3DXVec3TransformCoord(&tObb[1].vCenter, &tObb[1].vCenter, pmatSrcWorld);

	for (_uint i = 0; i < 2; ++i)
		Set_Axis(&tObb[i]);

	_float	fDistance[3] = {};

	for (_uint i = 0; i < 2; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			fDistance[0] = fabs(D3DXVec3Dot(&tObb[0].vProjAxis[0], &tObb[i].vAxis[j])) +
							fabs(D3DXVec3Dot(&tObb[0].vProjAxis[1], &tObb[i].vAxis[j])) +
							fabs(D3DXVec3Dot(&tObb[0].vProjAxis[2], &tObb[i].vAxis[j]));	// 첫 번째 obb가 임의의 축과 각 면을 향하는 벡터들의 투영한 길이의 합

			fDistance[1] = fabs(D3DXVec3Dot(&tObb[1].vProjAxis[0], &tObb[i].vAxis[j])) +
							fabs(D3DXVec3Dot(&tObb[1].vProjAxis[1], &tObb[i].vAxis[j])) +
							fabs(D3DXVec3Dot(&tObb[1].vProjAxis[2], &tObb[i].vAxis[j]));	// 두 번째 obb가 임의의 축과 각 면을 향하는 벡터들의 투영한 길이의 합

			fDistance[2] = fabs(D3DXVec3Dot(&(tObb[1].vCenter - tObb[0].vCenter), &tObb[i].vAxis[j]));
			
			if (fDistance[2] > fDistance[0] + fDistance[1])
				return false;
		}
	}

	return true;
}

_bool CCalculator::Collision_Ray(const LPD3DXMESH pDstMesh, const _matrix * pmatDstWorld, _vec3* pvRayPos, _vec3* pvRayDir, _vec3* pvCollisionPoint)
{
	DWORD dwTmp;
	_float  fU, fV, fDist;
	BOOL	bCollision;
	D3DXIntersect(pDstMesh, pvRayPos, pvRayDir, &bCollision, &dwTmp, &fU, &fV, &fDist, NULL, NULL);

	if (nullptr != pvCollisionPoint)
	{
		*pvCollisionPoint = *pvRayPos + *pvRayDir * fDist;
		D3DXVec3TransformCoord(pvCollisionPoint, pvCollisionPoint, pmatDstWorld);
	}
	
	return ((bCollision == TRUE) ? true : false);
}

_bool CCalculator::Collision_Mouse(HWND hWnd, const LPD3DXMESH pDstMesh, const _matrix * pmatDstWorld, _vec3* pvCollisionPoint)
{
	_vec3 vRayPos, vRayDir;
	Make_MouseRayPos(hWnd, &vRayPos, &vRayDir, pmatDstWorld);
	return Collision_Ray(pDstMesh, pmatDstWorld, &vRayPos, &vRayDir, pvCollisionPoint);
}

_bool CCalculator::Collision_Ray_Tri(const _vec3 * pDstTri, const _matrix * pmatDstWorld, _vec3 * pvRayPos, _vec3 * pvRayDir, _vec3 * pvCollisionPoint)
{
	_float  fU, fV, fDist;
	BOOL	bCollision;
	bCollision = D3DXIntersectTri(&pDstTri[0], &pDstTri[1], &pDstTri[2], pvRayPos, pvRayDir, &fU, &fV, &fDist);

	if (nullptr != pvCollisionPoint)
	{
		*pvCollisionPoint = *pvRayPos + *pvRayDir * fDist;
		D3DXVec3TransformCoord(pvCollisionPoint, pvCollisionPoint, pmatDstWorld);
	}

	return ((bCollision == TRUE) ? true : false);
}

_bool CCalculator::Collision_Mouse_Tri(HWND hWnd, const _vec3 * pDstTri, const _matrix * pmatDstWorld, _vec3 * pvCollisionPoint)
{
	_vec3 vRayPos, vRayDir;
	Make_MouseRayPos(hWnd, &vRayPos, &vRayDir, pmatDstWorld);
	return Collision_Ray_Tri(pDstTri, pmatDstWorld, &vRayPos, &vRayDir, pvCollisionPoint);
}

void CCalculator::Make_MouseRayPos(HWND hWnd, _vec3* pvOut_RayPos, _vec3* pvOut_RayDir, const _matrix * pmatDstWorld)
{
	POINT		ptMouse;
	D3DXMATRIX	matProjection;
	D3DXMATRIX	matView;

	auto& vRayPos = *pvOut_RayPos;
	auto& vRayDir = *pvOut_RayDir;

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);
	
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProjection);

	D3DXMATRIX matCameraWorld;
	D3DXMatrixInverse(&matCameraWorld, NULL, &matView);

	vRayPos = { 0.f, 0.f, 0.f };

	vRayDir.x = (float(ptMouse.x) / (WINCX >> 1) - 1.f) / matProjection._11;
	vRayDir.y = (float(-ptMouse.y) / (WINCY >> 1) + 1.f) / matProjection._22;
	vRayDir.z = 1.f;

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matCameraWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matCameraWorld);
	D3DXVec3Normalize(&vRayDir, &vRayDir);

	D3DXMATRIX matLocal;
	D3DXMatrixInverse(&matLocal, NULL, pmatDstWorld);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matLocal);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matLocal);
}

void CCalculator::Set_Point(OBB * pObb, const RANGE* pRange)
{
	pObb->vPoint[0] = _vec3(pRange->vMin.x, pRange->vMax.y, pRange->vMin.z);
	pObb->vPoint[1] = _vec3(pRange->vMax.x, pRange->vMax.y, pRange->vMin.z);
	pObb->vPoint[2] = _vec3(pRange->vMax.x, pRange->vMin.y, pRange->vMin.z);
	pObb->vPoint[3] = _vec3(pRange->vMin.x, pRange->vMin.y, pRange->vMin.z);


	pObb->vPoint[4] = _vec3(pRange->vMin.x, pRange->vMax.y, pRange->vMax.z);
	pObb->vPoint[5] = _vec3(pRange->vMax.x, pRange->vMax.y, pRange->vMax.z);
	pObb->vPoint[6] = _vec3(pRange->vMax.x, pRange->vMin.y, pRange->vMax.z);
	pObb->vPoint[7] = _vec3(pRange->vMin.x, pRange->vMin.y, pRange->vMax.z);

	pObb->vCenter = pRange->Get_Center();
}

void CCalculator::Set_Axis(OBB * pObb)
{
	pObb->vProjAxis[0] = (pObb->vPoint[2] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;
	pObb->vProjAxis[1] = (pObb->vPoint[0] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;
	pObb->vProjAxis[2] = (pObb->vPoint[7] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;
	
	pObb->vAxis[0] = pObb->vPoint[2] - pObb->vPoint[3];
	pObb->vAxis[1] = pObb->vPoint[0] - pObb->vPoint[3];
	pObb->vAxis[2] = pObb->vPoint[7] - pObb->vPoint[3];

	for (_uint i = 0; i < 3; ++i)
		D3DXVec3Normalize(&pObb->vAxis[i], &pObb->vAxis[i]);
}

