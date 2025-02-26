#include "Optimization.h"

USING(Engine)

Engine::COptimization::COptimization(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bClone(false)
	, m_pFrustum(nullptr)
	,m_pQuadTree(nullptr)
{
	m_pGraphicDev->AddRef();
}

Engine::COptimization::COptimization(const COptimization& rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
	, m_bClone(true)
	, m_pFrustum(rhs.m_pFrustum)
	, m_pQuadTree(rhs.m_pQuadTree)
{
	m_pGraphicDev->AddRef();
}

Engine::COptimization::~COptimization(void)
{

}

HRESULT Engine::COptimization::Ready_Optimization(USEAGE eType, const _ulong& dwCntX, const _ulong& dwCntZ)
{
	m_pFrustum = CFrustum::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(m_pFrustum, E_FAIL);

	if (DISABLE == eType)
		return S_OK;

	m_pQuadTree = CQuadTree::Create(dwCntX, dwCntZ);
	NULL_CHECK_RETURN(m_pQuadTree, E_FAIL);

	return S_OK;
}

_bool COptimization::Is_InFrustum_ForObject(const _vec3 * pPos, const _float fRadius)
{
	return m_pFrustum->Is_InFrustum_ForObject(pPos, fRadius);
}

void COptimization::Is_InFrustum_ForTerrain(const _vec3 * pVtxPos, const _ulong & dwCntX, const _ulong & dwCntZ, Engine::INDEX32 * pIndex, _ulong * pTriCnt)
{
	m_pFrustum->Is_InFrustum_ForTerrain(pVtxPos, dwCntX, dwCntZ, pIndex, pTriCnt, m_pQuadTree);
}

COptimization* Engine::COptimization::Create(LPDIRECT3DDEVICE9 pGraphicDev, USEAGE eType, const _ulong& dwCntX, const _ulong& dwCntZ)
{
	COptimization*		pInstance = new COptimization(pGraphicDev);

	if (FAILED(pInstance->Ready_Optimization(eType, dwCntX, dwCntZ)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent* Engine::COptimization::Clone(void)
{
	return new COptimization(*this);
}

void Engine::COptimization::Free(void)
{
	Safe_Release(m_pGraphicDev);	

	if (false == m_bClone)
	{
		Safe_Release(m_pFrustum);
		Safe_Release(m_pQuadTree);
	}
}

