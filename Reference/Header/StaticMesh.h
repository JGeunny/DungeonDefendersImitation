#ifndef StaticMesh_h__
#define StaticMesh_h__

#include "Mesh.h"

BEGIN(Engine)

class ENGINE_DLL CStaticMesh : public CMesh
{
private:
	explicit			CStaticMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CStaticMesh(const CStaticMesh& rhs);
	virtual				~CStaticMesh(void);

public:
	_ulong				Get_VtxNum(void) const { return m_dwVtxNum; }
	_ulong				Get_Stride(void) const { return m_dwStride; }
	const _vec3*		Get_VtxPos(void) const { return m_pVtxPos; }
	LPD3DXMESH			Get_Mesh() { return m_pMesh; }
public:
	HRESULT				Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName);
	void				Render_Meshes(LPD3DXEFFECT pEffect = nullptr);

private:
	_bool				Find_Alpha(const char* pFileName);

public:
	HRESULT				Make_Range(void);
private:
	LPD3DXMESH			m_pMesh;
	
	LPD3DXBUFFER		m_pAdjacency;
	LPD3DXBUFFER		m_pSubsetBuffer;

	D3DXMATERIAL*		m_pSubsets;
	_ulong				m_dwSubsetCnt;
	LPDIRECT3DTEXTURE9*	m_ppTexture;

	_ulong				m_dwVtxNum;
	_vec3*				m_pVtxPos;
	
	LPD3DXMESH			m_pOriMesh;
	_ulong				m_dwStride;

public:
	static CStaticMesh*	Create(LPDIRECT3DDEVICE9 pGraphicDev,
								const _tchar* pFilePath,
								const _tchar* pFileName);
	virtual CResources*	Clone(void);
private:
	virtual void		Free(void);

};

END
#endif // StaticMesh_h__
