#ifndef Effect_h__
#define Effect_h__

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

class CEffect : public Engine::CGameObject
{
public:
	enum EFFECT_TYPE { EFFECT_NONE, EFFECT_ONCE, EFFECT_REPEAT };
	
public:
	struct EFFECT_DESC
	{
		EFFECT_TYPE  eType = EFFECT_NONE;
		float		 fAnimationSpeed = 15.f;
		std::wstring strImageKey = L"Texture_Default";
		bool		 bNoTargetDead = false;
		_vec3		 vIntervalPos = _vec3(0.f, 0.f, 0.f);
		DWORD		 dwColor = D3DCOLOR_ARGB(0, 0, 0, 0);

		bool		 bParabora = false;
		_vec3		 vDir = _vec3(0.f, 0.f, 0.f);
		float		 fJumpPower = 3.f;
	};

protected:
	explicit				CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual					~CEffect(void);

public:
	virtual HRESULT			Ready_Object(EFFECT_DESC* pArg = nullptr);
	virtual	void			LateReady_Object(void);
	virtual _int			Update_Object(const _float& fTimeDelta);
	virtual void			LateUpdate_Object(void) override;
	virtual void			Render_Object(void) override;

	virtual bool			Update_AnimationFrame(float fTimeDelta);
	virtual bool			Update_Effect(float fTimeDelta);

	virtual Engine::CTransform*	Get_Transform() { return m_pTransCom; }
public:
	void					Set_EffectType(EFFECT_TYPE eType) { m_eType = eType; }
	virtual	HRESULT			Ready_Shader(Engine::CShader* pShader);
private:
	HRESULT					Add_Component(void);
	void					BillBoard(void);

	HRESULT					Update_Parabola(float fTimeDelta);

protected:
	Engine::CRcTex*			m_pBufferCom = nullptr;
	Engine::CTexture*		m_pTextureCom = nullptr;
	Engine::CTextureAdmin*	m_pTextureAdminCom = nullptr;
	Engine::CTransform*		m_pTransCom = nullptr;
	Engine::CRenderer*		m_pRendererCom = nullptr;
	Engine::CShader*		m_pShaderCom = nullptr;

protected:
	float					m_fCurrentImageIndex = 0.f;
	float					m_fAnimationSpeed = 15.f;
	EFFECT_TYPE				m_eType = EFFECT_NONE;
	std::wstring			m_strImageKey = L"Texture_Default";
	bool					m_bNoTargetDead = false;
	_vec3					m_vIntervalPos = _vec3(0.f, 0.f, 0.f);
	DWORD					m_dwColor = D3DCOLOR_ARGB(0, 0, 0, 0);

	// Æ÷¹°¼±
	bool					m_bParabora = false;
	float					m_fMoveY = 0.f;
	_vec3					m_vDir = _vec3(0.f, 0.f, 0.f);
	_vec3					m_vMovePos = _vec3(0.f, 0.f, 0.f);
	_vec3					m_vBeginPos = _vec3(0.f, 0.f, 0.f);
	float					m_fJumpPower = 3.f;
	float					m_fJumpAccel = 0.f;
	
public:
	static CEffect*			Create(LPDIRECT3DDEVICE9 pGraphicDev, EFFECT_DESC* pArg = nullptr);

private:
	virtual void Free(void) override;


};

#endif // Effect_h__
