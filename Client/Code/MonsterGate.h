#ifndef MonsterGate_h__
#define MonsterGate_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CObjectInfo;
class CTransform;
class CRenderer;
class CCalculator;
class CStaticMesh;
class CCollider;

class CRcTex;
class CTexture;
class CTransform;
class CShader;

END

class CMonsterGate : public Engine::CGameObject
{
public:
	struct MONSTERGATE_DESC
	{
		int iCreateType = 0;
	};

protected:
	explicit CMonsterGate(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonsterGate(void);

public:
	virtual HRESULT Ready_Object(const _uint& iFlag);
	virtual void	LateReady_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;
public:
	virtual void	BeCollision(_ulong dwUsingCollision, Engine::CGameObject* pGameObject);

	void			Start(bool bStart) { m_bStart = bStart; }
	HRESULT			Ready_Level(int iLevel);
	_bool			Is_Create(float fTimeDelta);
	HRESULT			Create_Monster(float fTimeDelta);

	_int			Get_MonsterMax() { return m_iMonsterMax; }
	_int			Get_MonsterCount() { return m_lstCreateList.size(); }
protected:
	HRESULT			Add_Component(void);
	virtual HRESULT	SetUp_ConstantTable(LPD3DXEFFECT pEffect, Engine::CTexture* pTexture, int iTextureIndex = 0);
	void			BillBoard(void);

protected:
	Engine::CObjectInfo*		m_pObjectInfo = nullptr;
	Engine::CRcTex*				m_pBufferCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CCalculator*		m_pCalculatorCom = nullptr;
	Engine::CCollider*			m_pColliderCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;

	_float						m_fFrame = 0.f;
	_vec3						m_vDir;
	_float						m_fSpeed = 5.f;
	_uint						m_iFlag;

	_int						m_iLevel;
	_int						m_iCreateCount;
	_float						m_fCreateTime;
	_float						m_fCreateTimeMax;

	_int						m_iMonsterMax = 0;

	bool						m_bStart = false;

	list<MONSTERGATE_DESC>		m_lstCreateList;
public:
	static CMonsterGate*			Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint& iFlag);

public:
	virtual void Free(void);
};

#endif // MonsterGate_h__
