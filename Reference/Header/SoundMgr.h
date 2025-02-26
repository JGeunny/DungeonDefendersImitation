#ifndef SOUNDMGR_H__
#define SOUNDMGR_H__

#include "Engine_Defines.h"

// ���� �Ŵ����� ����ϱ� ���� �߰��� �ش���. 
#include <io.h>
#include "fmod.h"

BEGIN(Engine)

// SoundMgr
// ���� ���� �а� ����

class ENGINE_DLL CSoundMgr
{
	DECLARE_SINGLETON(CSoundMgr)

	// ������ , �Ҹ���, �����Լ�
private:
	CSoundMgr();
	~CSoundMgr();

public:
	enum SOUNDID
	{
		BGM,
		SE,
		SE_SUB,
		ENV,
		BGM_SUB,
		End
	};

	///////////////////////////////////////////////

	// ���ӷ��� �Լ�,����
public:
	void Initialize();
	void Update(_vec3 vCameraPos = _vec3(0, 0, 0));
	void Release();

	void SetVolume(SOUNDID eSoundID, _vec3* pvSoundPos);

protected:


private:


	///////////////////////////////////////////////

	// ��Ÿ �Լ�,����
public:
	void			StartSound(wstring wstrSoundKey, SOUNDID eSoundID, bool bLoop = false, _vec3* pvPos = nullptr);	// �Ҹ� ���
	void			PauseSound(SOUNDID eSoundID);											// ä�� �Ͻ�����
	void			ResumeSound(SOUNDID eSoundID);											// ä�� �簳
	void			StopSound(SOUNDID eSoundID);											// ä�� ����
	void			StopAll();																// ��� ä�� ����

	void			SetVolume(SOUNDID eSoundID, float _fVolume);							// ���� ����
	int				GetIsPlay(FMOD_CHANNEL** pChannel);										// ä�� ��������� �Ǻ�
	FMOD_CHANNEL**	GetPlayableChannel(SOUNDID eSoundID);									// ��������� ä�� ��ȯ

	HRESULT			AddSoundFile(wstring wstrFilePath, wstring wstrSoundKey);				// ���� �а� �߰�
	HRESULT			LoadSoundFromTxt(const wstring & wstrFilePath);							// ���� ���� �ε�

protected:
	_vec3			m_vCameraPos = _vec3(0.f, 0.f, 0.f);

private:
	FMOD_SYSTEM*				m_pSystem;											// FMOD ���� ��ü
	std::map<std::wstring, FMOD_SOUND*>	m_mapSound;											// ���� ����
	std::vector<FMOD_CHANNEL*>		m_vecChannel[static_cast<int>(SOUNDID::End)];	// ä�� ����
	float						m_fVolume[static_cast<int>(SOUNDID::End)];	// ä�κ� ���� ����
};

END

#endif // !SOUNDMGR_H__
