#include "stdafx.h"
#include "MiniGame.h"

#include "Export_Function.h"

CMiniGame::CMiniGame(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_fAddTimeDelta(0.f)
	, m_fButtonSpeed(300.f)
	, m_iCombo(0.f)
{
	m_bActivate = false;
}

CMiniGame::~CMiniGame(void)
{
}

HRESULT CMiniGame::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	////////////////게임판
	m_fSizeX = 600.f;
	m_fSizeY = 150.f;
	m_fX = WINCX * 0.5f;
	m_fY = WINCY * 0.5f;
	m_fPointX = m_fX - 120.f;

	//Ready_Button();

	return S_OK;
}

_int CMiniGame::Update_Object(const _float& fTimeDelta)
{
	if (false == m_bActivate) 
		return 0;
	
	if (m_fPointSizeX > 4.f)
		m_fPointSpeed = -abs(m_fPointSpeed);
	if (m_fPointSizeX < 0.f)
		m_fPointSpeed = abs(m_fPointSpeed);

	m_fPointSizeX += m_fPointSpeed * fTimeDelta;
	

	if (m_fAddTimeDelta > 10000.f)
		m_fAddTimeDelta = 0.f;
	Engine::CGameObject::Update_Object(fTimeDelta);
	Update_TargetInfo();
	Update_Button(fTimeDelta);

	Update_ComboInfo(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_UI, this);
	return 0;
}

void CMiniGame::LateUpdate_Object(void)
{
	if (false == m_bActivate)
		return;
}

void CMiniGame::Render_Object(void)
{
	if (false == m_bActivate)
		return;
	_matrix		matWorld, matView, matOriginView, matOriginProj;

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matOriginView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOriginProj);

	matWorld._11 = m_fSizeX;
	matWorld._22 = m_fSizeY;
	matWorld._33 = 1.f;
	matWorld._41 = m_fX - (WINCX >> 1);
	matWorld._42 = -m_fY + (WINCY >> 1);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	m_pTextureCom->Render_Texture(5);
	m_pBufferCom->Render_Buffer();
	//////////////////////////
	D3DXMatrixIdentity(&matWorld);
	//matWorld._11 = 100.f;
	//matWorld._22 = 100.f;
	matWorld._11 = 30.f + m_fPointSizeX;
	matWorld._22 = 120.f;
	matWorld._33 = 1.f;
	matWorld._41 = m_fPointX - (WINCX >> 1);
	matWorld._42 = -m_fY + (WINCY >> 1) - 3;
	m_pTextureCom->Render_Texture(6);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pBufferCom->Render_Buffer();
	
	Render_Button();

	Render_ComboInfo();

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOriginView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOriginProj);	

	
}

void CMiniGame::Ready_Button(void)
{
	MINIGAME_DEST tButton;
	m_lstButton.clear();
	m_lstRenderButton.clear();
	m_lstComboInfo.clear();
	for(int i = 0; i < 15; ++i)
	{
		tButton.fStartTime = 0.2 + (rand() % 1000) * 0.001;
		if (i != 14)
			tButton.eType = (EBUTTON_TYPE)(rand() % BUTTON_ENTER);
		else
			tButton.eType = BUTTON_ENTER;
		std::cout << tButton.eType << std::endl;
		tButton.vPos = _vec2(m_fX + m_fSizeX *0.5f - 50, m_fY);
		tButton.vSize = _vec2(100.f, 100.f);
		m_lstButton.emplace_back(tButton);
	}

	Engine::Get_SoundMgr()->StopSound(Engine::CSoundMgr::BGM_SUB);
	Engine::Get_SoundMgr()->SetVolume(Engine::CSoundMgr::BGM, 0.0f);

	TCHAR szSoundName[MIN_STR];
	ZeroMemory(szSoundName, sizeof(szSoundName));
	swprintf_s(szSoundName, L"Music%d_MiniGame", rand() % 4 + 1);
	Engine::Get_SoundMgr()->StartSound(szSoundName, Engine::CSoundMgr::BGM_SUB, true);
}

void CMiniGame::Add_Combo(COMBO_TYPE eCombo)
{
	COMBO_DEST tCombo;
	float fTypeX;
	ZeroMemory(&tCombo, sizeof(COMBO_DEST));
	m_iCombo += 1;
	if (eCombo == COMBO_TYPE::COMBO_PERPECT)
		m_iCombo += 3;
	if (eCombo == COMBO_TYPE::COMBO_MISS)
		m_iCombo = 0;
	tCombo.eCombo = eCombo;
	tCombo.iCombo = m_iCombo;
	m_lstComboInfo.emplace_back(tCombo);
}

void CMiniGame::Update_Button(const _float & fTimeDelta)
{
	m_fAddTimeDelta += fTimeDelta;

	//시간에 맞는 놈들을 넘겨준다.
	if (m_lstButton.size() > 0)
	{
		auto iter = m_lstButton.begin();
		if ((*iter).fStartTime <= m_fAddTimeDelta)
		{
			m_lstRenderButton.emplace_back(*iter);
			m_lstButton.erase(iter);
			m_fAddTimeDelta = 0.f;
		}
	}

	//랜더 버튼쪽에서 일정 범위를 넘어가면 지워준다.
	if (m_lstRenderButton.size() > 0)
	{
		COMBO_TYPE eCombo = COMBO_TYPE::COMBO_NONE;
		auto iter = m_lstRenderButton.begin();
		auto& tButton = *iter;

		DWORD dwKey = 0;
		if (Engine::KeyDown(KEY_LEFT))
			dwKey |= KEY_LEFT;
		else if (Engine::KeyDown(KEY_UP))
			dwKey |= KEY_UP;
		else if (Engine::KeyDown(KEY_RIGHT))
			dwKey |= KEY_RIGHT;
		else if (Engine::KeyDown(KEY_DOWN))
			dwKey |= KEY_DOWN;
		else if (Engine::KeyDown(KEY_RETURN))
			dwKey |= KEY_RETURN;

		if ((dwKey & KEY_LEFT) ||
			(dwKey & KEY_UP) ||
			(dwKey & KEY_RIGHT) ||
			(dwKey & KEY_DOWN) ||
			(dwKey & KEY_RETURN))
			eCombo = COMBO_TYPE::COMBO_MISS;

		float fPerpectSize;
		float fExcellectSize;

		switch (tButton.eType)
		{
		case CMiniGame::BUTTON_LEFT:
			if (dwKey & KEY_LEFT)
			{
				if (CheckPoint(m_fPointX, tButton.vPos.x, fPerpectSize))
					eCombo = COMBO_TYPE::COMBO_PERPECT;
				else if (CheckPoint(m_fPointX, tButton.vPos.x, fExcellectSize))
					eCombo = COMBO_TYPE::COMBO_EXCELLENT;
			}
			break;
		case CMiniGame::BUTTON_UP:
			if (dwKey & KEY_UP)
			{
				if (CheckPoint(m_fPointX, tButton.vPos.x, 30.f))
					eCombo = COMBO_TYPE::COMBO_PERPECT;
				else if (CheckPoint(m_fPointX, tButton.vPos.x, 100.f))
					eCombo = COMBO_TYPE::COMBO_EXCELLENT;
			}
			break;
		case CMiniGame::BUTTON_RIGHT:
			if (dwKey & KEY_RIGHT)
			{
				if (CheckPoint(m_fPointX, tButton.vPos.x, 30.f))
					eCombo = COMBO_TYPE::COMBO_PERPECT;
				else if (CheckPoint(m_fPointX, tButton.vPos.x, 100.f))
					eCombo = COMBO_TYPE::COMBO_EXCELLENT;
			}
			break;
		case CMiniGame::BUTTON_DOWN:
			if (dwKey & KEY_DOWN)
			{
				if (CheckPoint(m_fPointX, tButton.vPos.x, 30.f))
					eCombo = COMBO_TYPE::COMBO_PERPECT;
				else if (CheckPoint(m_fPointX, tButton.vPos.x, 100.f))
					eCombo = COMBO_TYPE::COMBO_EXCELLENT;
			}
			break;
		case CMiniGame::BUTTON_ENTER:
			if (dwKey & KEY_RETURN)
			{
				if (CheckPoint(m_fPointX, tButton.vPos.x, 30.f))
					eCombo = COMBO_TYPE::COMBO_PERPECT;
				else if (CheckPoint(m_fPointX, tButton.vPos.x, 100.f))
					eCombo = COMBO_TYPE::COMBO_EXCELLENT;
			}
			break;
		}
		if (eCombo != COMBO_TYPE::COMBO_NONE)
		{
			Add_Combo(eCombo);
			iter = m_lstRenderButton.erase(iter);

			if(eCombo == COMBO_TYPE::COMBO_MISS)
				Engine::Get_SoundMgr()->StartSound(L"Miss_MiniGame", Engine::CSoundMgr::SE, false);
			else
				Engine::Get_SoundMgr()->StartSound(L"Ok_MiniGame", Engine::CSoundMgr::SE, false);	
		}
		//if (bDeath)
		//{
		//	Add_Combo(COMBO_TYPE::COMBO_EXCELLENT);
		//	iter = m_lstRenderButton.erase(iter);
		//}
		//else
		//{
		//	if (Engine::KeyDown(KEY_LEFT))
		//		bMiss = true;
		//	if (Engine::KeyDown(KEY_UP))
		//		bMiss = true;
		//	if (Engine::KeyDown(KEY_RIGHT))
		//		bMiss = true;
		//	if (Engine::KeyDown(KEY_DOWN))
		//		bMiss = true;
		//	if (Engine::KeyDown(KEY_RETURN))
		//		bMiss = true;
		//	if(bMiss)
		//		Add_Combo(COMBO_TYPE::COMBO_MISS);
		//}
		
		while (iter != m_lstRenderButton.end())
		{
			(*iter).vPos.x -= fTimeDelta * m_fButtonSpeed;
			if ((*iter).vPos.x < m_fX - m_fSizeX * 0.5f + 50)
			{
				Engine::Get_SoundMgr()->StartSound(L"Miss_MiniGame", Engine::CSoundMgr::SE, false);
				Add_Combo(COMBO_MISS);
				iter = m_lstRenderButton.erase(iter);
			}
			else
				++iter;
		}
	}

	if (m_lstRenderButton.size() <= 0 && m_lstButton.size() <= 0)
	{
		Engine::Get_SoundMgr()->StopSound(Engine::CSoundMgr::BGM_SUB);
		Engine::Get_SoundMgr()->SetVolume(Engine::CSoundMgr::BGM, 0.3f);
	}
}

void CMiniGame::Render_Button()
{
	_matrix		matWorld;
	for(auto& tButton : m_lstRenderButton)
	{		
		D3DXMatrixIdentity(&matWorld);
		matWorld._11 = tButton.vSize.x;
		matWorld._22 = tButton.vSize.x;
		matWorld._33 = 1.f;
		matWorld._41 = tButton.vPos.x - (WINCX >> 1);
		matWorld._42 = -tButton.vPos.y + (WINCY >> 1);

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		m_pTextureCom->Render_Texture(tButton.eType);
		m_pBufferCom->Render_Buffer();
	}
}

_bool CMiniGame::CheckPoint(float fPointX, float fButtonX, float fSize)
{
	if (fPointX - fSize < fButtonX && fPointX + fSize > fButtonX)
		return true;
	return false;
}

HRESULT CMiniGame::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_ButtonArrow"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Texture
	pComponent = m_pTextureCom_Number = dynamic_cast<Engine::CTexture*>(Engine::Clone(RESOURCE_STAGE, L"Texture_Number"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture_Number", pComponent);

	// Transform
	pComponent = m_pTransCom = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	pComponent->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone_Prototype(L"Shader_Sample"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);


	return S_OK;
}

void CMiniGame::Update_ComboInfo(const _float& fTimeDelta)
{
	if (m_lstComboInfo.size() == 0) return;

	auto iter = m_lstComboInfo.begin();
	while (iter != m_lstComboInfo.end())
	{
		auto& tCombo = (*iter);
		auto fDist = D3DXVec2Length(&tCombo.vPos);
		if (fDist > 40.f)
		{
			iter = m_lstComboInfo.erase(iter);
			continue;
		}
		else
			++iter;

		tCombo.vPos.y -= 100.f * fTimeDelta;
		tCombo.vSize += tCombo.vSize * fTimeDelta;
	}
}

void CMiniGame::Render_ComboInfo(void)
{
	DWORD dwComboColor;
	TCHAR szComboInfo[MIN_STR];
	int		iImageIndex = 0;
	std::wstring strType;
	for (auto tComboInfo : m_lstComboInfo)
	{
		switch (tComboInfo.eCombo)
		{
		case COMBO_PERPECT:
			strType = L"PERPECT";
			m_dwColor = D3DCOLOR_ARGB(255, 228, 31, 212);
			m_dwColorBorder = D3DCOLOR_ARGB(255, 64, 30, 210);
			tComboInfo.vSize.x = 250.f;
			tComboInfo.vSize.y = 80.f;
			iImageIndex = 7;
			break;
		case COMBO_EXCELLENT:
			strType = L"EXCELLENT";
			m_dwColor = D3DCOLOR_ARGB(255, 24, 47, 214);
			m_dwColorBorder = D3DCOLOR_ARGB(255, 56, 130, 139);
			tComboInfo.vSize.x = 280.f;
			tComboInfo.vSize.y = 80.f;
			iImageIndex = 8;
			break;
		case COMBO_MISS:
			strType = L"MISS";
			m_dwColor = D3DCOLOR_ARGB(255, 212, 228, 38);
			m_dwColorBorder = D3DCOLOR_ARGB(255, 59, 34, 10);
			tComboInfo.vSize.x = 180.f;
			tComboInfo.vSize.y = 80.f;
			iImageIndex = 9;
			break;
		default:
			continue;
		}
		_matrix matWorld;
		D3DXMatrixIdentity(&matWorld);
		matWorld._11 = tComboInfo.vSize.x;
		matWorld._22 = tComboInfo.vSize.y;
		matWorld._33 = 1.f;
		matWorld._41 = 10.f;
		matWorld._42 = -(tComboInfo.vPos.y + WINCY * 0.5f - 70.f - 120.f) + (WINCY >> 1);
		m_pTextureCom->Render_Texture(iImageIndex);
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
		m_pBufferCom->Render_Buffer();

		//Engine::Render_Font(L"Font_Minigame", strType.c_str(), &_vec2(tComboInfo.vPos.x + WINCX * 0.5f + fTypeX, tComboInfo.vPos.y + WINCY * 0.5f - 70.f - 120.f), dwComboColor);

		if (tComboInfo.iCombo > 0)
		{
			int number = tComboInfo.iCombo;
			list<int> lstNumber;
			while (number != 0)
			{
				lstNumber.emplace_front(number % 10);
				number /= 10;
			}
			float fSize = -1* (float)lstNumber.size() * 0.5f;
			for (auto& iNumber : lstNumber)
			{
				D3DXMatrixIdentity(&matWorld);
				matWorld._11 = 50.f;
				matWorld._22 = 50.f;
				matWorld._33 = 1.f;
				matWorld._41 = fSize * 40.f + 20.f;
				matWorld._42 = -(tComboInfo.vPos.y + WINCY * 0.5f - 100.f) + (WINCY >> 1);
				++fSize;
				//m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
				//m_pTextureCom_Number->Render_Texture(iNumber);
				//m_pBufferCom->Render_Buffer();
				//++fSize;

				LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
				NULL_CHECK(pEffect);

				SetUp_ConstantTable(pEffect, m_pTextureCom_Number, iNumber, matWorld);
				pEffect->AddRef();

				_uint iPassCnt = 0;
				pEffect->Begin(&iPassCnt, 0);
				pEffect->BeginPass(0);

				m_pBufferCom->Render_Buffer();

				pEffect->EndPass();
				pEffect->End();

				Safe_Release(pEffect);
			}
		}
	}
}

HRESULT CMiniGame::Update_TargetInfo(void)
{
	auto pTarget = Get_Target();
	if (nullptr == pTarget)
		return E_FAIL;
	auto pStatus = (Engine::CStatus*)pTarget->Get_Component(L"Com_Status", Engine::COMPONENTID::ID_STATIC);
}

HRESULT CMiniGame::SetUp_ConstantTable(LPD3DXEFFECT pEffect, Engine::CTexture * pTexture, int iTextureIndex, _matrix matWorld)
{
	pEffect->AddRef();
	_vec4 vColor = D3DXCOLOR(m_dwColor);
	_vec4 vColorBorder = D3DXCOLOR(m_dwColorBorder);
	_matrix		matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fCurHp", 1.f);
	pEffect->SetInt("g_iType", 2);
	pEffect->SetFloatArray("g_vColorHp", (float*)&vColor, 4);
	pEffect->SetFloatArray("g_vColorBorder", (float*)&vColorBorder, 4);
	
	pEffect->SetBool("g_bGrayScale", Engine::Get_FrameMgr()->Get_GayScale());
	pEffect->SetBool("g_bVertical", false);
	pTexture->Render_Texture(pEffect, "g_BaseTexture", iTextureIndex);

	Safe_Release(pEffect);
	return S_OK;
}

CMiniGame * CMiniGame::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMiniGame *	pInstance = new CMiniGame(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CMiniGame::Free(void)
{
	Engine::CGameObject::Free();
}



