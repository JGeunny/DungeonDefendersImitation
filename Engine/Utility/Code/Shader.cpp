#include "Shader.h"

USING(Engine)

Engine::CShader::CShader(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_pEffect(nullptr)
{
	m_pGraphicDev->AddRef();
}

Engine::CShader::CShader(const CShader& rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
	, m_pEffect(rhs.m_pEffect)
{
	m_pGraphicDev->AddRef();
	m_pEffect->AddRef();
}

Engine::CShader::~CShader(void)
{

}

HRESULT Engine::CShader::Ready_Shader(const _tchar* pFilePath)
{
	/*m_pEffect == nullptr 에러인 경우

	1.만약 m_pEffect는 값이 있고 m_pErrMsg는 값이 NULL인 경우가 정상
	2.만약 m_pEffect 값이 있고 m_pErrMsg 값이 있는 경우, 경고 메세지를 갖고 있는 상황*/

	//if(FAILED(D3DXCreateEffectFromFile(m_pGraphicDev, 
	//									pFilePath,
	//									NULL,	// 쉐이더 사용 시 포함할 메크로파일
	//									NULL,	// 쉐이더 사용 시 포함할 헤더파일
	//									D3DXSHADER_DEBUG, // 쉐이더 디버깅 옵션 플래그(외부라이브러리 파일을 포함해야하지만 만약 대비해서 옵션 설정)
	//									NULL,	// 각종 리소스 정보를 어디에 저장할 것인가(null을 넣어주면 기본 값으로 장치에 저장 : 그래픽 카드)
	//									&m_pEffect,	
	//									&m_pErrMsg)))	// 디버깅은 불가능하지만 에러 및 경고가 발생했을 경우 문자열 형태로 문제 상황을 저장할 수 있다.
	//{
	//	MessageBoxA(NULL, (char*)m_pErrMsg->GetBufferPointer(), "Shader_Error", MB_OK);
	//	return E_FAIL;
	//}

	//else if (nullptr != m_pErrMsg)
	//{
	//	MessageBoxA(NULL, (char*)m_pErrMsg->GetBufferPointer(), "Shader_warning", MB_OK);
	//}
	
	if (FAILED(D3DXCreateEffectFromFile(m_pGraphicDev,
		pFilePath,
		NULL,	// 쉐이더 사용 시 포함할 메크로파일
		NULL,	// 쉐이더 사용 시 포함할 헤더파일
		D3DXSHADER_DEBUG, // 쉐이더 디버깅 옵션 플래그(외부라이브러리 파일을 포함해야하지만 만약 대비해서 옵션 설정)
		NULL,	// 각종 리소스 정보를 어디에 저장할 것인가(null을 넣어주면 기본 값으로 장치에 저장 : 그래픽 카드)
		&m_pEffect,
		&m_pErrMsg)))	// 디버깅은 불가능하지만 에러 및 경고가 발생했을 경우 문자열 형태로 문제 상황을 저장할 수 있다.
		return E_FAIL;

	return S_OK;
	return S_OK;
}

CShader* Engine::CShader::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath)
{
	CShader*	pInstance = new CShader(pGraphicDev);

	if (FAILED(pInstance->Ready_Shader(pFilePath)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent* Engine::CShader::Clone(void)
{
	return new CShader(*this);
}

void Engine::CShader::Free(void)
{
	Safe_Release(m_pEffect);
	Safe_Release(m_pGraphicDev);
}

HRESULT CShader::Set_Value(D3DXHANDLE hParameter, void * pValue, size_t iSize)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetValue(hParameter, pValue, iSize);
}

HRESULT CShader::Set_Texture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetTexture(hParameter, pTexture);
}

HRESULT CShader::Set_Bool(D3DXHANDLE hParameter, _bool Value)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetBool(hParameter, Value);
}

HRESULT CShader::Commit_Changes()
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->CommitChanges();
}

HRESULT CShader::Begin_Shader(_uint * pNumPasses)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->Begin(pNumPasses, 0);
}

HRESULT CShader::Begin_Pass(_uint iPassIndex)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->BeginPass(iPassIndex);
}

HRESULT CShader::End_Shader()
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->End();
}

HRESULT CShader::End_Pass()
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->EndPass();
}
