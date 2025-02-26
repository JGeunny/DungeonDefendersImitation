#ifndef ShoulderCamera_h__
#define ShoulderCamera_h__

#include "Defines.h"
#include "Camera.h"

BEGIN(Engine)

END

class CShoulderCamera : public Engine::CCamera
{
private:
	explicit CShoulderCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CShoulderCamera(void);

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

public:
	void	Set_Mode(Engine::CONTROLTYPE eMode) { m_eMode = eMode; }
	Engine::CONTROLTYPE	Get_Mode() { return m_eMode; }

	void	DoShack(bool bGayScale = true);
public:
	_bool	Change_Mode(Engine::CONTROLTYPE eMode);

private:
	void	Shacking(float fTimeDelta);
	void	Check_InputDev(const _float fTimeDelta);
	void	Fix_Mouse(void);
	void	Move_Mouse(void);
	void	FollowObject(void);
	void	TopView(void);

	_vec3	m_vShacking = _vec3(0.f, 0.f, 0.f);
	_vec3	m_vDir_Shacking = _vec3(0.f, 1.f, 0.f);
	float	m_fSpeed = 1.f;
	bool	m_bShacking = false;
	int		m_iCount_Shacking = 0;
	
private:
	Engine::CONTROLTYPE	m_eMode = Engine::CONTROLTYPE_BASE;
	_bool			m_bMouseFix = true;
	_float			m_fDist_TopView = 0.f;
	_vec3			m_vLook_TopView = _vec3(0.f, 0.f, 1.f);
	
public:
	static CShoulderCamera*		Create(LPDIRECT3DDEVICE9 pGraphicDev,
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

#endif // ShoulderCamera_h__
