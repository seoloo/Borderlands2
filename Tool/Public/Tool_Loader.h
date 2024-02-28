#pragma once

#include "Tool_Defines.h"
#include "Base.h"

/* 다음레벨에 대한 자원을 생성하는 역활을 멀티스레드를 생성하여 수행한다. */

BEGIN(Tool)

class CTool_Loader final : public CBase
{
private:
	CTool_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_Loader() = default;

public: /* Getter */
	const _tchar* Get_LoadingText() const {
		return m_szLoading;
	}

public:
	HRESULT Initialize(LEVEL eNextLevel);
	HRESULT LoadingForNextLevel();

public:
	_bool is_Finished() const {
		return m_isFinished;
	}

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	HANDLE					m_hThread = { 0 };
	CRITICAL_SECTION		m_CriticalSection;
	LEVEL					m_eNextLevel = { LEVEL_END };

	_bool					m_isFinished = { false };
	_tchar					m_szLoading[MAX_PATH] = TEXT("");

private:
	HRESULT Loading_ForTool();

public:
	static CTool_Loader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END