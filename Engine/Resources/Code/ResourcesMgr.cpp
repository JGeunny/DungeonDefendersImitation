#include "ResourcesMgr.h"

USING(Engine)
IMPLEMENT_SINGLETON(CResourcesMgr)

Engine::CResourcesMgr::CResourcesMgr(void)
{

}

Engine::CResourcesMgr::~CResourcesMgr(void)
{
	Free();
}

HRESULT Engine::CResourcesMgr::Reserve_ContainerSize(const _ushort& wSize)
{
	if (nullptr != m_pmapResources)
		return E_FAIL;

	m_pmapResources = new std::map<std::wstring, CResources*>[wSize];

	m_wContainerSize = wSize;

	return S_OK;
}

HRESULT Engine::CResourcesMgr::Ready_Buffers(LPDIRECT3DDEVICE9 pGraphicDev,
										const _ushort& wContainerIdx,
										const _tchar* pBufferTag,
										BUFFERID eID,
										const _ulong& dwCntX ,
										const _ulong& dwCntZ ,
										const _ulong& dwVtxItv)
{
	if (nullptr == m_pmapResources)
	{
		MSG_BOX("Resources Container not Reserved");
		return E_FAIL;
	}

	CResources*		pResources = Find_Resources(wContainerIdx, pBufferTag);

	if (nullptr != pResources)
		return E_FAIL;

	switch (eID)
	{
	case BUFFER_TRICOL:
		pResources = CTriCol::Create(pGraphicDev);
		break;

	case BUFFER_RCTEX:
		pResources = CRcTex::Create(pGraphicDev);
		break;

	case BUFFER_TERRAINTEX:
		pResources = CTerrainTex::Create(pGraphicDev, dwCntX, dwCntZ, dwVtxItv);
		break;

	case BUFFER_CUBETEX:
		pResources = CCubeTex::Create(pGraphicDev);
		break;

	case BUFFER_VIEWPORT:
		pResources = CVIBuffer_ViewPort::Create(pGraphicDev, 0.f, 0.f, 200.f, 200.f);
		break;
	}

	NULL_CHECK_RETURN(pResources, E_FAIL);

	m_pmapResources[wContainerIdx].emplace(pBufferTag, pResources);

	return S_OK;
}

CResources* Engine::CResourcesMgr::Find_Resources(const _ushort& wContainerIdx, const _tchar* pResourcesTag)
{
	auto iter = m_pmapResources[wContainerIdx].find(pResourcesTag);
	if (iter == m_pmapResources[wContainerIdx].end())
		return nullptr;

	return iter->second;
}

void Engine::CResourcesMgr::Free(void)
{
	for (auto& lstTag : m_lstMesheTag)
		lstTag.clear();
	
	for (_ushort i = 0; i < m_wContainerSize; ++i)
	{
		for_each(m_pmapResources[i].begin(), m_pmapResources[i].end(), CDeleteMap());
		m_pmapResources[i].clear();
	}

	Safe_Delete_Array(m_pmapResources);
}

void Engine::CResourcesMgr::Render_Buffer(const _ushort& wContainerIdx,
											const _tchar* pBufferTag)
{
	CResources*		pResources = Find_Resources(wContainerIdx, pBufferTag);

	if (nullptr == pResources)
		return;

	dynamic_cast<CVIBuffer*>(pResources)->Render_Buffer();
}

CResources* Engine::CResourcesMgr::Clone(const _ushort& wContainerIdx,
										 const _tchar* pResourceTag)
{
	if (nullptr == m_pmapResources)
		return nullptr;

	CResources*		pPrototype = Find_Resources(wContainerIdx, pResourceTag);

	if (nullptr == pPrototype)
		return nullptr;
	return pPrototype->Clone();
}

CResources * CResourcesMgr::Original(const _ushort & wContainerIdx, const _tchar * pResourceTag)
{
	if (nullptr == m_pmapResources)
		return nullptr;

	CResources*		pPrototype = Find_Resources(wContainerIdx, pResourceTag);

	return pPrototype;
}

HRESULT Engine::CResourcesMgr::Ready_Texture(LPDIRECT3DDEVICE9 pGraphicDev, 
											const _ushort& wContainerIdx, 
											const _tchar* pTextureTag, 
											TEXTURETYPE eType, 
											const _tchar* pPath,
											const _uint& iCnt /*= 1*/)
{
	if (nullptr == m_pmapResources)
	{
		MSG_BOX("Resources Container not Reserved");
		return E_FAIL;
	}

	CResources*		pResources = Find_Resources(wContainerIdx, pTextureTag);

	if (nullptr != pResources)
		return E_FAIL;

	pResources = CTexture::Create(pGraphicDev, pPath, eType, iCnt);
	NULL_CHECK_RETURN(pResources, E_FAIL);

	m_pmapResources[wContainerIdx].emplace(pTextureTag, pResources);

	return S_OK;
}

void Engine::CResourcesMgr::Render_Texture(const _ushort& wContainerIdx, 
	const _tchar* pTextureTag, 
	const _uint& iIndex /*= 0*/)
{
	CResources*		pResources = Find_Resources(wContainerIdx, pTextureTag);

	if (nullptr == pResources)
		return;

	dynamic_cast<CTexture*>(pResources)->Render_Texture(iIndex);
}

HRESULT Engine::CResourcesMgr::Ready_Meshes(LPDIRECT3DDEVICE9 pGraphicDev,
											const _ushort& wContainerIdx,
											const _tchar* pMeshTag,
											MESHTYPE eType,
											const _tchar* pFilePath,
											const _tchar* pFileName)
{
	if (nullptr == m_pmapResources)
	{
		MSG_BOX("Resources Container not Reserved");
		return E_FAIL;
	}

	CResources*		pResources = Find_Resources(wContainerIdx, pMeshTag);

	if (nullptr != pResources)
		return E_FAIL;

	switch (eType)
	{
	case TYPE_STATIC:
		pResources = CStaticMesh::Create(pGraphicDev, pFilePath, pFileName);
		break;

	case TYPE_DYNAMIC:
		pResources = CDynamicMesh::Create(pGraphicDev, pFilePath, pFileName);
		break;

	case TYPE_NAVI:
		pResources = CNaviMesh::Create(pGraphicDev);
		break;
	}

	NULL_CHECK_RETURN(pResources, E_FAIL);
	m_lstMesheTag[wContainerIdx].emplace_back(pMeshTag);

	m_pmapResources[wContainerIdx].emplace(pMeshTag, pResources);

	return S_OK;
}

HRESULT CResourcesMgr::Ready_ResourceFromPathFile(LPDIRECT3DDEVICE9 pDevice, const wstring & wstrFilePath)
{
	wifstream fin;

	fin.open(wstrFilePath);

	if (fin.fail())
	{
		ERR_MSG(wstrFilePath.c_str());
		ERR_MSG(L"불러오기 실패");

		return E_FAIL;
	}

	TCHAR szType[MAX_STR] = L"";
	TCHAR szObjectKey[MAX_STR] = L"";
	TCHAR szCount[MIN_STR] = L"";
	TCHAR szRelativePath[MAX_STR] = L"";
	TCHAR szFileNamePath[MAX_STR] = L"";
	std::wstring strType = L"";

	HRESULT hr = 0;

	while (true)
	{
		fin.getline(szType, MAX_STR, '|');	// '|' 단위로 끊어서 읽어들임.
		fin.getline(szObjectKey, MAX_STR, '|');	// '|' 단위로 끊어서 읽어들임.
		fin.getline(szCount, MIN_STR, '|');
		fin.getline(szRelativePath, MAX_STR, '|'); // 개행 단위로 끊어서 읽어들임.
		fin.getline(szFileNamePath, MAX_STR); // 개행 단위로 끊어서 읽어들임.

		if (fin.eof())
			break;
		
		if (!lstrcmp(szType, L"Default")) {
			hr = Ready_Texture(pDevice,
				RESOURCEID::RESOURCE_STAGE,
				szObjectKey, Engine::TEX_NORMAL,
				szRelativePath,
				_wtoi(szCount));
		}
		else if (!lstrcmp(szType, L"DynamicMesh")) {
			hr = Ready_Meshes(pDevice,
				RESOURCE_STAGE,
				szObjectKey,
				Engine::TYPE_DYNAMIC,
				szRelativePath,
				szFileNamePath);
		}
		else if (!lstrcmp(szType, L"StaticMesh")) {
			hr = Ready_Meshes(pDevice,
				RESOURCE_STAGE,
				szObjectKey,
				Engine::TYPE_STATIC,
				szRelativePath,
				szFileNamePath);
		}
		else
			hr = E_FAIL;

		if (FAILED(hr))
		{
			fin.close();
			return E_FAIL;
		}
	}

	fin.close();

	return S_OK;
}