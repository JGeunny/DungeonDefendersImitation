#include "SoundMgr.h"
#include <fstream>
#include <iostream>
#include <atlstr.h>

USING(Engine)

IMPLEMENT_SINGLETON(CSoundMgr)

#define BGM_CHANNEL_COUNT_MAX 1
#define SE_CHANNEL_COUNT_MAX 16
#define ENV_CHANNEL_COUNT_MAX 8

CSoundMgr::CSoundMgr()
{

}

CSoundMgr::~CSoundMgr()
{
	Release();
}

void CSoundMgr::Initialize()
{
	// �ý��� �ʱ�ȭ
	FMOD_System_Create(&m_pSystem);
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	int iBGMID = static_cast<int>(SOUNDID::BGM);
	int iSEID = static_cast<int>(SOUNDID::SE);
	int iSE_SUBID = static_cast<int>(SOUNDID::SE_SUB);
	int iENVID = static_cast<int>(SOUNDID::ENV);
	int iBGM_SUBID = static_cast<int>(SOUNDID::BGM_SUB);

	// ä�� ����� �̸� ����
	m_vecChannel[iBGMID].resize(BGM_CHANNEL_COUNT_MAX);
	m_vecChannel[iSEID].resize(SE_CHANNEL_COUNT_MAX);
	m_vecChannel[iSE_SUBID].resize(SE_CHANNEL_COUNT_MAX);
	m_vecChannel[iENVID].resize(ENV_CHANNEL_COUNT_MAX);
	m_vecChannel[iBGM_SUBID].resize(BGM_CHANNEL_COUNT_MAX);

	// �ʱ� ���� ����
	m_fVolume[iBGMID] = 0.3f;
	m_fVolume[iSEID] = 0.5f;
	m_fVolume[iSE_SUBID] = 0.1f;
	m_fVolume[iENVID] = 0.5f;
	m_fVolume[iBGM_SUBID] = 0.3f;
}

void CSoundMgr::Update(_vec3 vCameraPos)
{
	m_vCameraPos = vCameraPos;
	//FMOD_VECTOR vel = { 0,0,0 }; // �ӵ� ����, ���÷� ����
	//FMOD_VECTOR forward = { 1,0,0 }; // �������� ���ϰ� �÷��̾ ���͸� ���ϵ��� ����
	//FMOD_VECTOR up = { 0,0,1 }; // ����� ����� �÷��̾��� ��� ������ ������� �ٽ� �����˴ϴ�.
	//FMOD_VECTOR playerEar = { 0,0,0 };
	//playerEar = *(FMOD_VECTOR*)&vPlayerPos;
	//FMOD_RESULT result = FMOD_System_Set3DListenerAttributes(m_pSystem, 0, &playerEar, &vel, &forward, &up);
}

void CSoundMgr::Release()
{
	// ���� ���� ����
	for (auto& iter : m_mapSound)
	{
		FMOD_Sound_Release(iter.second);
	}
	m_mapSound.clear();

	// ä�� clear
	int iSoundIDSize = static_cast<int>(SOUNDID::End);
	for (int i = 0; i < iSoundIDSize; ++i)
	{
		m_vecChannel[i].clear();
		m_vecChannel[i].shrink_to_fit();
	}

	FMOD_System_Close(m_pSystem);
	FMOD_System_Release(m_pSystem);
}

void CSoundMgr::SetVolume(SOUNDID eSoundID, _vec3 * pvSoundPos)
{
	auto fVolume = m_fVolume[eSoundID];
	if (nullptr == pvSoundPos)
	{
		auto fDist = D3DXVec3Length(&(m_vCameraPos - *pvSoundPos));
		float fRate = 1  - (fDist / 10.f);
		fRate = fRate < 0 ? 0 : fRate;
		fVolume *= fRate;
	}
	SetVolume(eSoundID, fVolume);
}

void CSoundMgr::StartSound(wstring wstrSoundKey, SOUNDID eSoundID, bool bLoop, _vec3* pvPos)
{
	if (nullptr == pvPos)
		return;

	// ���� ã��
	auto iter = m_mapSound.find(wstrSoundKey);
	if (iter == m_mapSound.end())
	{
		MSG_BOX("StartSound - ã������ ���尡 �����ϴ�.");
		return;
	}

	// ��������� ä�� ã��
	FMOD_CHANNEL** pChannel = GetPlayableChannel(eSoundID);
	if (pChannel == nullptr) 
	{
		MSG_BOX("StartSound - ��� ������ ä���� �����ϴ�.");
		return;
	}

	//FMOD_Channel_Set3DAttributes((*pChannel), &vPos, 0);
	//FMOD_Sound_Set3DMinMaxDistance(iter->second, 1.0f, 10.0f);
	// ���
	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &(*pChannel));
	SetVolume(eSoundID, pvPos);

	// �ݺ� ����
	if (bLoop)
		FMOD_Channel_SetMode(*pChannel, FMOD_LOOP_NORMAL);
	else
		FMOD_System_Update(m_pSystem);

}

void CSoundMgr::PauseSound(SOUNDID eSoundID)
{
	// �ش� ID�� ä�� ��� �Ͻ�����
	int iSoundID = static_cast<int>(eSoundID);
	for (auto& iter : m_vecChannel[iSoundID])
		FMOD_Channel_SetPaused(iter, TRUE);
}

void CSoundMgr::ResumeSound(SOUNDID eSoundID)
{
	// �ش� ID�� ä�� ��� �簳
	int iSoundID = static_cast<int>(eSoundID);
	for (auto& iter : m_vecChannel[iSoundID])
		FMOD_Channel_SetPaused(iter, FALSE);
}

void CSoundMgr::StopSound(SOUNDID eSoundID)
{
	// �ش� ID�� ä�� ��� ����
	int iSoundID = static_cast<int>(eSoundID);
	for (auto& iter : m_vecChannel[iSoundID])
		FMOD_Channel_Stop(iter);
}

void CSoundMgr::StopAll()
{
	// ��� ä�� ����
	int iSoundIDSize = static_cast<int>(SOUNDID::End);
	for (int i = 0; i < iSoundIDSize; ++i)
	{
		for (auto& iter : m_vecChannel[i])
			FMOD_Channel_Stop(iter);
	}
}

void CSoundMgr::SetVolume(SOUNDID eSoundID, float _fVolume)
{
	// �ش� ID�� ä�� ���� ����
	int iSoundID = static_cast<int>(eSoundID);
	for (auto& iter : m_vecChannel[iSoundID])
		FMOD_Channel_SetVolume(iter, _fVolume);
}

int CSoundMgr::GetIsPlay(FMOD_CHANNEL ** pChannel)
{
	// ����� üũ
	FMOD_BOOL bIsPlay;
	bIsPlay=FMOD_Channel_IsPlaying(*pChannel, &bIsPlay);
	return (bIsPlay != 1);
}

FMOD_CHANNEL ** CSoundMgr::GetPlayableChannel(SOUNDID eSoundID)
{
	// �ش� ä���� ���ɿ��� Ȯ���ؼ� ������ ��ȯ
	int iSoundID = static_cast<int>(eSoundID);
	for (auto& iter : m_vecChannel[iSoundID])
	{
		if (GetIsPlay(&iter))
			return &iter;
	}

	return nullptr;
}

HRESULT CSoundMgr::AddSoundFile(wstring wstrFilePath, wstring wstrSoundKey)
{
	// char* ��ȯ
	int len = wcslen((wchar_t*)wstrFilePath.c_str());
	string str = CW2A(wstrFilePath.c_str());

	// ����
	FMOD_SOUND* pSound = nullptr;
	//FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, str.c_str(), FMOD_HARDWARE, 0, &pSound);
	FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, str.c_str(), FMOD_HARDWARE | FMOD_3D, 0, &pSound);	

	if (eRes == FMOD_OK)
	{
		// ������ �ִ� ä������ �˻��غ��� ������ ���з� ����
		for (auto& iter : m_mapSound)
		{
			if (iter.first == wstrSoundKey)
			{
			//	MSG_BOX("AddSoundFile - �̹� �ִ� ���� Ű���Դϴ�.");
				return E_FAIL;
			}
		}

		m_mapSound.emplace(wstrSoundKey, pSound);
		return S_OK;
	}
	else
	{
		MSG_BOX("AddSoundFile - ���� �ֱ⿡ �����߽��ϴ�.");
		return E_FAIL;
	}
}

HRESULT CSoundMgr::LoadSoundFromTxt(const wstring & wstrFilePath)
{
	wifstream fin;

	fin.open(wstrFilePath);

	if (fin.fail())
	{
		//MSG_BOX("LoadSoundFromTxt - ���� ���� ����");
		return E_FAIL;
	}

	// ���� ���� �� ������� �Է¹޾� ���� ������ ����
	TCHAR szFilePath[128] = L"";
	TCHAR szSoundKey[128] = L"";
	TCHAR szNumber[128] = L"";

	fin.getline(szNumber, 128);
	int iSoundCount = _ttoi(szNumber);

	HRESULT hr = 0;

	Initialize();

	for (int i = 0; i < iSoundCount; ++i)
	{
		fin.getline(szFilePath, 128, '|');
		fin.getline(szSoundKey, 128);

		AddSoundFile(szFilePath, szSoundKey);
	}

	fin.close();

	return S_OK;
}
