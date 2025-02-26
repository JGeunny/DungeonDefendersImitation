#ifndef Mesh_h__
#define Mesh_h__

#include "Resources.h"

BEGIN(Engine)

class ENGINE_DLL CMesh : public CResources
{
protected:
	explicit			CMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CMesh(const CMesh& rhs);
	virtual				~CMesh(void);

public:
	virtual LPD3DXMESH	Get_Mesh() { return nullptr; }
	virtual MESHID		Get_MeshId() { return m_eMeshId; }
	virtual void		Render_Meshes(LPD3DXEFFECT pEffect = nullptr) {}

	const RANGE&		Get_Range(void) const { return m_tRange; }
	virtual HRESULT		Make_Range(void) PURE;
protected:
	_bool				m_bClone;
	MESHID				m_eMeshId;
	RANGE				m_tRange;

public:
	virtual CResources*		Clone(void)PURE;
public:
	virtual void Free(void);

};

END
#endif // Mesh_h__
