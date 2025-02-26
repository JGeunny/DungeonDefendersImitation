#ifndef Ending_h__
#define Ending_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;

END

class CEnding : public Engine::CGameObject
{
public:
	struct UI_DESC
	{
		_vec2	vPos = _vec2(WINCX >> 1, 0.f);
		_vec2	vSize = _vec2(WINCX, WINCY);
	};
private:
	explicit CEnding(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEnding(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object(void) override;
	virtual void Render_Object(void) override;
private:
	HRESULT		Add_Component(void);
	HRESULT		Render_UI(UI_DESC& tUIInfo);

	HRESULT		Update_Bounce(float fTimeDelta);
	
private:
	Engine::CRcTex*				m_pBufferCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;

private:
	_matrix						m_matProj;
	UI_DESC						m_tUIInfo;
	_float						m_fJumpAccel = 0.f;
	_float						m_fBouncePower = 0.f;
	_int						m_iBounceCount = 0;
	_bool						m_bLateStart = false;

public:
	static CEnding*			Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;


};

#endif // Ending_h__
