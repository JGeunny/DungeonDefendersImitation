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

	_ulong		dwFVF = m_pOriMesh->GetFVF(); // ���� �޽��� ���� ������ �Ӽ� ���� ������ �Լ�

	// FVF �Ӽ� �� NORMAL ������ ���� ���
	if (!(dwFVF & D3DFVF_NORMAL))
	{
		// ������ load�� �޽��� fvf ������ �����ϴ� �Լ�
		m_pOriMesh->CloneMeshFVF(m_pOriMesh->GetOptions(), dwFVF | D3DFVF_NORMAL, m_pGraphicDev, &m_pMesh);
		
		// �޽� ������ normal ���� ����Ͽ� �־��ִ� �Լ�
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

	D3DVERTEXELEMENT9		Decl[MAX_FVF_DECL_SIZE];	// ���� �ϳ��� ���� �� �ִ� �ִ��� FVF ����
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

	// ������ fvf�� �˰� ���� ��, ������ ���� ũ�⸦ ���´�.
	m_dwStride = D3DXGetFVFVertexSize(dwFVF);

	for (_ulong i = 0; i < m_dwVtxNum; ++i)
	{
		m_pVtxPos[i] = *((_vec3*)(((_ubyte*)pVertex) + (m_dwStride * i + byOffset)));
	}
	
	// �İ�ü�� ��� �ִ� ���� ���� �� ���� �� �ּҸ� �����ϴ� �Լ�
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
			pEffect->CommitChanges();	// ����� �ؽ�ó ������ �������ִ� �Լ�

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

