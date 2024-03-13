#pragma once

#include "Component_Manager.h"
#include "PipeLine.h"
#include "Input_Device.h"
#include "Key_Mgr.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Engine */
	HRESULT Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public:
	_byte Get_DIKeyState(_ubyte ubyKeyID);
	_byte Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouseID);
	_long Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pLevel);

public: /* For.Object_Manager */
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex = 0);
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	CGameObject* Get_GameObject(_uint level, const wstring& strLayerTag);
	class CGameObject* Clone_GameObject(const wstring& strPrototypeTag, void* pArg = nullptr);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public: /* For.Timer_Manager */
	HRESULT Add_Timer(const wstring& strTimerTag);
	_float Compute_TimeDelta(const wstring& strTimerTag, _bool bSlowDown = false, _bool bStop = false);

public: /* For.PipeLine */
	_matrix Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState);
	_float4x4 Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState);
	_matrix Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState);
	_float4x4 Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState);
	_float4	Get_CamPosition();

public: /* For.Light_Manager */
	const LIGHTDESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	HRESULT Remove_PointLight();
	void	Update_LightsPos(_float4 Position = _float4(0.f, 0.f, 0.f, 0.f));
	void	Lights_TurnOff(_bool TurnOff = false);

public: /* For. Key_Manager*/
	KEY_STATE	Get_Key_State(int _iKey);

public: /* For. Picking */
	_bool Intersect_Collider(class CBounding* pCollider);

public: /* For.Font_Manager */
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render_Font(const wstring& strFontTag, const wstring& strText, const _float2 vPos, _fvector vColor, _float fScale, const _float fRotation = 0.f);

public: /* For.Frustum */
	_bool isInFrustum_InWorld(_fvector vWorldPos, _float fRange = 0.f);

public: /* For.Target_Manager */
	HRESULT Bind_ShaderResource_RTV(class CShader* pShader, const wstring& strTargetTag, const char* pConstantName);

public: /* For. Sound_Manager*/
	void PlaySound(const wstring& pSoundKey, float fVolume);
	void PlaySoundeffect(const wstring& pSoundKey, _uint eID, float fVolume);
	void PlayBGM(const wstring& pSoundKey, float fVolume);
	void StopSound(_uint eID);
	void StopAll();
	void SetChannelVolume(_uint eID, float fVolume);
	bool isplaying(_uint eid);

private:
	class CGraphic_Device* m_pGraphic_Device = { nullptr };
	class CInput_Device* m_pInput_Device = { nullptr };
	class CLevel_Manager* m_pLevel_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CComponent_Manager* m_pComponent_Manager = { nullptr };
	class CTimer_Manager* m_pTimer_Manager = { nullptr };
	class CPipeLine* m_pPipeLine = { nullptr };
	class CLight_Manager* m_pLight_Manager = { nullptr };
	class CPicking* m_pPicking = { nullptr };
	class CKey_Mgr* m_pKeyMgr = { nullptr };
	class CFont_Manager* m_pFont_Manager = { nullptr };
	class CFrustum* m_pFrustum = { nullptr };
	class CTarget_Manager* m_pTarget_Manager = { nullptr };
	class CSoundMgr* m_pSound_Manager = { nullptr };

public:
	static void Release_Engine();
	virtual void Free() override;
};

END