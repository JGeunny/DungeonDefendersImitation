#ifndef TitleUI_h__
#define TitleUI_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;

END

class CTitleUI : public Engine::CGameObject
{
public:
	enum UIID { UI_TITLE, UI_GAMESTART, UI_GAMEEND, UI_END };
	struct UI_DESC
	{
		_vec2	vPos;
		_vec2	vSize;
		_bool	m_bMouseOver = false;
		_bool	m_bMouseClick = false;
		_int	iTextureIndex;
	};
private:
	explicit CTitleUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTitleUI(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object(void) override;
	virtual void Render_Object(void) override;

	HRESULT		Ready_UI(void);
private:
	HRESULT		Add_Component(void);
	HRESULT		Update_UI(UI_DESC& tUIInfo);
	HRESULT		Render_UI(UI_DESC& tUIInfo);

	HRESULT		Update_Bounce(float fTimeDelta);
	
public:
	_bool		IsGameStart() { return m_bGameStart; }
	_bool		IsGameEnd() { return m_bGameEnd; }

private:
	Engine::CRcTex*				m_pBufferCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;

private:
	_matrix						m_matProj;
	vector<UI_DESC>				m_vecUI;
	_vec2						m_vPos;
	_float						m_fJumpAccel = 0.f;
	_float						m_fBouncePower = 0.f;
	_int						m_iBounceCount = 0;
	_bool						m_bGameStart = false;
	_bool						m_bGameEnd = false;

public:
	static CTitleUI*			Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;


};

#endif // TitleUI_h__
