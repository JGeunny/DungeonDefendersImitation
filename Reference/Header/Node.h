#ifndef Node_h__
#define Node_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "Line.h"
#include "Export_Utility.h"

BEGIN(Engine)
class CObjectInfo;
class CTriCol;

class ENGINE_DLL CNode : public CBase
{
public:
	enum POINT		{ POINT_A, POINT_B, POINT_C, POINT_D, POINT_END	};

public:
	const _vec3*		Get_Point(POINT eType) const		{ return &m_vPoint[eType]; }

public:
	HRESULT			Ready_Node(Engine::CAMERATYPE eType,
								const _vec3* pPointB,
								const _vec3* pPointC);

	void			Render_Node(const _matrix* pmatWorld = nullptr);

	COLLTYPE		Get_CollType() { return m_eCollType; }
	void			Set_CollType(COLLTYPE eCollType) { m_eCollType = eCollType; }
public:
	_vec3				m_vPoint[POINT_END];
	Engine::CCollider*	m_pColliderCom_Point[POINT_END];
	CObjectInfo*		m_pObjectInfo = nullptr;
	
private:
	Engine::CAMERATYPE	m_eNodeType;
	DWORD				m_dwColor;
	COLLTYPE			m_eCollType;
	LPDIRECT3DDEVICE9	m_pGraphicDev = nullptr;
	LPD3DXLINE			m_pD3DXLine = nullptr;

private:
	explicit CNode(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CNode(void);

public:
	static CNode*		Create(LPDIRECT3DDEVICE9 pGraphicDev, 
								Engine::CAMERATYPE eType,
								const _vec3* pPointB,
								const _vec3* pPointC);
	virtual void		Free(void);

};
END
#endif // Node_h__
