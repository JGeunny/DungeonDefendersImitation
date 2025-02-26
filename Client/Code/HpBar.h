#ifndef HpBar_h__
#define HpBar_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;
class CShader;

END

class CHpBar : public Engine::CGameObject
{
public:
	enum IMAGEID { IMAGE_BAR, IMAGE_CUR, IMAGE_END};
public:
	struct HPBAR_DESC {
		_vec2 vPos;
		_vec2 vSize;
		DWORD dwColor = D3DCOLOR_ARGB(155, 155, 255, 50);
		_float* pfCurHp = nullptr;
		_float* pfMaxHp = nullptr;
		std::wstring strContent = L"";
	};
private:
	explicit			CHpBar(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual				~CHpBar(void);

public:
	virtual HRESULT		Ready_Object(HPBAR_DESC tArg);
	virtual _int		Update_Object(const _float& fTimeDelta) override;
	virtual void		LateUpdate_Object(void) override;
	virtual void		Render_Object(void) override;

private:
	HRESULT				Add_Component(void);
	HRESULT				SetUp_ConstantTable(LPD3DXEFFECT pEffect, int iImageId);

private:
	Engine::CRcTex*		m_pBufferCom = nullptr;
	Engine::CTexture*	m_pTextureCom = nullptr;
	Engine::CTransform*	m_pTransCom = nullptr;
	Engine::CRenderer*	m_pRendererCom = nullptr;
	Engine::CShader*	m_pShaderCom = nullptr;

	_matrix				m_matProj;
	HPBAR_DESC			m_tInfo;
public:
	static CHpBar*		Create(LPDIRECT3DDEVICE9 pGraphicDev, HPBAR_DESC tArg);
private:
	virtual void Free(void) override;
};

#endif // HpBar_h__
