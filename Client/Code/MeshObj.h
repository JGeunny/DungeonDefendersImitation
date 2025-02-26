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
public:
	enum ANICOUNT { ANICOUNT_NOIDLE, ANICOUNT_ATTACK, ANICOUNT_END};
protected:
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


public:
	void			Effect_Update(float fTimeDelta);
	void			Effect_BeAttacked_Update(float fTimeDelta);
	void			Effect_KnockBack_Update(float fTimeDelta);

	void			Effect_BeAttacked();
	void			Effect_KnockBack(_vec3 vTargetPos, float fPower = 4.f);

public:
	HRESULT			Init_AniCount();
	float			Get_ShaderType() { return m_fShaderType; }
	void			Set_ShaderType(_float fShaderType) {  m_fShaderType = fShaderType; }

	HRESULT			Set_NaviPos(_vec3* pvTargetDir, bool bComputtingHeight = true, bool* pbUpGround = nullptr);
	HRESULT			Seek_NaviIndex();

	virtual _vec3	Get_StartBulletPos();
protected:
	HRESULT			Add_Component(void);
	virtual HRESULT	SetUp_ConstantTable(LPD3DXEFFECT pEffect);

protected:
	Engine::CObjectInfo*		m_pObjectInfo = nullptr;
	Engine::CMeshAdmin*			m_pMeshAdminCom = nullptr;
	Engine::CMesh*				m_pMeshCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CCalculator*		m_pCalculatorCom = nullptr;
	Engine::CCollider*			m_pColliderCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;

	_float						m_fTimeDelta_AniPlay;

	std::string					m_strAniName;
	_int						m_iAniCount = 0;
	_int						m_iAniCount_Before = 0;
	_bool						m_abAniCount[ANICOUNT_END];

	_float						m_fFrame = 0.f;
	_vec3						m_vDir;
	_float						m_fSpeed = 5.f;
	_uint						m_iFlag;

	_float						m_fShaderType = 0.f;
	_float						m_fShaderType_Buffer = -1.f;
	_float						m_fShaderType_BufferTime = 0.f;

	_bool						m_bKnockBack_UpGround;
	_vec3						m_vKnockBack_Dir;
	_float						m_vKnockBack_Power;
	_float						m_fJumpAccel;

	_float						m_fHurtTime = 0.f;

	_vec2						m_vTexUV = _vec2(0.f, 0.f);
public:
	static CMeshObj*			Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

public:
	virtual void Free(void);
	
};

#endif // MeshObj_h__
