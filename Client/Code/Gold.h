#ifndef Gold_h__
#define Gold_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;
class CShader;

class CMeshAdmin;
class CMesh;
class CCollider;
END

class CGold : public Engine::CGameObject
{
private:
	explicit CGold(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGold(void);

public:
	virtual HRESULT Ready_Object(Engine::CGameObject* pTarget);
	virtual void	LateReady_Object(void);
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object(void) override;
	virtual void Render_Object(void) override;

public:
	void			Movement(const _float& fTimeDelta);

private:
	HRESULT		Add_Component(void);
	HRESULT		FollowTarget();
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT pEffect);

private:
	Engine::CMeshAdmin*			m_pMeshAdminCom = nullptr;
	Engine::CMesh*				m_pMeshCom = nullptr;
	Engine::CCollider*			m_pColliderCom = nullptr;
	//Engine::CRcTex*			m_pBufferCom = nullptr;
	//Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;

	int							m_iCurrentImageCount;
	float						m_fTimeDelta;
	float						m_fCurHp;
	_vec4						m_vColorHp;
	float						m_fSpeed = 4.f;

	bool						m_bFollowPlayer = false;
	_vec3						m_vBeginPos;
public:
	static CGold*			Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pTarget);

private:
	virtual void Free(void) override;

	void BillBoard(void);


};

#endif // Gold_h__
