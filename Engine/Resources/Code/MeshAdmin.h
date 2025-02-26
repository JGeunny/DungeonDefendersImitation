#ifndef MeshAdmin_h__
#define MeshAdmin_h__

#include "Resources.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"
#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CMeshAdmin : public CComponent
{
protected:
	explicit		CMeshAdmin(CMesh** ppMeshCom, CCollider** ppColliderCom = nullptr);
	virtual			~CMeshAdmin(void);

public:
	HRESULT			Ready_MeshAdmin();
	void			Change_Mesh(RESOURCEID eResourceType, const _tchar* pMeshName);

public:
	CMesh*			Get_Mesh() { return *m_ppMeshCom; }
	std::wstring	Get_MeshKey() { return m_strMeshKey; }
	Engine::MESHID	Get_MeshId() { return (*m_ppMeshCom)->Get_MeshId(); }
	RESOURCEID		Get_ResourceType() { return m_eResourceType; }
	const D3DXFRAME_DERIVED*	Get_FrameByName(const char* pFrameName);

	void			Set_AnimationSet(std::string strAniName);
	void			Set_AnimationSet(_int index);
	std::string		Get_AnimationSet();
	_bool			Is_AnimationSetEnd(RANGEFLOAT* pRangeFloat = nullptr);
	void			Play_Animation(_float fTimeDelta, _int* piCount = nullptr);

private:
	CMesh**			m_ppMeshCom;
	CCollider**		m_ppColliderCom;
	std::wstring	m_strMeshKey;
	RESOURCEID		m_eResourceType;

public:
	static CMeshAdmin*	Create(CMesh** ppMeshCom, CCollider** ppColliderCom = nullptr);

public:
	virtual void Free(void);
};

END
#endif // MeshAdmin_h__
