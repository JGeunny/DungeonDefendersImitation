#include "Export_Resources.h"
inline const std::list<std::wstring>& Get_MeshTagList(RESOURCEID eResourceType)
{
	return CResourcesMgr::GetInstance()->Get_MeshTagList(eResourceType);
}
// ResourcesMgr
// Get
// Set
// General
HRESULT	Reserve_ContainerSize(const _ushort& wSize)
{
	return CResourcesMgr::GetInstance()->Reserve_ContainerSize(wSize);
}
HRESULT	Ready_Buffers(LPDIRECT3DDEVICE9 pGraphicDev, const _ushort& wContainerIdx, const _tchar* pBufferTag, BUFFERID eID,	const _ulong& dwCntX,	const _ulong& dwCntZ,	const _ulong& dwVtxItv)
{
	return CResourcesMgr::GetInstance()->Ready_Buffers(pGraphicDev, wContainerIdx, pBufferTag, eID, dwCntX, dwCntZ, dwVtxItv);
}
HRESULT		Ready_Texture(LPDIRECT3DDEVICE9 pGraphicDev, const _ushort& wContainerIdx, const _tchar* pTextureTag, TEXTURETYPE eType, const _tchar* pPath, const _uint& iCnt)
{
	return CResourcesMgr::GetInstance()->Ready_Texture(pGraphicDev, wContainerIdx, pTextureTag, eType, pPath, iCnt);
}

HRESULT		Ready_Meshes(LPDIRECT3DDEVICE9 pGraphicDev, const _ushort& wContainerIdx, const _tchar* pMeshTag, MESHTYPE eType, const _tchar* pFilePath, const _tchar* pFileName)
{
	return CResourcesMgr::GetInstance()->Ready_Meshes(pGraphicDev, wContainerIdx, pMeshTag, eType, pFilePath, pFileName);
}

HRESULT Ready_ResourceFromPathFile(LPDIRECT3DDEVICE9 pDevice, const wstring & wstrFilePath)
{
	return CResourcesMgr::GetInstance()->Ready_ResourceFromPathFile(pDevice, wstrFilePath);
}


void		Render_Buffer(const _ushort& wContainerIdx, const _tchar* pBufferTag)
{
	CResourcesMgr::GetInstance()->Render_Buffer(wContainerIdx, pBufferTag);
}
void		Render_Texture(const _ushort& wContainerIdx, const _tchar* pTextureTag, const _uint& iIndex)
{
	CResourcesMgr::GetInstance()->Render_Texture(wContainerIdx, pTextureTag, iIndex);
}

CResources*		Clone(const _ushort& wContainerIdx, const _tchar* pResourceTag)
{
	return CResourcesMgr::GetInstance()->Clone(wContainerIdx, pResourceTag);
}

CResources * Original(const _ushort & wContainerIdx, const _tchar * pResourceTag)
{
	return CResourcesMgr::GetInstance()->Original(wContainerIdx, pResourceTag);
}


// Release Resources
void		Release_Resources(void)
{
	CResourcesMgr::GetInstance()->DestroyInstance();
}