#ifndef HpBar_B_h__
#define HpBar_B_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;
class CShader;

END

class CHpBar_B : public Engine::CGameObject
{
private:
	explicit CHpBar_B(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CHpBar_B(void);

public:
	virtual HRESULT Ready_Object(Engine::CGameObject* pTarget);
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object(void) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		Add_Component(void);
	HRESULT		FollowTarget();
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	void		BillBoard(void);
private:
	Engine::CRcTex*				m_pBufferCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;

	int							m_iCurrentImageCount;
	float						m_fTimeDelta;
	float						m_fCurHp;
	_vec4						m_vColorHp;
public:
	static CHpBar_B*			Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pTarget);

private:
	virtual void Free(void) override;
};

#endif // HpBar_B_h__
