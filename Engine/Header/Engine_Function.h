#ifndef Engine_Function_h__
#define Engine_Function_h__

namespace Engine
{
	inline _vec3* MatrixToRadian(_vec3* pOut_Radian, _matrix* pmatRotate)
	{
		*pOut_Radian = _vec3(atan2(pmatRotate->_23, pmatRotate->_33),
			atan2(-pmatRotate->_13, sqrt(pow(pmatRotate->_23, 2) + pow(pmatRotate->_33, 2))),
			atan2(pmatRotate->_12, pmatRotate->_11));
		return pOut_Radian;
	}
	inline _float Convert_TrueRadian(_float fRadian)
	{
		_float fAngle = D3DXToDegree(fRadian);
		_int iAngle = (int)fAngle;
		return D3DXToRadian(iAngle % 360 + fAngle - iAngle);
	}

	inline _vec3* DirToRadian(_vec3* pOut_Radian, _vec3* pIn_vDir)
	{
		_vec3 vDir, vLook;
		vLook = { 0.f, 0.f, 1.f };
		vDir = *pIn_vDir;

		//Y축 회전
		_vec3 vTargetDirY = vDir;
		vTargetDirY.y = 0.f;

		D3DXVec3Normalize(&vTargetDirY, &vTargetDirY);
		float fRadianY = acosf(D3DXVec3Dot(&vTargetDirY, &vLook));

		if (vDir.x < 0.f)
			fRadianY = 2 * D3DX_PI - fRadianY;

		//X축 회전
		_vec3 vTargetDirX = vDir;

		D3DXVec3Normalize(&vTargetDirX, &vTargetDirX);
		float fRadianX = -acosf(D3DXVec3Dot(&vTargetDirX, &vTargetDirY));

		if (vDir.y < 0.f)
			fRadianX = 2 * D3DX_PI - fRadianX;

		if (std::isnan(fRadianX))
			fRadianX = 0.f;
		if (std::isnan(fRadianY))
			fRadianY = 0.f;
		fRadianX = Engine::Convert_TrueRadian(fRadianX);
		fRadianY = Engine::Convert_TrueRadian(fRadianY);

		*pOut_Radian = _vec3(fRadianX, fRadianY, 0.f);
		return pOut_Radian;
	}

	template <typename T>
	unsigned long Safe_AddRef(T& pInstance)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->AddRef();
		}

		return dwRefCnt;
	}

	template <typename T>
	DWORD Safe_Release(T& pointer)
	{
		DWORD	dwRefCnt = 0;
		if (NULL != pointer)
		{
			dwRefCnt = pointer->Release();
			if (dwRefCnt == 0)
				pointer = NULL;
		}
		return dwRefCnt;
	}                                                       

	template <typename T>
	void Safe_Single_Destory(T& pointer)
	{
		if (NULL != pointer)
		{
			pointer->DestroyInstance();
			pointer = NULL;
		}
	}

	template <typename T>
	void Safe_Delete(T& pointer)
	{
		if (NULL != pointer)
		{
			delete pointer;
			pointer = NULL;
		}
	}

	template <typename T>
	void Safe_Delete_Array(T& pointer)
	{
		if (NULL != pointer)
		{
			delete[] pointer;
			pointer = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////
	/////////////////////////////////Functor//////////////////////////

	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const _tchar* pTag)
			: m_pTargetTag(pTag) {		}
		~CTag_Finder() {		}
	public:
		template<typename T>
		bool operator()(const T& pair)
		{
			if (0 == lstrcmpW(m_pTargetTag, pair.first))
			{
				return true;
			}

			return false;
		}

	private:
		const _tchar*		m_pTargetTag = nullptr;
	};

	class CDeleteObj
	{
	public:
		explicit CDeleteObj(void) {}
		~CDeleteObj(void) {}
	public: // operator
		template <typename T>
		void operator () (T& pInstance)
		{
			_ulong dwRefCnt = 0;

			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}
	};

	// 연관컨테이너 삭제용
	class CDeleteMap
	{
	public:
		explicit CDeleteMap(void) {}
		~CDeleteMap(void) {}
	public: // operator	
		template <typename T>
		void operator () (T& Pair)
		{
			_ulong dwRefCnt = 0;

			dwRefCnt = Pair.second->Release();

			if (0 == dwRefCnt)
				Pair.second = NULL;
		}
	};
}


#endif // Engine_Function_h__
