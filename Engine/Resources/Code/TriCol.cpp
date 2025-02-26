#include "TriCol.h"

USING(Engine)

Engine::CTriCol::CTriCol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
	ZeroMemory(m_vPosArr, sizeof(_vec3) * 3);
	m_dwColor = COLOR_BACKGROUND;
}

Engine::CTriCol::CTriCol(const CTriCol& rhs)
	: CVIBuffer(rhs)
{

}

Engine::CTriCol::~CTriCol(void)
{

}

HRESULT Engine::CTriCol::Ready_Buffer(void)
{
	m_dwVtxCnt = 3;
	m_dwVtxFVF = FVF_COL;
	m_dwTriCnt = 1;
	m_dwVtxSize = sizeof(VTXCOL);

	m_IdxFmt = D3DFMT_INDEX32;
	m_dwIdxSize = sizeof(INDEX32);

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXCOL*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPos = _vec3(0.0f, 0.5f, 0.f);
	pVertex[0].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertex[1].vPos = _vec3(0.5f, -0.5f, 0.f);
	pVertex[1].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertex[2].vPos = _vec3(-0.5f, -0.5f, 0.f);
	pVertex[2].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	m_pVB->Unlock();

	INDEX32*		pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	m_pIB->Unlock();

	for (int i = 0; i < 3; ++i)
		m_vPosArr[i] = pVertex[i].vPos;
	m_dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);
	return S_OK;
}

void CTriCol::Set_Property(_vec3 * pvPosArr, DWORD dwColor)
{
	bool bChange = false;
	//°°Áö ¾ÊÀº°Ô ÇÏ³ª¶óµµ ÀÖ´Ù¸é ¹Ù²ãÁÜ
	bChange = (m_dwColor != dwColor);
	for (int i = 0; i < 3 && !bChange; ++i)
		bChange = (m_vPosArr[i] != pvPosArr[i]);

	if (!bChange)
		return;
	
	//¹Ù²ãÁÜ
	for (int i = 0; i < 3; ++i)
		m_vPosArr[i] = pvPosArr[i];
	m_dwColor = dwColor;

	VTXCOL*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPos = m_vPosArr[0];
	pVertex[0].dwColor = m_dwColor;

	pVertex[1].vPos = m_vPosArr[1];
	pVertex[1].dwColor = m_dwColor;

	pVertex[2].vPos = m_vPosArr[2];
	pVertex[2].dwColor = m_dwColor;

	m_pVB->Unlock();

}

Engine::CTriCol* Engine::CTriCol::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTriCol*	pInstance = new CTriCol(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
		Engine::Safe_Release(pInstance);


	return pInstance;
}

void Engine::CTriCol::Free(void)
{
	CVIBuffer::Free();
}

Engine::CResources* Engine::CTriCol::Clone(void)
{
	return new CTriCol(*this);
}

