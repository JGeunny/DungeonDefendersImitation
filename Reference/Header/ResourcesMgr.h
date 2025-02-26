#ifndef ResourcesMgr_h__
#define ResourcesMgr_h__

#include "Engine_Defines.h"
#include "Base.h"

#include "Resources.h"
#include "TriCol.h"
#include "RcTex.h"
#include "TerrainTex.h"
#include "CubeTex.h"
#include "Curve.h"
#include "VIBuffer_ViewPort.h"

#include "Texture.h"

#include "StaticMesh.h"
#include "DynamicMesh.h"
#include "NaviMesh.h"

BEGIN(Engine)

class ENGINE_DLL CResourcesMgr : public CBase
{
	DECLARE_SINGLETON(CResourcesMgr)

private:
	explicit CResourcesMgr(void);
	virtual ~CResourcesMgr(void);

public:
	HRESULT	Reserve_ContainerSize(const _ushort& wSize);
	
	HRESULT	Ready_Buffers(LPDIRECT3DDEVICE9 pGraphicDev, 
							const _ushort& wContainerIdx,
							const _tchar* pBufferTag,
							BUFFERID eID, 
							const _ulong& dwCntX = 1, 
							const _ulong& dwCntZ = 1, 
							const _ulong& dwVtxItv = 1);

	HRESULT		Ready_Texture(LPDIRECT3DDEVICE9 pGraphicDev,
								const _ushort& wContainerIdx,
								const _tchar* pTextureTag,
								TEXTURETYPE eType,
								const _tchar* pPath,
								const _uint& iCnt = 1);
	
	HRESULT		Ready_Meshes(LPDIRECT3DDEVICE9 pGraphicDev, 
								const _ushort& wContainerIdx,
								const _tchar* pMeshTag, 
								MESHTYPE eType,
								const _tchar* pFilePath,
								const _tchar* pFileName);


	void		Render_Texture(const _ushort& wContainerIdx, 
								const _tchar* pTextureTag, 
								const _uint& iIndex = 0);
	
	void		 Render_Buffer(const _ushort& wContainerIdx,
								const _tchar* pBufferTag);

	HRESULT		Ready_ResourceFromPathFile(LPDIRECT3DDEVICE9 pDevice, 
							const wstring & wstrFilePath);

	CResources*		Clone(const _ushort& wContainerIdx,
		const _tchar* pResourceTag);

	CResources*		Original(const _ushort& wContainerIdx,
		const _tchar* pResourceTag);

private:
	CResources*	Find_Resources(const _ushort& wContainerIdx,
							   const _tchar* pResourcesTag);

public:
	const std::list<std::wstring>&	Get_MeshTagList(RESOURCEID eResourceType) {
		return m_lstMesheTag[eResourceType];
	}

private:
	map<std::wstring, CResources*>*		m_pmapResources = nullptr;
	_ushort									m_wContainerSize = 0;
	//Ready_Meshes 한 메쉬의 목록을 보기 위한 리스트
	std::list<std::wstring>					m_lstMesheTag[RESOURCE_END];

public:
	virtual void Free(void);

};

END
#endif // ResourcesMgr_h__
