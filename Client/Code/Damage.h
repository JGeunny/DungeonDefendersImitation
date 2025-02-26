#ifndef Damage_h__
#define Damage_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;
class CShader;

END

class CDamage : public Engine::CGameObject
{
private:
	explicit CDamage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDamage(void);

public:
	virtual HRESULT Ready_Object(Engine::CGameObject* pTarget, _float fDamage);
	virtual void	LateReady_Object(void);
	virtual _int	Update_Object(const _float& fTimeDelta) override;
	virtual void	LateUpdate_Object(void) override;
	virtual void	Render_Object(void) override;

	virtual HRESULT	SetUp_ConstantTable(LPD3DXEFFECT pEffect, Engine::CTexture* pTexture, int iTextureIndex, _matrix matWorld);
	void			BillBoard(void);
private:
	HRESULT			Add_Component(void);
	HRESULT			FollowTarget();

private:
	Engine::CRcTex*				m_pBufferCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;

	DWORD						m_dwColor;
	DWORD						m_dwColorBorder;
	
	float						m_fMoveY;
	_vec3						m_vDir;
	float						m_fJumpPower;
	float						m_fJumpAccel;
	float						m_fDamage;
	_vec3						m_vMovePos;
	_vec3						m_vTargetPos;

	_matrix						m_matProj;
public:
	static CDamage*			Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pTarget, _float fDamage);

private:
	virtual void Free(void) override;


};

#endif // Damage_h__
