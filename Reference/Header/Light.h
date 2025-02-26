#ifndef Light_h__
#define Light_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "Shader.h"
#include "VIBuffer_ViewPort.h"

BEGIN(Engine)

class ENGINE_DLL CLight : public CBase
{
private:
	explicit CLight(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLight(void);

public:
	const D3DLIGHT9*		Get_LightInfo(void) const { return &m_tLightInfo; }

public:
	HRESULT	Ready_Light(const D3DLIGHT9* pLightInfo, const _uint& iIndex);
	HRESULT Render_Light(CShader* pShader);

private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	D3DLIGHT9			m_tLightInfo;
	_uint				m_iIndex;
	CVIBuffer_ViewPort*	m_pVIBuffer = nullptr;

public:
	static CLight*	Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const D3DLIGHT9* pLightInfo,
		const _uint& iIndex);
private:
	virtual void Free(void);

};

END
#endif // Light_h__
