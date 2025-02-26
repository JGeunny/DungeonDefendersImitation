#include "stdafx.h"
#include "ShoulderCamera.h"

#include "Export_Function.h"

#include "Player.h"

CShoulderCamera::CShoulderCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
{

}

CShoulderCamera::~CShoulderCamera(void)
{

}

HRESULT CShoulderCamera::Ready_Object(const _vec3* pEye,
									const _vec3* pAt,
									const _vec3* pUp,
									const _float& fFovY,
									const _float& fAspect,
									const _float& fNear,
									const _float& fFar)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	m_fFovY = fFovY;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	FAILED_CHECK_RETURN(Engine::CCamera::Ready_Object(), E_FAIL);

	return S_OK;
}

_int CShoulderCamera::Update_Object(const _float& fTimeDelta)
{
	if (false == m_bActivate) return EVENT_DEFAULT;
	//auto pTarget = Get_Target();
	//if (nullptr == pTarget)
	//	return 0;
	//auto pPlayer = (CPlayer*)pTarget;
	
	//if (Engine::KeyDown(KEY_Q) || Engine::KeyDown(KEY_Q))
	//{
	//	m_eMode = (Engine::CONTROLTYPE_BASE == m_eMode) ? Engine::CONTROLTYPE_BUILD : Engine::CONTROLTYPE_BASE;

	//	m_eMode = (Engine::CONTROLTYPE_BASE == m_eMode) ? Engine::CONTROLTYPE_FISHING : Engine::CONTROLTYPE_BASE;
	//	if (m_eMode == Engine::CONTROLTYPE_BUILD || m_eMode == Engine::CONTROLTYPE_FISHING)
	//	{
	//		m_fDist_TopView = 0.f;
	//		if (nullptr != pTarget)
	//		{
	//			auto pTrans_Target = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
	//			pTrans_Target->Get_Info(Engine::INFO::INFO_LOOK, &m_vLook_TopView);
	//			D3DXVec3Normalize(&m_vLook_TopView, &m_vLook_TopView);
	//		}
	//	}
	//}
	
	switch (m_eMode)
	{
	case Engine::CONTROLTYPE_BASE:
		FollowObject();
		break;
	case Engine::CONTROLTYPE_BUILD: 
	case Engine::CONTROLTYPE_FISHING:
		TopView();
		break;
	default:
		break;
	}
	Shacking(fTimeDelta);
	_int iExit = Engine::CCamera::Update_Object(fTimeDelta);
	Engine::Get_SoundMgr()->Update(m_vEye);
	return iExit;
}

void CShoulderCamera::LateUpdate_Object(void)
{
	if (false == m_bActivate) return;
}

void CShoulderCamera::Render_Object(void)
{
	if (false == m_bActivate) return;
}

void CShoulderCamera::Free(void)
{
	Engine::CCamera::Free();
}

void CShoulderCamera::Check_InputDev(const _float fTimeDelta)
{
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, 0, &m_matView);

	if (Engine::KeyPressing(KEY_W))
	{
		_vec3	vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vLook, &vLook) * 8.f * fTimeDelta;

		m_vEye += vLength;
		m_vAt  += vLength;
	}

	if (Engine::KeyPressing(KEY_S))
	{
		_vec3	vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vLook, &vLook) * 8.f * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Engine::KeyPressing(KEY_A))
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vRight, &vRight) * 8.f * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Engine::KeyPressing(KEY_D))
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vRight, &vRight) * 8.f * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}
}

void CShoulderCamera::Fix_Mouse(void)
{
	POINT	ptMouse{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CShoulderCamera::Move_Mouse(void)
{
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, 0, &m_matView);
	_long	dwMouse = 0;

	if (dwMouse = Engine::Get_DIMouseMove(Engine::DIMS_Y))
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLook = m_vAt - m_vEye;
		D3DXVec3Normalize(&vLook, &vLook);

		_matrix matRot;
		D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouse / 10.f));

		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;
	}

	if (dwMouse = Engine::Get_DIMouseMove(Engine::DIMS_X))
	{
		_vec3 vUp = _vec3(0.f, 1.f, 0.f);

		_vec3	vLook = m_vAt - m_vEye;
		D3DXVec3Normalize(&vLook, &vLook);

		_matrix matRot;
		D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouse / 10.f));

		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;
	}
}

void CShoulderCamera::FollowObject(void)
{
	auto pTarget = Get_Target();
	if (nullptr == pTarget) return;
	auto pTrans_Target = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC);

	//카메라의 At과 Eye를 어깨에서 바라보게 끔 좌표 셋팅! 
	_vec3 vLook, vPos, vInterval;
	pTrans_Target->Get_Info(Engine::INFO::INFO_POS, &vPos);
	pTrans_Target->Get_Info(Engine::INFO::INFO_LOOK, &vLook);
	D3DXVec3Normalize(&vLook, &vLook);

	float fDist = 20.f;
	vInterval = {1.f, 2.5f, -5.f};
	//vInterval = { 1.f, 5.f, -20.f };
	vInterval *= 10.f;
	D3DXVec3TransformNormal(&vInterval, &vInterval, &pTrans_Target->m_matWorld);
	
	m_vEye = vPos + vInterval;
	m_vAt = fDist * vLook + m_vEye;

	//std::cout << vLook.x << ":" << vLook.y << ":" << vLook.z << std::endl;
}

void CShoulderCamera::TopView(void)
{
	auto pTarget = Get_Target();
	if (nullptr == pTarget) return;
	auto pTrans_Target = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC);
	
	//카메라의 At과 Eye를 어깨에서 바라보게 끔 좌표 셋팅! 
	_vec3 vPos;
	pTrans_Target->Get_Info(Engine::INFO::INFO_POS, &vPos);
	//pTrans_Target->Get_Info(Engine::INFO::INFO_LOOK, &m_vLook_TopView);
	//D3DXVec3Normalize(&m_vLook_TopView, &m_vLook_TopView);
	
	if(m_fDist_TopView < 15)
		m_fDist_TopView += 0.5f;
	m_vEye = vPos + m_vUp * m_fDist_TopView;
	m_vAt = 1 * m_vLook_TopView + vPos;
	//m_vAt = vPos;
//	std::cout << m_vEye.x << "|" << m_vEye.y << "|" << m_vEye.z << "ddd" << m_vAt.x << "|" << m_vAt.y << "|" << m_vAt.z << std::endl;
}

void CShoulderCamera::DoShack(bool bGayScale)
{
	m_bShacking = true;
	if(true == bGayScale)
		Engine::Get_FrameMgr()->Set_GayScale(true);
}

void CShoulderCamera::Shacking(float fTimeDelta)
{
	if (true == m_bShacking)
	{
		m_bShacking = false;
		m_iCount_Shacking = 2;
	}
	if (m_iCount_Shacking > 0)
	{
		if (m_vShacking.y > 0.1f)
			m_vDir_Shacking.y = -1 * abs(m_vDir_Shacking.y);
		if (m_vShacking.y < -0.1f)
		{
			m_vDir_Shacking.y = abs(m_vDir_Shacking.y);
			--m_iCount_Shacking;
		}
		m_vShacking += m_vDir_Shacking * fTimeDelta * 4.f;
		m_vEye += m_vShacking;
	}
	else
		Engine::Get_FrameMgr()->Set_GayScale(false);
}

_bool CShoulderCamera::Change_Mode(Engine::CONTROLTYPE eMode)
{
	auto pTarget = Get_Target();
	if (nullptr == pTarget)
		return 0;
	auto pPlayer = (CPlayer*)pTarget;

	if (m_eMode != eMode)
	{
		m_eMode = eMode;
		if (m_eMode == Engine::CONTROLTYPE_BUILD || m_eMode == Engine::CONTROLTYPE_FISHING)
		{
			m_bMouseFix = true;
			m_fDist_TopView = 0.f;
			if (nullptr != pTarget)
			{
				auto pTrans_Target = (Engine::CTransform*)pTarget->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC);
				pTrans_Target->Get_Info(Engine::INFO::INFO_LOOK, &m_vLook_TopView);
				D3DXVec3Normalize(&m_vLook_TopView, &m_vLook_TopView);
			}
		}
		else
			m_bMouseFix = false;
		return true;
	}
	return false;
}

CShoulderCamera* CShoulderCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	CShoulderCamera*	pInstance = new CShoulderCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}
