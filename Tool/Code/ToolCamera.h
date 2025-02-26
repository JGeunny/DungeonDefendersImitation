#ifndef ToolCamera_h__
#define ToolCamera_h__

#include "Defines.h"
#include "Camera.h"

BEGIN(Engine)

END

class CToolCamera : public Engine::CCamera
{
private:
	explicit CToolCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CToolCamera(void);

public:
	 HRESULT Ready_Object(const _vec3* pEye,
								const _vec3* pAt,
								const _vec3* pUp,
								const _float& fFovY,
								const _float& fAspect,
								const _float& fNear,
								const _float& fFar) ;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void LateUpdate_Object(void) override;
	virtual void Render_Object(void) override;

	void   Set_EnableControl(_bool bEnable_Control) { m_bEnable_Control = bEnable_Control; }
private:
	void	Check_InputDev(const _float fTimeDelta);
	void	Fix_Mouse(void);
	void	Move_Mouse(void);

private:
	_bool		m_bClick = false;
	_bool		m_bMouseFix = true;
	_float		m_fSpeed;
	_bool		m_bEnable_Control;

public:
	static CToolCamera*		Create(LPDIRECT3DDEVICE9 pGraphicDev, 
										const _vec3* pEye,
										const _vec3* pAt,
										const _vec3* pUp,
										const _float& fFovY, 
										const _float& fAspect, 
										const _float& fNear,
										const _float& fFar);

private:
	virtual void Free(void) override;

	

};

#endif // ToolCamera_h__
