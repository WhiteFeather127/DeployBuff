#include <EC.h>
#include <EC.Listener.h>
#include <WIC.h>
#include <Syringe.h>

#include "DeployBuff.h"

// 声明 IH.Loader.cpp 中的初始化函数
namespace Init {
	bool Initialize();
}

// ============================================================
// SyringeForceLoad — 强制 SyringeIH 加载本 DLL
// ============================================================
extern "C" __declspec(dllexport) void SyringeForceLoad(void) {}

// ============================================================
// DeployBuffECInit — .inj 文件中定义的钩子函数
// 在 IHCore 的 ECInitialize 入口 (0x6BC0CD) 处触发，
// 此时 IHLibList 已就绪，可以安全调用 ECInitLibrary。
// ============================================================
extern "C" __declspec(dllexport) DWORD __cdecl DeployBuffECInit(REGISTERS* R)
{
	if (Init::Initialize())
	{
		InitDependency dep_WIC{
			"SIWinterIsComing",
			DoNotCheckVersion,
			WIC_LIBRARY_VERSION,
			true
		};

		std::function<void()> fnFirst = []()
		{
			ECListener::Listen_ClearScenario([]() {});
		};

		std::function<void()> fnOrdered = []()
		{
			try
			{
				SIClassManager::RegisterBuff<DeployBuffClass>("Deploy");
			}
			catch (SIException& e)
			{
				// 注册失败时记录错误，防止崩溃
			}
		};

		ECInitLibrary(
			"DeployBuff",
			1, 1,
			(UTF8_CString)u8"WIC Deploy Buff - Periodic Unit Deploy Trigger",
			fnFirst,
			fnOrdered,
			{ dep_WIC }
		);
	}

	return 0;
}

// ============================================================
// DllMain — 空实现，初始化在钩子函数中完成
// ============================================================

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	return TRUE;
}
