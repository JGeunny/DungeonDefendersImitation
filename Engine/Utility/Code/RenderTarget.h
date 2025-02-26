#ifndef RenderTarget_h__
#define RenderTarget_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "Component.h"
#include "Optimization.h"

BEGIN(Engine)

class CVIBuffer_ViewPort;
class CRenderTarget final : public CBase
{
private:
	explicit CRenderTarget(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CRenderTarget() = default;
public:
	LPDIRECT3DTEXTURE9 Get_TargetTexture() const {
		return m_pTexture; }
public:
	HRESULT Ready_RenderTarget(_uint iWidth, _uint iHeight, D3DFORMAT Format, D3DXCOLOR ClearColor);
	HRESULT SetUp_OnGraphicDev(_uint iIndex);
	HRESULT Release_OnGraphicDev(_uint iIndex);
	HRESULT Clear_RenderTarget();
public:
	HRESULT Ready_Debug_Buffer(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug_Buffer();
private:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = nullptr;
	LPDIRECT3DTEXTURE9			m_pTexture = nullptr;
	LPDIRECT3DSURFACE9			m_pSurface = nullptr;
	LPDIRECT3DSURFACE9			m_pOldSurface = nullptr;
	D3DXCOLOR					m_ClearColor;
private:
	CVIBuffer_ViewPort*			m_pVIBuffer = nullptr;
public:
	static CRenderTarget* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iWidth, _uint iHeight, D3DFORMAT Format, D3DXCOLOR ClearColor);
	virtual void Free();
};

END

#endif // RenderTarget_h__