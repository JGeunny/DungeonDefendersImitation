#ifndef Collider_h__
#define Collider_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider : public CComponent
{
private:
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCollider(void);

public:
	const	RANGE*		Get_Range(void) const { return &m_tRange; }
	const _matrix*		Get_ColliderWorld() { return &m_matWorld; }
	const _ulong		Get_UsingCollision() const { return m_dwUsingCollision; }
	const LPD3DXMESH	Get_ColliderMesh() const { return m_pColliderMesh; }
	Engine::COLLMESHTYPE Get_CollMeshType() { return m_eCollMeshType; }
public:
	void				Set_UsingCollision(_ulong dwUsingCollision)  { m_dwUsingCollision = dwUsingCollision; }
	void				Add_UsingCollision(_ulong dwUsingCollision) { m_dwUsingCollision |= dwUsingCollision; }
	void				Set_CollMeshType(Engine::COLLMESHTYPE eCollMeshType) { m_eCollMeshType = eCollMeshType; }
	
private:
	HRESULT		Ready_Collider();

public:
	HRESULT		Change_Collider(const RANGE& tRange);
	void		Render_Collider(const _matrix* pColliderMatrix);

	COLLTYPE	Get_CollType() { return m_eCollType;	}
	void		Set_CollType(COLLTYPE eCollType) { m_eCollType = eCollType; }
private:
	Engine::COLLMESHTYPE m_eCollMeshType = Engine::COLLMESHTYPE_BOX;
	RANGE		m_tRange;
	_matrix		m_matWorld;
	_ulong		m_dwUsingCollision;
	HRESULT		Set_MeshColor(LPD3DXMESH& pMesh, DWORD dwColor);

//#ifdef _DEBUG
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	COLLTYPE				m_eCollType;
	COLLTYPE				m_eOldCollType;
	DWORD					m_dwColor;
	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;
	LPDIRECT3DTEXTURE9		m_pTexture[COL_END];
	LPD3DXMESH				m_pColliderMesh;
	_matrix					m_matWorld_Interval;
//#endif

public:
	static CCollider*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void);

};

END

#endif // Collider_h__

