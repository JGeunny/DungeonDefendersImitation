#include "StaticMesh.h"

USING(Engine)

Engine::CStaticMesh::CStaticMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMesh(pGraphicDev)
{

}

Engine::CStaticMesh::CStaticMesh(const CStaticMesh& rhs)
	: CMesh(rhs)
	, m_pMesh(rhs.m_pMesh)
	, m_pAdjacency(rhs.m_pAdjacency)
	, m_pSubsetBuffer(rhs.m_pSubsetBuffer)
	, m_pSubsets(rhs.m_pSubsets)
	, m_dwSubsetCnt(rhs.m_dwSubsetCnt)
	, m_pOriMesh(rhs.m_pOriMesh)
	, m_pVtxPos(rhs.m_pVtxPos)
	, m_dwVtxNum(rhs.m_dwVtxNum)
	, m_dwStride(rhs.m_dwStride)
{
	m_ppTexture = new LPDIRECT3DTEXTURE9[rhs.m_dwSubsetCnt];

	for (_ulong i = 0; i < rhs.m_dwSubsetCnt; ++i)
	{
		m_ppTexture[i] = rhs.m_ppTexture[i];
		m_ppTexture[i]->AddRef();
	}
	m_pAdjacency->AddRef();
	m_pMesh->AddRef();
	m_pOriMesh->AddRef();
	m_pSubsetBuffer->AddRef();
}

Engine::CStaticMesh::~CStaticMesh(void)
{

}

HRESULT Engine::CStaticMesh::Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName)
{
	_tchar	szFullPath[128] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	FAILED_CHECK_RETURN(D3DXLoadMeshFromX(szFullPath,
										D3DXMESH_MANAGED, 
										m_pGraphicDev,
										&m_pAdjacency,
										&m_pSubsetBuffer,
										NULL,
										&m_dwSubsetCnt,
										&m_pOriMesh),
										E_FAIL);

	_ulong		dwFVF = m_pOriMesh->GetFVF(); // 현재 메쉬가 지닌 정점의 속성 값을 얻어오는 함수

	// FVF 속성 중 NORMAL 정보가 없을 경우
	if (!(dwFVF & D3DFVF_NORMAL))
	{
		// 기존의 load한 메쉬의 fvf 정보를 복제하는 함수
		m_pOriMesh->CloneMeshFVF(m_pOriMesh->GetOptions(), dwFVF | D3DFVF_NORMAL, m_pGraphicDev, &m_pMesh);
		
		// 메쉬 정점의 normal 값을 계산하여 넣어주는 함수
		D3DXComputeNormals(m_pMesh, (_ulong*)m_pAdjacency->GetBufferPointer());
	}
	else
	{
		m_pOriMesh->CloneMeshFVF(m_pOriMesh->GetOptions(), dwFVF, m_pGraphicDev, &m_pMesh);
	}

	void*		pVertex = nullptr;

	m_dwVtxNum = m_pMesh->GetNumVertices();

	m_pVtxPos = new _vec3[m_dwVtxNum];

	m_pMesh->LockVertexBuffer(0, &pVertex);

	D3DVERTEXELEMENT9		Decl[MAX_FVF_DECL_SIZE];	// 정점 하나가 지닐 수 있는 최대의 FVF 개수
	ZeroMemory(Decl, sizeof(D3DVERTEXELEMENT9) * MAX_FVF_DECL_SIZE);

	m_pMesh->GetDeclaration(Decl);

	_ulong	byOffset = 0;

	for (_ulong i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (Decl[i].Usage == D3DDECLUSAGE_POSITION)
		{
			byOffset = (_ulong)Decl[i].Offset;
			break;
		}
	}

	// 정점의 fvf를 알고 있을 떄, 정점의 최종 크기를 얻어온다.
	m_dwStride = D3DXGetFVFVertexSize(dwFVF);

	for (_ulong i = 0; i < m_dwVtxNum; ++i)
	{
		m_pVtxPos[i] = *((_vec3*)(((_ubyte*)pVertex) + (m_dwStride * i + byOffset)));
	}
	
	// 컴객체가 담고 있는 재질 정보 중 가장 앞 주소를 리턴하는 함수
	m_pSubsets = (D3DXMATERIAL*)m_pSubsetBuffer->GetBufferPointer();

	m_ppTexture = new LPDIRECT3DTEXTURE9[m_dwSubsetCnt];

	for (_ulong i = 0; i < m_dwSubsetCnt; ++i)
	{
		_tchar	szFileName[128] = L"";

		lstrcpy(szFullPath, pFilePath);

		MultiByteToWideChar(CP_ACP, 
			0, 
			m_pSubsets[i].pTextureFilename,
			strlen(m_pSubsets[i].pTextureFilename),
			szFileName, 
			128);

		lstrcat(szFullPath, szFileName);

		FAILED_CHECK_RETURN(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_ppTexture[i]), E_FAIL);

	}

	m_pMesh->UnlockVertexBuffer();

	Make_Range();
	return S_OK;
}

void Engine::CStaticMesh::Render_Meshes(LPD3DXEFFECT pEffect)
{
	for (_ulong i = 0; i < m_dwSubsetCnt; ++i)
	{
		m_pGraphicDev->SetTexture(0, m_ppTexture[i]);
		m_pMesh->DrawSubset(i);
	}

	if (nullptr != pEffect)
	{
		pEffect->AddRef();
		for (_ulong i = 0; i < m_dwSubsetCnt; ++i)
		{
			_bool	bAlpha = false;
			_uint iPassCnt = 0;

			if (bAlpha = Find_Alpha(m_pSubsets[i].pTextureFilename))
				iPassCnt = 1;

			pEffect->SetTexture("g_BaseTexture", m_ppTexture[i]);
			pEffect->CommitChanges();	// 변경된 텍스처 정보를 정리해주는 함수

			pEffect->BeginPass(iPassCnt);

			m_pMesh->DrawSubset(i);

			pEffect->EndPass();
		}
		Safe_Release(pEffect);
	}
}

_bool CStaticMesh::Find_Alpha(const char * pFileName)
{
	_uint	iLength = strlen(pFileName);

	for (_uint i = 0; i < iLength; ++i)
	{
		if (pFileName[i] == '.')
		{
			if (pFileName[i - 1] == 'A')
				return true;
		}
	}

	return false;
}

HRESULT CStaticMesh::Make_Range(void)
{
	D3DXComputeBoundingBox(m_pVtxPos, m_dwVtxNum, sizeof(_vec3), &m_tRange.vMin, &m_tRange.vMax);
	//auto vCenter = m_tRange.Get_Center();
	//m_tRange.vMax -= vCenter;
	//m_tRange.vMin -= vCenter;
	return S_OK;
}

CStaticMesh* Engine::CStaticMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName)
{
	CStaticMesh*		pInstance = new CStaticMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_Meshes(pFilePath, pFileName)))
		Safe_Release(pInstance);

	return pInstance;
}

CResources* Engine::CStaticMesh::Clone(void)
{
	return new CStaticMesh(*this);
}

void Engine::CStaticMesh::Free(void)
{
	CMesh::Free();

	for (_ulong i = 0; i < m_dwSubsetCnt; ++i)
	{
		Safe_Release(m_ppTexture[i]);
	}
	
	Safe_Release(m_pAdjacency);
	Safe_Release(m_pSubsetBuffer);
	Safe_Release(m_pMesh);
	Safe_Release(m_pOriMesh);

	Safe_Delete_Array(m_ppTexture);

	if (false == m_bClone)
		Safe_Delete_Array(m_pVtxPos);

}

