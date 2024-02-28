#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)
class CCatridge final : public CGameObject
{
public:
	typedef struct tagPointInstanceDesc
	{
		_int		iNumInstance = { 0 };
		_float2		fSize = { 0.f,0.f };
		_float4		vPivotPos = { 0.f,0.f,0.f,0.f };
		_float3		vRange = { 0.f,0.f,0.f };
		_float		fLifeTime = { 0.f };
		_float		fMinSpeed, fMaxSpeed = (0.f, 0.f);
		_bool		bFalling = { false };
		_bool		bRising = { false };
		_bool		bExplosion = { false };
		_bool		bBleeding = { false };
		_bool		bDig = { false };
		_bool		bCatridge = { false };
		_bool		bGreen = { false };
	}POINTINSTANCEDESC;

private:
	CCatridge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCatridge(const CCatridge& rhs);
	virtual ~CCatridge() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CTransform* m_pPlayerTransformCom = { nullptr };
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	_bool	m_bExplosion = { false };
	_bool	m_bBleeding = { false };
	_bool	m_bDig = { false };
	_bool	m_bCatridge = { false };
	_float4	m_fPos = { 0.f,0.f,0.f,0.f };
	_float4	m_fCreatePos = { 0.f,0.f,0.f,0.f };
	_vector m_vLook;
	POINTINSTANCEDESC m_Desc;

	_vector m_vPlayerPos;
	_vector m_vPlayerLook;

public:
	static CCatridge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END