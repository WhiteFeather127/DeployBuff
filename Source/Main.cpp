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
// DllMain — 在 DLL 加载时直接调用 ECInitLibrary。
// ECInitLibrary 内部会调用 Init::Initialize() 注册到 IHLibList，
// EC 框架稍后会自动回调 SDK 中的 MyInit，进而执行 fnFirst/fnOrdered。
//
// 优点：无需 .inj 钩子文件，无静态初始化顺序问题。
// ============================================================

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
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
	return TRUE;
}
