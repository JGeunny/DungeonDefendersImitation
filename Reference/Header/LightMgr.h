#ifndef LightMgr_h__
#define LightMgr_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "Light.h"
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CLightMgr : public CBase
{
	DECLARE_SINGLETON(CLightMgr)

private:
	explicit CLightMgr(void);
	virtual ~CLightMgr(void);

public:
	const D3DLIGHT9*		Get_LightInfo(const _uint& iIndex = 0)
	{
		if (m_VecLight.size() <= iIndex)
			return nullptr;
		return m_VecLight[iIndex]->Get_LightInfo();
	}

public:
	HRESULT	Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev,
		const D3DLIGHT9* pLightInfo,
		const _uint& iIndex);

	HRESULT Render_Light(CShader* pShader);

	void	Clear_Light();

private:
	vector<CLight*>		m_VecLight;

private:
	virtual void Free(void);

};
END
#endif // LightMgr_h__
