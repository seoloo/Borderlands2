#include "..\Public\GameInstance.h"

#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Light_Manager.h"
#include "Picking.h"
#include "Font_Manager.h"
#include "Frustum.h"
#include "Target_Manager.h"
#include "SoundMgr.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device{ CGraphic_Device::Get_Instance() }
	, m_pInput_Device{ CInput_Device::Get_Instance() }
	, m_pLevel_Manager{ CLevel_Manager::Get_Instance() }
	, m_pObject_Manager{ CObject_Manager::Get_Instance() }
	, m_pComponent_Manager{ CComponent_Manager::Get_Instance() }
	, m_pTimer_Manager{ CTimer_Manager::Get_Instance() }
	, m_pPipeLine{ CPipeLine::Get_Instance() }
	, m_pLight_Manager{ CLight_Manager::Get_Instance() }
	, m_pPicking{ CPicking::Get_Instance() }
	, m_pKeyMgr{ CKey_Mgr::Get_Instance() }
	, m_pFont_Manager{ CFont_Manager::Get_Instance() }
	, m_pFrustum{ CFrustum::Get_Instance() }
	, m_pTarget_Manager{ CTarget_Manager::Get_Instance() }
	, m_pSound_Manager{ CSoundMgr::Get_Instance() }
{
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pPicking);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pKeyMgr);
	Safe_AddRef(m_pSound_Manager);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iSizeX, GraphicDesc.iSizeY, ppDevice, ppContext)))
		return E_FAIL;

	if (FAILED(m_pInput_Device->Ready_DInput(hInstance, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pPicking->Initialize(*ppDevice, *ppContext, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	if (FAILED(m_pSound_Manager->Initialize()))
	{
		MSG_BOX("FMOD Initialize Fail");
		return E_FAIL;
	}

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pObject_Manager)
		return;

	/* 키보드와 마우스의 상태를 저장한다. */
	m_pInput_Device->Update_DInput();


	m_pKeyMgr->Update();

	m_pObject_Manager->Tick(fTimeDelta);
	m_pLevel_Manager->Tick(fTimeDelta);

	m_pPipeLine->Tick();
	m_pFrustum->Tick();
	m_pPicking->Tick();

	m_pObject_Manager->Late_Tick(fTimeDelta);
	m_pLevel_Manager->Late_Tick(fTimeDelta);
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Clear(iLevelIndex);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

_byte CGameInstance::Get_DIKeyState(_ubyte ubyKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(ubyKeyID);
}

_byte CGameInstance::Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouseID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseState(eMouseID);
}

_long CGameInstance::Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMouseMoveID);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel* pLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pLevel);
}

CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComTag, iIndex);
}

HRESULT CGameInstance::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Get_GameObject(_uint level, const wstring& strLayerTag)
{
	if(nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_GameObject(level, strLayerTag);
}

CGameObject* CGameInstance::Clone_GameObject(const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, CComponent* pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{

	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_Timer(const wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix(eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_Float4x4_Inverse(eState);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4();
	return m_pPipeLine->Get_CamPosition();
}

const LIGHTDESC* CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Lights(pDevice, pContext, LightDesc);
}

HRESULT CGameInstance::Remove_PointLight()
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Remove_PointLight();
}

void CGameInstance::Update_LightsPos(_float4 Position)
{
	m_pLight_Manager->Update_LightsPos(Position);
}

void CGameInstance::Lights_TurnOff(_bool TurnOff)
{
	m_pLight_Manager->Lights_TurnOff(TurnOff);
}

KEY_STATE CGameInstance::Get_Key_State(int _iKey)
{
	if (!m_pKeyMgr)
		return ERR;
	return m_pKeyMgr->Get_Key_State(_iKey);
}

_bool CGameInstance::Intersect_Collider(CBounding* pCollider)
{
	if (!m_pPicking)
		return false;

	return m_pPicking->Intersect_Collider(pCollider);
}

HRESULT CGameInstance::Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontTag, const wstring& strFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Font(pDevice, pContext, strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring& strFontTag, const wstring& strText, const _float2 vPos, _fvector vColor, _float fScale, const _float fRotation)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font(strFontTag, strText, vPos, vColor, fScale, fRotation);
}

_bool CGameInstance::isInFrustum_InWorld(_fvector vWorldPos, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isInFrustum_InWorld(vWorldPos, fRange);
}

HRESULT CGameInstance::Bind_ShaderResource_RTV(CShader* pShader, const wstring& strTargetTag, const char* pConstantName)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Bind_ShaderResource(pShader, strTargetTag, pConstantName);
}

void CGameInstance::PlaySound(const wstring& pSoundKey, float fVolume)
{
	if (!m_pSound_Manager)
		return;

	m_pSound_Manager->PlaySound(pSoundKey, fVolume);
}

void CGameInstance::PlaySoundeffect(const wstring& pSoundKey, _uint eID, float fVolume)
{
	if (!m_pSound_Manager)
		return;

	m_pSound_Manager->PlaySoundeffect(pSoundKey, (CSoundMgr::CHANNELID)eID, fVolume);
}

void CGameInstance::PlayBGM(const wstring& pSoundKey, float fVolume)
{
	if (!m_pSound_Manager)
		return;

	m_pSound_Manager->PlayBGM(pSoundKey, fVolume);
}

void CGameInstance::StopSound(_uint eID)
{
	if (!m_pSound_Manager)
		return;

	m_pSound_Manager->StopSound((CSoundMgr::CHANNELID)eID);
}

void CGameInstance::StopAll()
{
	if (!m_pSound_Manager)
		return;

	m_pSound_Manager->StopAll();
}

void CGameInstance::SetChannelVolume(_uint eID, float fVolume)
{
	if (!m_pSound_Manager)
		return;

	m_pSound_Manager->SetChannelVolume((CSoundMgr::CHANNELID)eID, fVolume);
}

bool CGameInstance::isplaying(_uint eid)
{
	if (!m_pSound_Manager)
		return false;

	return m_pSound_Manager->isplaying((CSoundMgr::CHANNELID)eid);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Destroy_Instance();

	CObject_Manager::Get_Instance()->Destroy_Instance();

	CKey_Mgr::Get_Instance()->Destroy_Instance();

	CSoundMgr::Get_Instance()->Destroy_Instance();

	CComponent_Manager::Get_Instance()->Destroy_Instance();

	CTimer_Manager::Get_Instance()->Destroy_Instance();

	CPipeLine::Get_Instance()->Destroy_Instance();

	CInput_Device::Get_Instance()->Destroy_Instance();

	CLevel_Manager::Get_Instance()->Destroy_Instance();

	CPicking::Get_Instance()->Destroy_Instance();

	CLight_Manager::Get_Instance()->Destroy_Instance();

	CFont_Manager::Get_Instance()->Destroy_Instance();

	CFrustum::Get_Instance()->Destroy_Instance();

	CTarget_Manager::Get_Instance()->Destroy_Instance();

	CGraphic_Device::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pPicking);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pKeyMgr);
	Safe_Release(m_pGraphic_Device);
}
