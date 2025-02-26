#ifndef UI_h__
#define UI_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;

END

class CUI : public Engine::CGameObject
{
private:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object(void) override;
	virtual void Render_Object(void) override;
	void		Set_Combat(bool bCombat) { m_bCombat = bCombat; }
private:
	HRESULT		Add_Component(void);
	void		Render_GoldInfo(void);
	void		Render_StateInfo(void);
	//void		Render_AminingPoint(void);
	HRESULT		Update_TargetInfo(void);

private:
	Engine::CRcTex*				m_pBufferCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;

private:
	_matrix						m_matProj;
	_float						m_fX, m_fY;
	_float						m_fSizeX, m_fSizeY;
	_int						m_iCurGold;
	bool						m_bCombat = false;

	_float						m_fAminingAngle = 0.f;
	
public:
	static CUI*			Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;


};

#endif // UI_h__
