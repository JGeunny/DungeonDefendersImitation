#include "stdafx.h"
#include "ToolCamera.h"

#include "Export_Function.h"

CToolCamera::CToolCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
	, m_bMouseFix(false)
	, m_fSpeed(30.f)
	, m_bEnable_Control(true)
{
	
}

CToolCamera::~CToolCamera(void)
{

}

HRESULT CToolCamera::Ready_Object(const _vec3* pEye,
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

_int CToolCamera::Update_Object(const _float& fTimeDelta)
{
	if (m_bEnable_Control)
		Check_InputDev(fTimeDelta);

	_int iExit = Engine::CCamera::Update_Object(fTimeDelta);
	return iExit;
}

void CToolCamera::LateUpdate_Object(void)
{
}

void CToolCamera::Render_Object(void)
{
}

void CToolCamera::Free(void)
{
	Engine::CCamera::Free();
}

void CToolCamera::Check_InputDev(const _float fTimeDelta)
{
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, 0, &m_matView);


	if (Engine::KeyPressing(KEY_W))
	{
		_vec3	vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed * fTimeDelta;

		m_vEye += vLength;
		m_vAt  += vLength;
	}

	if (Engine::KeyPressing(KEY_S))
	{
		_vec3	vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vLook, &vLook) * m_fSpeed * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Engine::KeyPressing(KEY_A))
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Engine::KeyPressing(KEY_D))
	{
		_vec3	vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3		vLength = *D3DXVec3Normalize(&vRight, &vRight) * m_fSpeed * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (Engine::KeyDown(KEY_RBUTTON))
		m_bMouseFix = true;
	else if (Engine::KeyUp(KEY_RBUTTON))
		m_bMouseFix = false;

	if (true == m_bMouseFix)
		Move_Mouse();
}

void CToolCamera::Fix_Mouse(void)
{
	POINT	ptMouse{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CToolCamera::Move_Mouse(void)
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

CToolCamera* CToolCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	CToolCamera*	pInstance = new CToolCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

