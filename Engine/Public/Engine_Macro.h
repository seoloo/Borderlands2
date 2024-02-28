#pragma once

#define NO_COPY(ClassName)								\
ClassName(const ClassName&) = delete;					\
ClassName& operator=(const ClassName&) = delete;

#define DECLARE_SINGLETON(ClassName)					\
		NO_COPY(ClassName)								\
public :												\
	static ClassName* Get_Instance();					\
	static unsigned long Destroy_Instance();			\
private:												\
	static ClassName* m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)					\
ClassName* ClassName::m_pInstance = nullptr;			\
ClassName* ClassName::Get_Instance()					\
{														\
	if (nullptr == m_pInstance)							\
		m_pInstance = new ClassName;					\
	return m_pInstance;									\
}														\
unsigned long  ClassName::Destroy_Instance()			\
{														\
	unsigned long	dwRefCnt = { 0 };					\
	if(nullptr != m_pInstance)							\
	{													\
		dwRefCnt = m_pInstance->Release();				\
		if(0 == dwRefCnt)								\
			m_pInstance = nullptr;						\
	}													\
	return dwRefCnt;									\
}

#define BEGIN(NAME)		namespace NAME {
#define END				}


#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)



#endif

#define MSG_BOX(MESSAGE)		MessageBox(0, TEXT(MESSAGE), TEXT("System Error"), MB_OK)

#define KEYUP(VK)				CGameInstance::Get_Instance()->Get_Key_State(VK) == UP

#define KEYDOWN(VK)				CGameInstance::Get_Instance()->Get_Key_State(VK) == DOWN

#define KEYPRESSING(VK)			CGameInstance::Get_Instance()-> Get_Key_State(VK) == HOLD