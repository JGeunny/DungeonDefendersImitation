#include "Export_Utility.h"

#include <iostream>

USING(Engine)
IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr(void)
	: m_pCalculatorCom(nullptr)
{
}

CCollisionMgr::~CCollisionMgr(void)
{
	Free();
}

HRESULT CCollisionMgr::Ready_CollisionMgr(LPDIRECT3DDEVICE9 pGraphicDev)
{
	// Calculator
	m_pCalculatorCom = CCalculator::Create(pGraphicDev);
	NULL_CHECK_RETURN(m_pCalculatorCom, E_FAIL);

	return S_OK;
}

void CCollisionMgr::LateUpdate_CollisionMgr(void)
{
	if (m_mapCollisionObject.size() == 0) return;
	CollisionPush(m_mapCollisionObject[L"Push_DstObject"], m_mapCollisionObject[L"Push_SrcObject"]);
	CollisionBase(m_mapCollisionObject[L"Base_DstObject"], m_mapCollisionObject[L"Base_SrcObject"]);
}

void CCollisionMgr::ReleaseData_CollisionMgr(void)
{
	for (auto pair : m_mapCollisionObject)
		pair.second.clear();
	m_mapCollisionObject.clear();
}

void CCollisionMgr::Add_CollisionGroup(int iCollisionUsingInfo, Engine::CGameObject * pGameObject)
{
	if (iCollisionUsingInfo & COLLISION_BASE_SRC)
		m_mapCollisionObject[L"Base_SrcObject"].emplace_back(pGameObject);
	if (iCollisionUsingInfo & COLLISION_BASE_DST)
		m_mapCollisionObject[L"Base_DstObject"].emplace_back(pGameObject);
	if (iCollisionUsingInfo & COLLISION_PUSH_SRC)
		m_mapCollisionObject[L"Push_SrcObject"].emplace_back(pGameObject);
	if (iCollisionUsingInfo & COLLISION_PUSH_DST)
		m_mapCollisionObject[L"Push_DstObject"].emplace_back(pGameObject);	
}

void CCollisionMgr::Del_CollisionGroup(Engine::CGameObject * pGameObject)
{
	for (auto& pair : m_mapCollisionObject)
	{
		auto& lstObj =  pair.second;

		lstObj.remove_if([&](auto pObj) {
			return pObj == pGameObject;
		});
	}
}

void CCollisionMgr::Free(void)
{
	ReleaseData_CollisionMgr();
	Engine::Safe_Release(m_pCalculatorCom);
}

void CCollisionMgr::CollisionBase(std::list<CGameObject*>& rDst, std::list<CGameObject*>& rSrc)
{
	_bool bCrash = false;
	
	for (auto& pDest : rDst)
	{
		if (pDest->Get_Event() == EVENT_DEAD) continue;
		auto pDestTrans = dynamic_cast<CTransform*>(pDest->Get_Component(L"Com_Transform", COMPONENTID::ID_DYNAMIC));
		auto pDestCollider = dynamic_cast<CCollider*>(pDest->Get_Component(L"Com_Collider", COMPONENTID::ID_STATIC));
		if (!pDestCollider) continue;

		for (auto& pSource : rSrc) //밀려지는 객체
		{
			if (pSource->Get_Event() == EVENT_DEAD) continue;
			if (pDest == pSource) continue;//자신이면 패스0
			if (pSource->Get_Team() == pDest->Get_Team()) continue; //같은 팀이면 패스
			auto pSrcTrans = dynamic_cast<CTransform*>(pSource->Get_Component(L"Com_Transform", COMPONENTID::ID_DYNAMIC));
			auto pSrcCollider = dynamic_cast<CCollider*>(pSource->Get_Component(L"Com_Collider", COMPONENTID::ID_STATIC));
			if (!pSrcCollider) continue;
			
			pDestCollider->Get_Range()->Get_Radius();
			_matrix matSrcWorld, matDestWorld;
			pSrcTrans->Get_WorldMatrix(&matSrcWorld);
			pDestTrans->Get_WorldMatrix(&matDestWorld);
			bCrash = m_pCalculatorCom->Collision_OBB(pDestCollider->Get_Range(), &matDestWorld
													, pSrcCollider->Get_Range(), &matSrcWorld);
			if (bCrash)
			{
				pSource->BeCollision(COLLISION_BASE_SRC, pDest);
				pDest->BeCollision(COLLISION_BASE_DST, pSource);
			}
		}
	}
}

void CCollisionMgr::CollisionPush(std::list<Engine::CGameObject*>& rDst, std::list<Engine::CGameObject*>& rSrc)
{
	Engine::RANGE tRange = {};
	D3DXVECTOR3	vMove = {};
	float		fMove = 0.f;
	bool		bCrash = false;
	bool		bPlusMa = false;

	///// 안움직인놈 우선 정렬
	//rSrc.sort([&](auto pGameObject1, auto pGameObject2) {
	//	auto pSrcAABB1 = dynamic_cast<CAABBCol*>(pGameObject1->GetComponent(L"AABBCol"));
	//	auto pSrcAABB2 = dynamic_cast<CAABBCol*>(pGameObject2->GetComponent(L"AABBCol"));
	//	if (!pSrcAABB1->m_IsMove && pSrcAABB2->m_IsMove)
	//		return true;
	//	else
	//		return false;
	//});
	for (auto& pSource : rSrc) //밀려지는 객체
	{
		if (pSource->Get_Event() == EVENT_DEAD) continue;
		auto pSrcTransform = dynamic_cast<Engine::CTransform*>(pSource->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC));
		auto pSrcCollider = dynamic_cast<CCollider*>(pSource->Get_Component(L"Com_Collider", Engine::COMPONENTID::ID_STATIC));

		if (!pSrcTransform) continue;
		if (!pSrcCollider) continue;

		//pSrcAABB->m_IsMove = false;
		////가장 가까운 대상 부터 검사하도록 정렬
		auto vSrcPos = pSrcTransform->Get_Pos();
		rDst.sort([&](auto pGameObject1, auto pGameObject2) {
			auto vPos1 = ((Engine::CTransform*)(pGameObject1->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC)))->Get_Pos();
			auto vPos2 = ((Engine::CTransform*)(pGameObject2->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC)))->Get_Pos();
			return D3DXVec3Length(&(vPos1 - vSrcPos))
				< D3DXVec3Length(&(vPos2 - vSrcPos));
		});
		
		for (auto& pDest : rDst)
		{
			if (pDest == pSource) continue;//자신이면 패스
			if (pDest->Get_Event() == EVENT_DEAD) continue;
			auto pDestTransform = dynamic_cast<Engine::CTransform*>(pDest->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC));
			auto pDestCollider = dynamic_cast<CCollider*>(pDest->Get_Component(L"Com_Collider", Engine::COMPONENTID::ID_STATIC));
			if (!pDestTransform) continue;
			if (!pDestCollider) continue;

			auto vSrcWorldPos = pSrcTransform->Get_Pos();
			auto vDestWorldPos = pDestTransform->Get_Pos();     

			_matrix matSrcWorld_AABB, matDestWorld_AABB;
			pSrcTransform->Get_NRotWorldMatrix(&matSrcWorld_AABB);
			pDestTransform->Get_NRotWorldMatrix(&matDestWorld_AABB);
			auto tSrcRange = *pSrcCollider->Get_Range();
			auto tDestRange = *pDestCollider->Get_Range();
		
			bCrash = m_pCalculatorCom->InterSectAABB(&tRange, &tDestRange, &matDestWorld_AABB, &tSrcRange, &matSrcWorld_AABB);
			if (bCrash)
			{
				pSource->BeCollision(COLLISION_PUSH_SRC, pDest);
				pDest->BeCollision(COLLISION_PUSH_DST, pSource);
				vMove = tRange.vMax - tRange.vMin;

				//auto matParentsWorld = pSrcTransform->m_matParentsWorld;
				//D3DXMatrixInverse(&matParentsWorld, nullptr, &matParentsWorld);
				//D3DXVec3TransformCoord(&vMove, &vMove, &matParentsWorld);

				int iPosIter = -1;
				_vec3 vPos_Src = pSrcTransform->Get_Pos();
				float* fPos = (float*)(vPos_Src);

				iPosIter = 0;
				fMove = vMove.x;
				bPlusMa = (vSrcWorldPos.x > vDestWorldPos.x);

				//if (fMove > vMove.y)
				//{
				//	iPosIter = 1;
				//	fMove = vMove.y;
				//	bPlusMa = (vSrcWorldPos.y > vDestWorldPos.y);
				//}

				if (fMove > vMove.z)
				{
					iPosIter = 2;
					fMove = vMove.z;
					bPlusMa = (vSrcWorldPos.z > vDestWorldPos.z);
				}

				for (int i = 0; i < iPosIter; ++i)
					++fPos;

				if (!std::isnan(fMove) && iPosIter != -1)
				{
					if (bPlusMa)
						*fPos += fMove;
					else
						*fPos -= fMove;
				}
				
				if(FAILED(pSource->Set_NaviPos(&(vPos_Src - pSrcTransform->Get_Pos()))))
					pSrcTransform->Set_Pos(vPos_Src);
				pSrcTransform->Update_Component(0);

			}
		}
	}

	//Engine::AABB_INFO tAABBInfo = {};
	//D3DXVECTOR3	vMove = {};
	//float		fMove = 0.f;
	//bool		bCrash = false;
	//bool		bPlusMa = false;

	///// 안움직인놈 우선 정렬
	//rSrc.sort([&](auto pGameObject1, auto pGameObject2) {
	//	auto pSrcAABB1 = dynamic_cast<CAABBCol*>(pGameObject1->GetComponent(L"AABBCol"));
	//	auto pSrcAABB2 = dynamic_cast<CAABBCol*>(pGameObject2->GetComponent(L"AABBCol"));
	//	if (!pSrcAABB1->m_IsMove && pSrcAABB2->m_IsMove)
	//		return true;
	//	else
	//		return false;
	//});
	//for (auto& pSource : rSrc) //밀려지는 객체
	//{
	//	if (pSource->GetEvent() == EVENT_DEAD) continue;
	//	auto pSrcTransform = dynamic_cast<Engine::CTransform*>(pSource->GetComponent(L"Transform"));
	//	auto pSrcAABB = dynamic_cast<CAABBCol*>(pSource->GetComponent(L"AABBCol"));

	//	if (!pSrcTransform) continue;
	//	if (!pSrcAABB) continue;

	//	pSrcAABB->m_IsMove = false;
	//	////가장 가까운 대상 부터 검사하도록 정렬
	//	auto vSrcPos = pSrcTransform->GetWorldPos();
	//	rDst.sort([&](auto pGameObject1, auto pGameObject2) {
	//		auto vPos1 = ((Engine::CTransform*)(pGameObject1->GetComponent(L"Transform")))->GetWorldPos();
	//		auto vPos2 = ((Engine::CTransform*)(pGameObject2->GetComponent(L"Transform")))->GetWorldPos();
	//		return D3DXVec3Length(&(vPos1 - vSrcPos))
	//			< D3DXVec3Length(&(vPos2 - vSrcPos));
	//	});
	//	
	//	for (auto& pDest : rDst)
	//	{
	//		if (pDest->GetEvent() == EVENT_DEAD) continue;
	//		auto pDestTransform = dynamic_cast<Engine::CTransform*>(pDest->GetComponent(L"Transform"));
	//		auto pDestAABB = dynamic_cast<CAABBCol*>(pDest->GetComponent(L"AABBCol"));
	//		if (pDest == pSource) continue;//자신이면 패스
	//		if (!pDestTransform) continue;
	//		if (!pDestAABB) continue;

	//		auto vSrcWorldPos = pSrcTransform->GetWorldPos();
	//		auto vDestWorldPos = pDestTransform->GetWorldPos();
	//		//auto fLength = D3DXVec3Length(&D3DXVECTOR3(vSrcWorldPos - vDestWorldPos));
	//		//if (fLength > 50.f) continue;
	//		
	//		bCrash = InterSectCube(&tAABBInfo, pSrcAABB->GetAABBInfo(), pDestAABB->GetAABBInfo());
	//		if (bCrash)
	//		{
	//			pSource->BeCollision(COLLISION_CUBEEX_SRC, pDest);
	//			pDest->BeCollision(COLLISION_CUBEEX_DST, pSource);
	//			vMove = tAABBInfo.vMax - tAABBInfo.vMin;

	//			auto matParentsWorld = pSrcTransform->m_matParentsWorld;
	//			D3DXMatrixInverse(&matParentsWorld, nullptr, &matParentsWorld);
	//			D3DXVec3TransformCoord(&vMove, &vMove, &matParentsWorld);

	//			int iPosIter = -1;
	//			float* fPos = (float*)(pSrcTransform->m_vPos);

	//			iPosIter = 0;
	//			fMove = vMove.x;
	//			bPlusMa = (vSrcWorldPos.x > vDestWorldPos.x);

	//			if (fMove > vMove.y)
	//			{
	//				iPosIter = 1;
	//				fMove = vMove.y;
	//				bPlusMa = (vSrcWorldPos.y > vDestWorldPos.y);
	//			}

	//			if (fMove > vMove.z)
	//			{
	//				iPosIter = 2;
	//				fMove = vMove.z;
	//				bPlusMa = (vSrcWorldPos.z > vDestWorldPos.z);
	//			}

	//			for (int i = 0; i < iPosIter; ++i)
	//				++fPos;

	//			if (!std::isnan(fMove) && iPosIter != -1)
	//			{
	//				if (bPlusMa)
	//					*fPos += fMove;
	//				else
	//					*fPos -= fMove;
	//			}

	//			pSrcTransform->MakeWorldMatrix();

	//			//*fPos = roundf(*fPos* powf(float(10), (float)2)) / powf(float(10), (float)2);
	//			//pSrcAABB->MakeColBox(pSrcTransform->m_vPos, pSrcTransform->m_vSize);
	//			
	//		}
	//	}
	//}
}


//bool CCollisionMgr::RayPickCube(Engine::RAY_INFO* _pOutput, Engine::CTransform* _pDstInfoCom, Engine::CTransform* _pSrcInfoCom)
//{
//	if (!m_pInputCube) return false;
//
//	auto vTargetPos = _pSrcInfoCom->GetWorldPos();
//	auto vRayPos = _pDstInfoCom->GetWorldPos();
//
//	_pOutput->vRayDir = _pDstInfoCom->m_vDir;
//
//	D3DXMATRIX matInverseTarget, matTarget;
//	matInverseTarget = matTarget = *_pSrcInfoCom->GetWorldMatrix();
//	D3DXMatrixInverse(&matInverseTarget, nullptr, &matInverseTarget);
//
//	auto vRayDir = _pDstInfoCom->m_vDir;
//	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matInverseTarget);
//	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matInverseTarget);
//	D3DXVec3Normalize(&vRayDir, &vRayDir);
//
//	float fU, fV, fDist;
//	bool CheckIntersect = false;
//	map<float, D3DXVECTOR3> mapCompare;
//	map<float, D3DXVECTOR3> mapNormal;
//	D3DXVECTOR3 vNormal{ 0.f, 0.f, 0.f };
//
//	// 앞면 z-
//	if (D3DXIntersectTri(&m_pInputCube[1],
//		&m_pInputCube[2],
//		&m_pInputCube[0],
//		&vRayPos,
//		&vRayDir,
//		&fU, &fV, &fDist))
//	{
//		mapCompare.emplace(fDist, m_pInputCube[1]
//			+ (m_pInputCube[2] - m_pInputCube[1]) * fU
//			+ (m_pInputCube[0] - m_pInputCube[1]) * fV);
//
//		D3DXVec3Cross(&vNormal, &(m_pInputCube[2] - m_pInputCube[1]), &(m_pInputCube[0] - m_pInputCube[1]));
//		mapNormal.emplace(fDist, vNormal);
//		CheckIntersect = true;
//	}
//	if (D3DXIntersectTri(&m_pInputCube[3],
//		&m_pInputCube[0],
//		&m_pInputCube[2],
//		&vRayPos,
//		&vRayDir,
//		&fU, &fV, &fDist))
//	{
//		mapCompare.emplace(fDist, m_pInputCube[3]
//			+ (m_pInputCube[0] - m_pInputCube[3]) * fU
//			+ (m_pInputCube[2] - m_pInputCube[3]) * fV);
//
//		D3DXVec3Cross(&vNormal, &(m_pInputCube[0] - m_pInputCube[3]), &(m_pInputCube[2] - m_pInputCube[3]));
//		mapNormal.emplace(fDist, vNormal);
//		CheckIntersect = true;
//	}
//
//	// 뒷면 z+
//	if (D3DXIntersectTri(&m_pInputCube[5],
//		&m_pInputCube[4],
//		&m_pInputCube[7],
//		&vRayPos,
//		&vRayDir,
//		&fU, &fV, &fDist))
//	{
//		mapCompare.emplace(fDist, m_pInputCube[5]
//			+ (m_pInputCube[4] - m_pInputCube[5]) * fU
//			+ (m_pInputCube[7] - m_pInputCube[5]) * fV);
//
//
//		D3DXVec3Cross(&vNormal, &(m_pInputCube[4] - m_pInputCube[5]), &(m_pInputCube[7] - m_pInputCube[5]));
//		mapNormal.emplace(fDist, vNormal);
//		CheckIntersect = true;
//
//	}
//	if (D3DXIntersectTri(&m_pInputCube[5],
//		&m_pInputCube[7],
//		&m_pInputCube[6],
//		&vRayPos,
//		&vRayDir,
//		&fU, &fV, &fDist))
//	{
//		mapCompare.emplace(fDist, m_pInputCube[5]
//			+ (m_pInputCube[7] - m_pInputCube[5]) * fU
//			+ (m_pInputCube[6] - m_pInputCube[5]) * fV);
//
//
//		D3DXVec3Cross(&vNormal, &(m_pInputCube[7] - m_pInputCube[5]), &(m_pInputCube[6] - m_pInputCube[5]));
//		mapNormal.emplace(fDist, vNormal);
//		CheckIntersect = true;
//	}
//
//	// 윗면 +y
//	if (D3DXIntersectTri(&m_pInputCube[5],
//		&m_pInputCube[1],
//		&m_pInputCube[4],
//		&vRayPos,
//		&vRayDir,
//		&fU, &fV, &fDist))
//	{
//		mapCompare.emplace(fDist, m_pInputCube[5]
//			+ (m_pInputCube[1] - m_pInputCube[5]) * fU
//			+ (m_pInputCube[4] - m_pInputCube[5]) * fV);
//
//
//		D3DXVec3Cross(&vNormal, &(m_pInputCube[1] - m_pInputCube[5]), &(m_pInputCube[4] - m_pInputCube[5]));
//		mapNormal.emplace(fDist, vNormal);
//		CheckIntersect = true;
//	}
//	if (D3DXIntersectTri(&m_pInputCube[0],
//		&m_pInputCube[4],
//		&m_pInputCube[1],
//		&vRayPos,
//		&vRayDir,
//		&fU, &fV, &fDist))
//	{
//		mapCompare.emplace(fDist, m_pInputCube[0]
//			+ (m_pInputCube[4] - m_pInputCube[0]) * fU
//			+ (m_pInputCube[1] - m_pInputCube[0]) * fV);
//
//
//		D3DXVec3Cross(&vNormal, &(m_pInputCube[4] - m_pInputCube[0]), &(m_pInputCube[1] - m_pInputCube[0]));
//		mapNormal.emplace(fDist, vNormal);
//		CheckIntersect = true;
//	}
//
//	// 아랫면 -y
//	if (D3DXIntersectTri(&m_pInputCube[2],
//		&m_pInputCube[6],
//		&m_pInputCube[3],
//		&vRayPos,
//		&vRayDir,
//		&fU, &fV, &fDist))
//	{
//		mapCompare.emplace(fDist, m_pInputCube[2]
//			+ (m_pInputCube[6] - m_pInputCube[2]) * fU
//			+ (m_pInputCube[3] - m_pInputCube[2]) * fV);
//
//		D3DXVec3Cross(&vNormal, &(m_pInputCube[6] - m_pInputCube[2]), &(m_pInputCube[3] - m_pInputCube[2]));
//		mapNormal.emplace(fDist, vNormal);
//		CheckIntersect = true;
//	}
//	if (D3DXIntersectTri(&m_pInputCube[7],
//		&m_pInputCube[3],
//		&m_pInputCube[6],
//		&vRayPos,
//		&vRayDir,
//		&fU, &fV, &fDist))
//	{
//		mapCompare.emplace(fDist, m_pInputCube[7]
//			+ (m_pInputCube[3] - m_pInputCube[7]) * fU
//			+ (m_pInputCube[6] - m_pInputCube[7]) * fV);
//
//		D3DXVec3Cross(&vNormal, &(m_pInputCube[3] - m_pInputCube[7]), &(m_pInputCube[6] - m_pInputCube[7]));
//		mapNormal.emplace(fDist, vNormal);
//		CheckIntersect = true;
//	}
//
//	// 좌측면 -x
//	if (D3DXIntersectTri(&m_pInputCube[0],
//		&m_pInputCube[3],
//		&m_pInputCube[4],
//		&vRayPos,
//		&vRayDir,
//		&fU, &fV, &fDist))
//	{
//		mapCompare.emplace(fDist, m_pInputCube[0]
//			+ (m_pInputCube[3] - m_pInputCube[0]) * fU
//			+ (m_pInputCube[4] - m_pInputCube[0]) * fV);
//
//		D3DXVec3Cross(&vNormal, &(m_pInputCube[3] - m_pInputCube[0]), &(m_pInputCube[4] - m_pInputCube[0]));
//		mapNormal.emplace(fDist, vNormal);
//		CheckIntersect = true;
//	}
//	if (D3DXIntersectTri(&m_pInputCube[7],
//		&m_pInputCube[4],
//		&m_pInputCube[3],
//		&vRayPos,
//		&vRayDir,
//		&fU, &fV, &fDist))
//	{
//		mapCompare.emplace(fDist, m_pInputCube[7]
//			+ (m_pInputCube[4] - m_pInputCube[7]) * fU
//			+ (m_pInputCube[3] - m_pInputCube[7]) * fV);
//
//		D3DXVec3Cross(&vNormal, &(m_pInputCube[4] - m_pInputCube[7]), &(m_pInputCube[3] - m_pInputCube[7]));
//		mapNormal.emplace(fDist, vNormal);
//		CheckIntersect = true;
//	}
//
//	// 우측면 +x
//	if (D3DXIntersectTri(&m_pInputCube[5],
//		&m_pInputCube[6],
//		&m_pInputCube[1],
//		&vRayPos,
//		&vRayDir,
//		&fU, &fV, &fDist))
//	{
//		mapCompare.emplace(fDist, m_pInputCube[5]
//			+ (m_pInputCube[6] - m_pInputCube[5]) * fU
//			+ (m_pInputCube[1] - m_pInputCube[5]) * fV);
//
//		D3DXVec3Cross(&vNormal, &(m_pInputCube[6] - m_pInputCube[5]), &(m_pInputCube[1] - m_pInputCube[5]));
//		mapNormal.emplace(fDist, vNormal);
//		CheckIntersect = true;
//	}
//	if (D3DXIntersectTri(&m_pInputCube[2],
//		&m_pInputCube[1],
//		&m_pInputCube[6],
//		&vRayPos,
//		&vRayDir,
//		&fU, &fV, &fDist))
//	{
//		mapCompare.emplace(fDist, m_pInputCube[2]
//			+ (m_pInputCube[1] - m_pInputCube[2]) * fU
//			+ (m_pInputCube[6] - m_pInputCube[2]) * fV);
//
//		D3DXVec3Cross(&vNormal, &(m_pInputCube[1] - m_pInputCube[2]), &(m_pInputCube[6] - m_pInputCube[2]));
//		mapNormal.emplace(fDist, vNormal);
//		CheckIntersect = true;
//	}
//
//	if (CheckIntersect == true)
//	{
//		auto iter = mapCompare.begin();
//		_pOutput->vPickPos = iter->second;
//		D3DXVec3TransformCoord(&_pOutput->vPickPos, &_pOutput->vPickPos, &matTarget);
//		iter = mapNormal.begin();
//		_pOutput->vNormal = iter->second;
//		D3DXVec3Normalize(&_pOutput->vNormal, &_pOutput->vNormal);
//	}
//	return CheckIntersect;
//}
