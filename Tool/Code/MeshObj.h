#ifndef MeshObj_h__
#define MeshObj_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CObjectInfo;
class CTransform;
class CRenderer;
class CCalculator;
class CStaticMesh;
class CCollider;

class CMesh;
class CMeshAdmin;
class CShader;

END

class CMeshObj : public Engine::CGameObject
{
private:
	explicit CMeshObj(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMeshObj(void);

public:
	virtual HRESULT Ready_Object(const _uint& iFlag);
	virtual void	LateReady_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;
public:
	virtual void	BeCollision(_ulong dwUsingCollision, Engine::CGameObject* pGameObject);

private:
	HRESULT			Add_Component(void);
	virtual HRESULT	SetUp_ConstantTable(LPD3DXEFFECT pEffect);

private:
	Engine::CObjectInfo*		m_pObjectInfo = nullptr;
	Engine::CMeshAdmin*			m_pMeshAdminCom = nullptr;
	Engine::CMesh*				m_pMeshCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CCalculator*		m_pCalculatorCom = nullptr;
	Engine::CCollider*			m_pColliderCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;

	_float						m_fTimeDelta_AniPlay;
	_int						m_iAniCount = 0;

	_float						m_fFrame = 0.f;
	_vec3						m_vDir;
	_float						m_fSpeed = 5.f;
	_uint						m_iFlag;

public:
	static CMeshObj*			Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

private:
	virtual void Free(void) override;


};

#endif // MeshObj_h__
