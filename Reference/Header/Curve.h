#ifndef Curve_h__
#define Curve_h__

#include "Engine_Defines.h"
#include "BasePrototype.h"
#include "Line.h"
#include "Export_Utility.h"

BEGIN(Engine)

class ENGINE_DLL CCurve : public CBasePrototype
{
private:
	explicit CCurve(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCurve(const CCurve& rhs);
	virtual ~CCurve(void);

public:
	virtual unsigned long Release(void);

public:
	enum POINT		{ POINT_A, POINT_B, POINT_C, POINT_D, POINT_END	};


public:
	HRESULT			Ready_Curve(Engine::CAMERATYPE eType,
								const _vec3* pPoint);

	void			Render_Curve(const _matrix* pmatWorld = nullptr);

public:
	const _vec3		Get_Point() const { return m_vPoint; }
	const _vec3		Get_Point(POINT eType) const { return (nullptr == m_pvPoint[eType]) ? m_vPoint : *m_pvPoint[eType]; }
	void			Set_Point(_vec3 vPoint) { m_vPoint = vPoint; }
	
	COLLTYPE		Get_CollType() { return m_eCollType; }
	void			Set_CollType(COLLTYPE eCollType) { m_eCollType = eCollType; }

	const _float	Get_Speed() const { return m_fSpeed; }
	void			Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; }
	Engine::CCollider* Get_Collider() { return m_pColliderCom; }

	bool			IsClone() { return m_bClone; }
public:
	HRESULT			Create_Collider(Engine::CAMERATYPE eType);
private:
	_float				m_fSpeed;
	_vec3				m_vPoint;
	_vec3*				m_pvPoint[POINT_END];
	Engine::CCollider*	m_pColliderCom;
private:
	Engine::CAMERATYPE	m_eCurveType;
	DWORD				m_dwColor;
	COLLTYPE			m_eCollType;
private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	LPD3DXLINE			m_pD3DXLine;
	bool				m_bClone;

public:
	virtual CCurve*		Clone(_vec3 vPoint = _vec3(0.f, 0.f, 0.f));
	static CCurve*		Create(LPDIRECT3DDEVICE9 pGraphicDev, 
								Engine::CAMERATYPE eType,
								const _vec3* pPoint);
	virtual void		Free(void);
};
END
#endif // Curve_h__
