#ifndef Cell_h__
#define Cell_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "Line.h"
#include "Export_Utility.h"

BEGIN(Engine)
class CObjectInfo;
class CTriCol;

class ENGINE_DLL CCell : public CBase
{
public:
	enum POINT		{	POINT_A, POINT_B, POINT_C, POINT_END	};
	enum NEIGHBOR	{	NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };
	enum LINE		{	LINE_AB, LINE_BC, LINE_CA, LINE_END };
	enum COMPARE	{	COMPARE_MOVE, COMPARE_STOP };

public:
	const _vec3*		Get_Point(POINT eType) const		{ return &m_vPoint[eType]; }
	const _vec3*		Get_Points() const					{ return m_vPoint; }
	CCell*				Get_Neighbor(NEIGHBOR eType) const	{ return m_pNeighbor[eType]; }
	const _ulong*		Get_Index(void) const { return &m_dwIndex; }
	_vec3				Get_Pos();
	_vec3				Get_Up();
public:
	void				Set_Neighbor(NEIGHBOR eType, CCell* pNeighbor)	{		m_pNeighbor[eType] = pNeighbor;	}


public:
	HRESULT			Ready_Cell(const _ulong& dwIndex,
								const _vec3* pPointA,
								const _vec3* pPointB, 
								const _vec3* pPointC);
	
	_bool			Compare_Point(const _vec3* pFirstPoint, 
								const _vec3* pSecondPoint, 
								CCell* pCell);

	COMPARE			Compare(const _vec3* pEndPos, _ulong* pCellIndex);
	CLine::COMPARE	Compare_Line(LINE eLine, const _vec3* pEndPos);


	void			Render_Cell(const _matrix* pmatWorld = nullptr);

	COLLTYPE		Get_CollType() { return m_eCollType; }
	void			Set_CollType(COLLTYPE eCollType) { m_eCollType = eCollType; }
public:

	_vec3				m_vPoint[POINT_END];
	DWORD				m_dwColor;
	Engine::CTriCol*	m_pTriCol;
	Engine::CCollider*	m_pColliderCom_Point[POINT_END];
	CObjectInfo*		m_pObjectInfo = nullptr;
private:
	COLLTYPE			m_eCollType;
	CCell*				m_pNeighbor[NEIGHBOR_END];
	CLine*				m_pLine[LINE_END];
	_ulong				m_dwIndex = 0;

	LPD3DXLINE			m_pD3DXLine = nullptr;
	LPDIRECT3DDEVICE9	m_pGraphicDev = nullptr;

private:
	explicit CCell(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCell(void);

public:
	static CCell*		Create(LPDIRECT3DDEVICE9 pGraphicDev, 
								const _ulong& dwIndex,
								const _vec3* pPointA,
								const _vec3* pPointB,
								const _vec3* pPointC);
	virtual void		Free(void);

};
END
#endif // Cell_h__
