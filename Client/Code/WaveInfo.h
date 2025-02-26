#ifndef WaveInfo_h__
#define WaveInfo_h__

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

class CWaveInfo : public Engine::CGameObject
{
public:
	enum WAVEINFO_TYPE {
		WAVEINFO_WAVE1, WAVEINFO_WAVE2, WAVEINFO_WAVE3,
		WAVEINFO_COMPLETE, WAVEINFO_BUILD, WAVEINFO_COMBAT, 
		WAVEINFO_VICTORY, WAVEINFO_COMEBACKHOME, WAVEINFO_NONE
	};

public:
	struct WAVEINFO_DESC
	{
		_vec2		 vBeginPos = _vec2(0.f, 0.f);
		_vec2		 vSize = _vec2(600.f, 200.f);
		float		 fSpeed = 150.f;
		std::wstring strImageKey = L"Texture_WaveInfo";		
	};
protected:
	explicit				CWaveInfo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual					~CWaveInfo(void);

public:
	virtual HRESULT			Ready_Object(WAVEINFO_DESC* pArg = nullptr);
	virtual	void			LateReady_Object(void);
	virtual _int			Update_Object(const _float& fTimeDelta);
	virtual void			LateUpdate_Object(void) override;
	virtual void			Render_Object(void) override;

	virtual void			Ready_UI(int iLevel, bool bBuild);
	//virtual Engine::CTransform*	Get_Transform() { return m_pTransCom; }

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
	WAVEINFO_DESC			m_tWaveInfo;
	_vec2					m_vPos = _vec2(0.f, 0.f);
	_vec2					m_vSize = _vec2(600.f, 200.f);
	float					m_fDelayTime = 0.f;

	_float					m_fIntervalY = -130.f;

	WAVEINFO_TYPE			m_eCurUI = WAVEINFO_NONE;
	std::vector<WAVEINFO_TYPE> m_vecUI;

public:
	static CWaveInfo*		Create(LPDIRECT3DDEVICE9 pGraphicDev, WAVEINFO_DESC* pArg = nullptr);

private:
	virtual void Free(void) override;


};

#endif // WaveInfo_h__
