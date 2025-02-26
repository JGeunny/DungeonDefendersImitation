#include "DynamicMesh.h"

USING(Engine)

Engine::CDynamicMesh::CDynamicMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMesh(pGraphicDev)
	, m_pLoader(nullptr)
	, m_pAniCtrl(nullptr)
{
	m_eMeshId = MESH_DYNAMIC;
}

Engine::CDynamicMesh::CDynamicMesh(const CDynamicMesh& rhs)
	: CMesh(rhs)	
	, m_pLoader(rhs.m_pLoader)
	, m_pRootFrame(rhs.m_pRootFrame)
	, m_MeshContainerList(rhs.m_MeshContainerList)
{
	m_pAniCtrl = CAniCtrl::Create(*rhs.m_pAniCtrl);
	m_eMeshId = MESH_DYNAMIC;
}

Engine::CDynamicMesh::~CDynamicMesh(void)
{

}

HRESULT Engine::CDynamicMesh::Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName)
{
	_tchar	szFullPath[128] = L"";
	
	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);
	
	m_pLoader = CHierarchyLoader::Create(m_pGraphicDev, pFilePath);
	NULL_CHECK_RETURN(m_pLoader, E_FAIL);
	
	LPD3DXANIMATIONCONTROLLER	pAniCtrl = nullptr;
	
	//FAILED_CHECK_RETURN(D3DXLoadMeshHierarchyFromX(szFullPath,
	//											D3DXMESH_MANAGED,
	//											m_pGraphicDev,
	//											m_pLoader,	// hierachy 클래스의 포인터
	//											NULL,
	//											&m_pRootFrame,
	//											&pAniCtrl), // anitCtrl 클래스의 포인터
	//											E_FAIL);
	D3DXLoadMeshHierarchyFromX(szFullPath,
		D3DXMESH_MANAGED,
		m_pGraphicDev,
		m_pLoader,	// hierachy 클래스의 포인터
		NULL,
		&m_pRootFrame,
		&pAniCtrl);

	m_pAniCtrl = CAniCtrl::Create(pAniCtrl);
	NULL_CHECK_RETURN(m_pAniCtrl, E_FAIL);

	Safe_Release(pAniCtrl);

	_matrix		matTemp;
	
	Update_FrameMatrices((D3DXFRAME_DERIVED*)m_pRootFrame, D3DXMatrixRotationY(&matTemp, D3DXToRadian(180.f)));

	SetUp_FrameMatrixPointer((D3DXFRAME_DERIVED*)m_pRootFrame);

	Make_Range();
	return S_OK;
}

void Engine::CDynamicMesh::Render_Meshes(LPD3DXEFFECT pEffect)
{
	for (auto& pMeshContainer : m_MeshContainerList)
	{

		for (_ulong i = 0; i < pMeshContainer->dwNumBones; ++i)
		{
			pMeshContainer->pRenderingMatrix[i] = pMeshContainer->pFrameOffSetMatrix[i] * (*pMeshContainer->ppFrameCombinedMatrix[i]);
		}

		void*		pSrcVtx = nullptr;
		void*		pDestVtx = nullptr;

		pMeshContainer->pOriMesh->LockVertexBuffer(0, &pSrcVtx);
		pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pDestVtx);

		// 소프트웨어 스키닝을 수행하는 함수(스키닝 뿐만 아니라 애니메이션 변경 시, 뼈대들과 정점 정보들의 변경을 동시에 수행을 하기도 한다)
		pMeshContainer->pSkinInfo->UpdateSkinnedMesh(pMeshContainer->pRenderingMatrix, 
														NULL,		// 원래 대로 돌리기 위한 상태 행렬(본래는 행렬에 대한 역행렬을 싹다 구해서 넣어줘야한다고 함. 안 넣어줘도 됨)
														pSrcVtx,	// 변하지 않는 원본 메쉬의 정점 정보 상태
														pDestVtx);	// 변환된 정보를 담기 위한 정점 정보 상태

		for (_ulong i = 0; i < pMeshContainer->NumMaterials; ++i)
		{
			if (nullptr != pEffect)
			{
				pEffect->SetTexture("g_BaseTexture", pMeshContainer->ppTexture[i]);
				pEffect->CommitChanges();	// 변경된 텍스처 정보를 정리해주는 함수
			}
			else
				m_pGraphicDev->SetTexture(0, pMeshContainer->ppTexture[i]);

			pMeshContainer->MeshData.pMesh->DrawSubset(i);
		}

		pMeshContainer->pOriMesh->UnlockVertexBuffer();
		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
	}
}

_bool Engine::CDynamicMesh::Is_AnimationSetEnd(RANGEFLOAT* pRangeFloat)
{
	return m_pAniCtrl->Is_AnimationSetEnd(pRangeFloat);
}

void CDynamicMesh::Set_AnimationSet(const _uint & iIndex)
{
	m_pAniCtrl->Set_AnimationSet(iIndex);
}

void CDynamicMesh::Set_AnimationSet(std::string strAniName)
{
	m_pAniCtrl->Set_AnimationSet(strAniName);
}

std::string CDynamicMesh::Get_AnimationSet()
{
	return m_pAniCtrl->Get_AnimationSet();
}

void CDynamicMesh::Play_Animation(const _float & fTimeDelta, _int* piCount)
{
	m_pAniCtrl->Play_Animation(fTimeDelta, piCount);

	_matrix	matTemp;
	Update_FrameMatrices((D3DXFRAME_DERIVED*)m_pRootFrame, D3DXMatrixRotationY(&matTemp, D3DXToRadian(180.f)));
}

HRESULT CDynamicMesh::Make_Range(void)
{
	_matrix				matWorld;
	_ulong				dwVtxNum;
	//_vec3*				pVtxPos;
	//_ulong				dwStride;
	RANGE				tRange_Temp;
	m_tRange.vMax = { -9999.f, -9999.f, -9999.f };
	m_tRange.vMin = { 9999.f, 9999.f, 9999.f };
//	D3DXMatrixIdentity(&matWorld);

	for (auto& pMeshData : m_MeshContainerList)
	{
		void*		pSrcVtx = nullptr;

		pMeshData->pOriMesh->LockVertexBuffer(0, &pSrcVtx);
		pMeshData->pOriMesh->UnlockVertexBuffer();
		dwVtxNum = pMeshData->pOriMesh->GetNumVertices();

		D3DXComputeBoundingBox((_vec3*)pSrcVtx, dwVtxNum, sizeof(_vec3), &tRange_Temp.vMin, &tRange_Temp.vMax);

		D3DXVec3Minimize(&m_tRange.vMin, &m_tRange.vMin, &tRange_Temp.vMin);
		D3DXVec3Maximize(&m_tRange.vMax, &m_tRange.vMax, &tRange_Temp.vMax);
		break;
	}
	auto vVolume = m_tRange.Get_Volume() / 5.f;
	m_tRange.vMin += vVolume;
	m_tRange.vMax -= vVolume;
	auto vCenter = m_tRange.Get_Center();
	auto vVertical = m_tRange.Get_Vertical();
	m_tRange.vMax += -vCenter + vVertical;
	m_tRange.vMin += -vCenter + vVertical;
	return S_OK;
}

LPD3DXMESH CDynamicMesh::Get_Mesh()
{
	LPD3DXMESH pMesh = nullptr;
	if (m_MeshContainerList.size() > 1)
		pMesh = m_MeshContainerList.front()->MeshData.pMesh;
	return pMesh;
}

const D3DXFRAME_DERIVED* Engine::CDynamicMesh::Get_FrameByName(const char* pFrameName)
{
	return (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pFrameName);
}

void Engine::CDynamicMesh::Update_FrameMatrices(D3DXFRAME_DERIVED* pFrame, const _matrix* pParentMatrix)
{
	if (nullptr == pFrame)
		return;
	pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix * (*pParentMatrix);

	if (nullptr != pFrame->pFrameSibling)
		Update_FrameMatrices((D3DXFRAME_DERIVED*)pFrame->pFrameSibling, pParentMatrix);

	if (nullptr != pFrame->pFrameFirstChild)
		Update_FrameMatrices((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);

}

CDynamicMesh* Engine::CDynamicMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName)
{
	CDynamicMesh*	pInstance = new CDynamicMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_Meshes(pFilePath, pFileName)))
		Safe_Release(pInstance);

	return pInstance;
}

CResources* Engine::CDynamicMesh::Clone(void)
{
	return new CDynamicMesh(*this);
}

void Engine::CDynamicMesh::Free(void)
{
	CMesh::Free();

	Safe_Release(m_pAniCtrl);

	if (false == m_bClone)
	{
		m_pLoader->DestroyFrame(m_pRootFrame);
		Safe_Release(m_pLoader);
	}

	m_MeshContainerList.clear();
}

void Engine::CDynamicMesh::SetUp_FrameMatrixPointer(D3DXFRAME_DERIVED* pFrame)
{
	if (nullptr != pFrame->pMeshContainer)
	{
		D3DXMESHCONTAINER_DERIVED*	pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;

		for (_ulong i = 0; i < pMeshContainer->dwNumBones; ++i)
		{
			const char*	pBoneName = pMeshContainer->pSkinInfo->GetBoneName(i);	 // 메쉬 안에 있는 뼈 이름을 얻어오는 함수
			D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pBoneName); // 문자열을 기준으로 최초 뼈대부터 뼈를 찾아주는 함수
			
			pMeshContainer->ppFrameCombinedMatrix[i] = &pFrame->CombinedTransformationMatrix;			
		}

		m_MeshContainerList.push_back(pMeshContainer);
	}

	if(nullptr != pFrame->pFrameSibling)
		SetUp_FrameMatrixPointer((D3DXFRAME_DERIVED*)pFrame->pFrameSibling);

	if(nullptr != pFrame->pFrameFirstChild)
		SetUp_FrameMatrixPointer((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild);

}

