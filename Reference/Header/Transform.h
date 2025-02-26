#ifndef Transform_h__
#define Transform_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform : public CComponent
{
private:
	explicit CTransform(void);
	virtual ~CTransform(void);

public:
	void		Get_Info(INFO eType, _vec3* pInfo);
	void		Move_Pos(const _vec3* const pDir)	 { m_vInfo[INFO_POS] += *pDir; }
	void		Rotation(ROTATION eType, const _float& fAngle)	{		*((_float*)&m_vAngle + eType) += fAngle; }
	void		Rotation(_vec3* pvRadian) { m_vAngle += *pvRadian; }

	void		Set_Radian(_vec3 vRadian) { m_vAngle = vRadian; }
	float		Get_Radian(ROTATION eType) { return *((_float*)&m_vAngle + eType); }
	_vec3		Get_Radian() { return m_vAngle; }

	void		Chase_Target(const _vec3* pTargetPos, const _float& fSpeed);
	const	_matrix* Compute_LookAtTarget(const _vec3* pTargetPos);

	void		Set_Scale(_vec3 vScale) { m_vScale = vScale; }
	void		Set_Scale(const _float& fX, const _float& fY, const _float& fZ)
	{
		m_vScale.x = fX;
		m_vScale.y = fY;
		m_vScale.z = fZ;
	}
	const _vec3	Get_Scale(void) const
	{
		return m_vScale;
	}

	void		Set_Pos(_vec3 vPos) { m_vInfo[INFO_POS] = vPos; }
	void		Set_Pos(const _float& fX, const _float& fY, const _float& fZ)
	{
		m_vInfo[INFO_POS].x = fX;
		m_vInfo[INFO_POS].y = fY;
		m_vInfo[INFO_POS].z = fZ;
	}
	const _vec3	Get_Pos(void) const
	{
		return m_vInfo[INFO_POS];
	}

	void		Get_WorldMatrix(_matrix* pWorldMatrix) const 
	{ 
		*pWorldMatrix = m_matWorld;  
	}

	void		Set_BillBoardMatrix(const _matrix* pMatrix)
	{
		m_matWorld = *pMatrix;
	}

	//void		Get_WorldMatrix_AABB(_matrix* pWorldMatrix)
	//{
	//	Get_WorldMatrix(pWorldMatrix);
	//	auto pRight = ((_vec3*)&pWorldMatrix->m[0]);
	//	auto pUp = ((_vec3*)&pWorldMatrix->m[1]);
	//	auto pLook = ((_vec3*)&pWorldMatrix->m[2]);
	//	auto fSizeX = D3DXVec3Length(pRight);
	//	auto fSizeY = D3DXVec3Length(pUp);
	//	auto fSizeZ = D3DXVec3Length(pLook);
	//	*pRight = _vec3(fSizeX, 0.f, 0.f);
	//	*pUp = _vec3(0.f, fSizeY, 0.f);
	//	*pLook = _vec3(0.f, 0.f, fSizeX);
	//}

	void		Get_NRotWorldMatrix(_matrix* pWorldMatrix) const
	{
		*pWorldMatrix = m_matNRotWorld;
	}

	void		Set_ParentMatrix(const _matrix* pParentMatrix)
	{
		m_matWorld *= *pParentMatrix;
	}

	const _matrix*	Get_WorldMatrixPointer(void) const { return &m_matWorld; }

public:
	HRESULT		Ready_Transform(void);
	virtual void Update_Component(const _float& fTimeDelta);
	

public:
	_vec3		m_vInfo[INFO_END];
	_vec3		m_vScale;
	_vec3		m_vAngle;
	_matrix		m_matWorld;

	_matrix		m_matNRotWorld;

	_vec3		m_vInterval = _vec3(0.f, 0.f, 0.f);

public:
	static CTransform*		Create(void);

private:
	virtual void	Free(void);

};

END

#endif // Transform_h__
