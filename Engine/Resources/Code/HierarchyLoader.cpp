#include "HierarchyLoader.h"

USING(Engine)

Engine::CHierarchyLoader::CHierarchyLoader(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pPath)
	: m_pGraphicDev(pGraphicDev)
	, m_pPath(pPath)
{
	m_pGraphicDev->AddRef();
}

Engine::CHierarchyLoader::~CHierarchyLoader(void)
{

}

STDMETHODIMP Engine::CHierarchyLoader::CreateFrame(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
	D3DXFRAME_DERIVED*		pNewFrame = new D3DXFRAME_DERIVED;
	ZeroMemory(pNewFrame, sizeof(D3DXFRAME_DERIVED));

	Allocate_Name(&pNewFrame->Name, Name);

	pNewFrame->CombinedTransformationMatrix = *D3DXMatrixIdentity(&pNewFrame->TransformationMatrix);

	*ppNewFrame = pNewFrame;

	return S_OK;
}

STDMETHODIMP Engine::CHierarchyLoader::CreateMeshContainer(THIS_ LPCSTR Name, 
															CONST D3DXMESHDATA *pMeshData, 
															CONST D3DXMATERIAL *pMaterials, 
															CONST D3DXEFFECTINSTANCE *pEffectInstances,
															DWORD NumMaterials, 
															CONST DWORD *pAdjacency, 
															LPD3DXSKININFO pSkinInfo, 
															LPD3DXMESHCONTAINER *ppNewMeshContainer)
{
	D3DXMESHCONTAINER_DERIVED*	pNewMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	ZeroMemory(pNewMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));

	pNewMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	LPD3DXMESH	pMesh = pMeshData->pMesh;
	
	_ulong dwNumFaces = pMesh->GetNumFaces();	// 폴리곤의 개수를 의미

	pNewMeshContainer->pAdjacency = new _ulong[dwNumFaces * 3]; // 인접한 폴리곤의 개수들까지 메모리 할당을 해놓는다.
	memcpy(pNewMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * dwNumFaces * 3);

	_ulong	dwFVF = pMesh->GetFVF();

	if (!(dwFVF & D3DFVF_NORMAL))
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF | D3DFVF_NORMAL, m_pGraphicDev, &pNewMeshContainer->MeshData.pMesh);
		D3DXComputeNormals(pNewMeshContainer->MeshData.pMesh, pNewMeshContainer->pAdjacency);
	}
	else
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF, m_pGraphicDev, &pNewMeshContainer->MeshData.pMesh);
	}

	pNewMeshContainer->NumMaterials = (NumMaterials == 0 ? 1 : NumMaterials);

	pNewMeshContainer->pMaterials = new D3DXMATERIAL[pNewMeshContainer->NumMaterials];
	ZeroMemory(pNewMeshContainer->pMaterials, sizeof(D3DXMATERIAL) * pNewMeshContainer->NumMaterials);

	pNewMeshContainer->ppTexture = new LPDIRECT3DTEXTURE9[pNewMeshContainer->NumMaterials];
	ZeroMemory(pNewMeshContainer->ppTexture, sizeof(LPDIRECT3DTEXTURE9) * pNewMeshContainer->NumMaterials);

	if (0 != NumMaterials)
	{
		memcpy(pNewMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * pNewMeshContainer->NumMaterials);

		for (_ulong i = 0; i < pNewMeshContainer->NumMaterials; ++i)
		{
			_tchar		szFullPath[128] = L"";
			_tchar		szFileName[128] = L"";

			MultiByteToWideChar(CP_ACP,
				0,
				pNewMeshContainer->pMaterials[i].pTextureFilename,
				strlen(pNewMeshContainer->pMaterials[i].pTextureFilename),
				szFileName,
				128);

			lstrcpy(szFullPath, m_pPath);
			lstrcat(szFullPath, szFileName);

			FAILED_CHECK_RETURN(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pNewMeshContainer->ppTexture[i]), E_FAIL);
		}
	}
	else
	{
		pNewMeshContainer->pMaterials[0].MatD3D.Diffuse = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		pNewMeshContainer->pMaterials[0].MatD3D.Specular = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		pNewMeshContainer->pMaterials[0].MatD3D.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 0.f); 
		pNewMeshContainer->pMaterials[0].MatD3D.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		pNewMeshContainer->pMaterials[0].MatD3D.Power = 0.f;

		pNewMeshContainer->ppTexture = nullptr;
	}

	if (nullptr == pSkinInfo)
		return S_OK;

	pNewMeshContainer->pSkinInfo = pSkinInfo;
	pNewMeshContainer->pSkinInfo->AddRef();

	pNewMeshContainer->MeshData.pMesh->CloneMeshFVF(pNewMeshContainer->MeshData.pMesh->GetOptions(), 
													pNewMeshContainer->MeshData.pMesh->GetFVF(), 
													m_pGraphicDev, 
													&pNewMeshContainer->pOriMesh);

	pNewMeshContainer->dwNumBones = pNewMeshContainer->pSkinInfo->GetNumBones(); // 뼈의 개수를 반환


	pNewMeshContainer->pFrameOffSetMatrix = new _matrix[pNewMeshContainer->dwNumBones];
	ZeroMemory(pNewMeshContainer->pFrameOffSetMatrix, sizeof(_matrix) * pNewMeshContainer->dwNumBones);

	
	pNewMeshContainer->ppFrameCombinedMatrix = new _matrix*[pNewMeshContainer->dwNumBones];
	ZeroMemory(pNewMeshContainer->ppFrameCombinedMatrix, sizeof(_matrix*) * pNewMeshContainer->dwNumBones);

	
	pNewMeshContainer->pRenderingMatrix = new _matrix[pNewMeshContainer->dwNumBones];
	ZeroMemory(pNewMeshContainer->pRenderingMatrix, sizeof(_matrix) * pNewMeshContainer->dwNumBones);

	for (_ulong i = 0; i < pNewMeshContainer->dwNumBones; ++i)
	{
		pNewMeshContainer->pFrameOffSetMatrix[i] = *pNewMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i); // 메쉬를 그리기 위한 뼈에는 인덱스 모두 있다. 이런 뼈들을 순회하면서 행렬 정보를 얻어오는 함수
	}
	
	*ppNewMeshContainer = pNewMeshContainer;

	return S_OK;
}

STDMETHODIMP Engine::CHierarchyLoader::DestroyFrame(THIS_ LPD3DXFRAME pFrameToFree)
{
	Safe_Delete_Array(pFrameToFree->Name);

	if (nullptr != pFrameToFree->pMeshContainer)
		DestroyMeshContainer(pFrameToFree->pMeshContainer);

	if (nullptr != pFrameToFree->pFrameSibling)
		DestroyFrame(pFrameToFree->pFrameSibling);

	if (nullptr != pFrameToFree->pFrameFirstChild)
		DestroyFrame(pFrameToFree->pFrameFirstChild);

	Safe_Delete(pFrameToFree);

	return S_OK;
}

STDMETHODIMP Engine::CHierarchyLoader::DestroyMeshContainer(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerToFree;

	// 텍스쳐 삭제
	for (_ulong i = 0; i < pMeshContainer->NumMaterials; ++i)
		Safe_Release(pMeshContainer->ppTexture[i]);

	Safe_Delete_Array(pMeshContainer->ppTexture);

	Safe_Release(pMeshContainer->pOriMesh);

	Safe_Delete_Array(pMeshContainer->pFrameOffSetMatrix);
	Safe_Delete_Array(pMeshContainer->ppFrameCombinedMatrix);
	Safe_Delete_Array(pMeshContainer->pRenderingMatrix);

	Safe_Delete_Array(pMeshContainer->Name);
	Safe_Release(pMeshContainer->MeshData.pMesh);
	Safe_Delete_Array(pMeshContainer->pMaterials);
	Safe_Delete_Array(pMeshContainer->pAdjacency);
	Safe_Release(pMeshContainer->pSkinInfo);

	Safe_Delete(pMeshContainer);

	return S_OK;
}

void Engine::CHierarchyLoader::Allocate_Name(char** ppName, const char* pFrameName)
{
	if (nullptr == pFrameName)
		return;

	_uint	iLength = strlen(pFrameName);

	*ppName = new char[iLength + 1];

	strcpy_s(*ppName, iLength + 1, pFrameName);

}

CHierarchyLoader* Engine::CHierarchyLoader::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pPath)
{
	return new CHierarchyLoader(pGraphicDev, pPath);
}

_ulong Engine::CHierarchyLoader::Release(void)
{
	Engine::Safe_Release(m_pGraphicDev);

	delete this;

	return 0;
}

