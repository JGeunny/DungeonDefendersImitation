#ifndef MiniGame_h__
#define MiniGame_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CRenderer;
class CShader;

END

class CMiniGame : public Engine::CGameObject
{
public:
	enum COMBO_TYPE { COMBO_NONE, COMBO_PERPECT, COMBO_EXCELLENT, COMBO_MISS };
	enum EBUTTON_TYPE { BUTTON_RIGHT, BUTTON_UP, BUTTON_LEFT, BUTTON_DOWN, BUTTON_ENTER, BUTTON_END};
	struct MINIGAME_DEST
	{
		_float			fStartTime;
		EBUTTON_TYPE	eType;
		_vec2			vSize;
		_vec2			vPos;
	};
	struct COMBO_DEST
	{
		_int			iCombo;
		COMBO_TYPE		eCombo;
		_vec2			vPos;
		_vec2			vSize;
	};
private:
	explicit CMiniGame(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMiniGame(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object(void) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		Add_Component(void);
	void		Update_ComboInfo(const _float& fTimeDelta);
	void		Render_ComboInfo(void);
	HRESULT		Update_TargetInfo(void);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT pEffect, Engine::CTexture * pTexture, int iTextureIndex, _matrix matWorld);

public:
	void	Ready_Button(void);
	void	Add_Combo(COMBO_TYPE eCombo);

	_bool	Get_Clear() { 
		return (m_lstButton.size() == 0 &&
			m_lstRenderButton.size() == 0) ? true : false;
	}
	int		Get_Combo() { return m_iCombo; }
private:
	void	Update_Button(const _float& fTimeDelta);
	void	Render_Button();

	_bool	CheckPoint(float fPointX, float fButtonX, float fSize);

private:
	Engine::CRcTex*				m_pBufferCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CTexture*			m_pTextureCom_Number = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;

private:
	_matrix						m_matProj;
	_float						m_fX, m_fY;
	_float						m_fSizeX, m_fSizeY;

	_float						m_fPointX;

	_int						m_iCombo;

	_float						m_fPointSizeX = 0.f;
	_float						m_fPointSpeed = 50.f;

	_float						m_fButtonSpeed;
	_float						m_fAddTimeDelta;
	std::list<MINIGAME_DEST>	m_lstButton;
	std::list<MINIGAME_DEST>	m_lstRenderButton;

	std::list<COMBO_DEST>		m_lstComboInfo;
	DWORD						m_dwColor;
	DWORD						m_dwColorBorder;
public:
	static CMiniGame*			Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;
};

#endif // MiniGame_h__
