#ifndef VIBuffer_ViewPort_h__
#define VIBuffer_ViewPort_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_ViewPort final : public CVIBuffer
{
private:
	explicit CVIBuffer_ViewPort(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CVIBuffer_ViewPort(const CVIBuffer_ViewPort& rhs);
	virtual ~CVIBuffer_ViewPort() = default;
public:
	virtual HRESULT Ready_Component_Prototype(_float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual HRESULT Ready_Component(void* pArg);
	virtual HRESULT Render_Buffer(void);
public:
	static CVIBuffer_ViewPort* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _float fX, _float fY, _float fSizeX, _float fSizeY);
	virtual Engine::CResources* Clone(void);
	virtual void Free();
};

END

#endif	//VIBuffer_ViewPort_h__