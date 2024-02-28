#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)
class CAridDistanceTerrain final : public CGameObject
{
public:
	typedef struct tagObjectInfo {
		wstring strName;
		_float3	ObjectSize = { 1.f, 1.f, 1.f };
		_float	fRadian = { 0.f };
		_vector	CreatePos = { XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };
	}OBJECTINFO;
private:
	CAridDistanceTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAridDistanceTerrain(const CAridDistanceTerrain& rhs);
	virtual ~CAridDistanceTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CAridDistanceTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END
