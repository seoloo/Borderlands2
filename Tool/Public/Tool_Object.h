#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
class CVIBuffer;
END

BEGIN(Tool)
class CTool_Object abstract : public CGameObject
{
public:
	typedef struct tagOBJECTDESC {
		wstring strName = L"";
		_float3	ObjectSize = { 1.f, 1.f, 1.f };
		_float	fRadian = { 0.f };
		_vector	CreatePos = { XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };
	}OBJECTDESC;

protected:
	CTool_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTool_Object(const CTool_Object& rhs);
	virtual ~CTool_Object() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	OBJINFO Get_Info()
	{
		return m_Info;
	}
	_matrix Get_WorldMatrix();

private:
	CModel*		m_pModelCom = {nullptr};
	CShader*	m_pShaderCom = { nullptr };
	CRenderer*	m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer*	m_pVIBufferCom = { nullptr };

protected:
	_vector		m_vRight = { 0.f, 0.f, 0.f };
	_vector		m_vUp = { 0.f, 0.f, 0.f };
	_vector		m_vLook = { 0.f, 0.f, 0.f };
	OBJINFO		m_Info;

protected:
	class CImgui_Manager* m_pImgui_Manager = { nullptr };
	class CTool_ObjectManager* m_pObject_Manager = { nullptr };

public:
	virtual void Free() override;
};

END