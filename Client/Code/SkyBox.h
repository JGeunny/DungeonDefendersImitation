#ifndef SkyBox_h__
#define SkyBox_h__

#include "Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CCubeTex;
class CTexture;
class CTransform;
class CRenderer;

END

class CSkyBox : public Engine::CGameObject
{
private:
	explicit CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSkyBox(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object(void) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		Add_Component(void);

private:
	Engine::CCubeTex*			m_pBufferCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CTransform*			m_pTransCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	
public:
	static CSkyBox*			Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;


};

#endif // SkyBox_h__
