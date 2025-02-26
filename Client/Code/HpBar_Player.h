#ifndef HpBar_Player_h__
#define HpBar_Player_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;
class CShader;

END

class CHpBar_Player : public Engine::CGameObject
{
public:
	struct SKILL_DESC
	{
		_vec2	vPos;
		_vec2	vSize;
		float	fCurDelayTime = 0.f;
		float	fDelayTimeSpeed = 1.f;
		bool	bActivate = true;
	};
private:
	explicit CHpBar_Player(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CHpBar_Player(void);

public:
	virtual HRESULT Ready_Object(Engine::CGameObject* pTarget);
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object(void) override;
	virtual void Render_Object(void) override;

	void		Update_SkillInfo(float fTimeDelta);
	void		Ready_SkillInfo();

	void		Set_DelayTimeKey(DWORD* pdwKey);

	float		Get_CurMonsterHp() { return m_fCurMonsterHp; }

	void		Render_MonsterHpInfo(void);
private:
	HRESULT		Add_Component(void);
	HRESULT		FollowTarget();
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT pEffect, int iHpBar = 0);
	HRESULT		SetUp_ConstantTable_Skill(LPD3DXEFFECT pEffect, int index = 0);
	HRESULT		SetUp_ConstantTable_EnemyHpBar(LPD3DXEFFECT pEffect, int index = 0);

private:
	Engine::CRcTex*				m_pBufferCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CTexture*			m_pTextureCom_Skill = nullptr;
	Engine::CTexture*			m_pTextureCom_EnemyHpBar = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;

	int							m_iCurrentImageCount;
	float						m_fTimeDelta;
	float						m_fCurHp;
	float						m_fCurMp;
	float						m_fCurExp;

	int							m_iMonsterCount = 0;
	int							m_iMonsterMax = 0;
	float						m_fCurMonsterHp;
	_vec4						m_vColorHp;
	int							m_iCurGold;

	_matrix						m_matProj;
	_float						m_fX, m_fY;
	_float						m_fSizeX, m_fSizeY;

	std::vector<SKILL_DESC>		m_vecSkillInfo;
public:
	static CHpBar_Player*		Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pTarget);

private:
	virtual void Free(void) override;

	void BillBoard(void);


};

#endif // HpBar_Player_h__
