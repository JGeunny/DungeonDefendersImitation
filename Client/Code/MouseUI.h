#ifndef MouseUI_h__
#define MouseUI_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTextureAdmin;
class CTransform;
class CRenderer;
class CShader;

END

class CMouseUI : public Engine::CGameObject
{
public:
	enum MOUSE_TYPE {
		MOUSE_BASE, MOUSE_AIM
	};

public:
	struct MOUSEUI_DESC
	{
		MOUSE_TYPE	 eCurType;
		std::wstring strImageKey = L"Texture_MouseUI";		
	};
protected:
	explicit				CMouseUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual					~CMouseUI(void);

public:
	virtual HRESULT			Ready_Object(MOUSEUI_DESC* pArg = nullptr);
	virtual	void			LateReady_Object(void);
	virtual _int			Update_Object(const _float& fTimeDelta);
	virtual void			LateUpdate_Object(void) override;
	virtual void			Render_Object(void) override;

	virtual void			Update_MouseUI(const _float& fTimeDelta, MOUSE_TYPE eType);
	void					Set_MouseType(MOUSE_TYPE eType) { m_tMouseUI.eCurType = eType; }

public:
	virtual	HRESULT			Ready_Shader(Engine::CShader* pShader);
private:
	HRESULT					Add_Component(void);
	void					Ortho(void);

protected:
	Engine::CRcTex*			m_pBufferCom = nullptr;
	Engine::CTexture*		m_pTextureCom = nullptr;
	Engine::CTextureAdmin*	m_pTextureAdminCom = nullptr;
	Engine::CTransform*		m_pTransCom = nullptr;
	Engine::CRenderer*		m_pRendererCom = nullptr;
	Engine::CShader*		m_pShaderCom = nullptr;

protected:
	_matrix					m_matProj;
	MOUSEUI_DESC			m_tMouseUI;
	_vec2					m_vPos = _vec2(0.f, 0.f);
	_vec2					m_vSize = _vec2(80.f, 80.f);
	_float					m_fAimAngle = 0.f;

public:
	static CMouseUI*		Create(LPDIRECT3DDEVICE9 pGraphicDev, MOUSEUI_DESC* pArg = nullptr);

private:
	virtual void Free(void) override;


};

#endif // MouseUI_h__
