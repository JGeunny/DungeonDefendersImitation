#ifndef Logo_h__
#define Logo_h__

#include "Defines.h"
#include "Scene.h"

#include "BackGround.h"
#include "Loading.h"

class CTitleUI;
class CHpBar;

class CLogo : public Engine::CScene
{
private:
	explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLogo(void);

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void LateUpdate_Scene(void) override;
	virtual void Render_Scene(void) override;

private:
	//HRESULT		Ready_EnvironmentLayer(const _tchar* pLayerTag);
	HRESULT		Ready_GameLogicLayer(const _tchar* pLayerTag);
	//HRESULT		Ready_UILayer(const _tchar* pLayerTag);
	HRESULT		Ready_Resource(LPDIRECT3DDEVICE9 pGraphicDev, RESOURCEID eMax);

private:
	float				m_fCurLoading = 1.f;
	float				m_fMaxLoading = 1.f;
	CLoading*			m_pLoading = nullptr;
	CTitleUI*			m_pTitleUI = nullptr;
	CHpBar*				m_pHpBar = nullptr;

public:
	static CLogo*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

#endif // Logo_h__
