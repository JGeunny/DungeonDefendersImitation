#ifndef Terrain_h__
#define Terrain_h__

#include "GameObject.h"

BEGIN(Engine)

class CTerrainTex;
class CTexture;
class CTransform;
class CRenderer;

END

class CTerrain : public Engine::CGameObject
{
private:
	explicit				CTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual					~CTerrain(void);

public:
	virtual HRESULT			Ready_Object(void);
	virtual _int			Update_Object(const _float& _fTimeDelta);
	virtual	void			LateUpdate_Object(void);
	virtual void			Render_Object(void);

private:
	HRESULT					Add_Component(void);

private:
	Engine::CTerrainTex*	m_pBufferCom = nullptr;
	Engine::CTexture*		m_pTextureCom = nullptr;
	Engine::CTransform*		m_pTransCom = nullptr;
	Engine::CRenderer*		m_pRendererCom = nullptr;

public:
	static CTerrain*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;


};

#endif // Terrain_h__
