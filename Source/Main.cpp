#include <EC.h>
#include <EC.Listener.h>
#include <EC.LoadSave.h>
#include <EC.GameClass.h>
#include <EC.Stream.h>
#include <WIC.h>

#include "DeployBuff.h"

// Init::Initialize() 由 SyringeIH 运行时提供，声明在 IH.Loader.cpp 中
namespace Init {
	bool Initialize();
}

// ============================================================
// ECGameClass_ 派生类 — 自动接入 EC 框架的 Save/Load 系统
// 当游戏存档时, ECGameClass_SaveGame 会遍历所有注册的
// ECGameClass_ 实例并调用其 SaveGame
// 当游戏读档时同理触发 LoadGame → FinalSwizzle
// ============================================================
class DeployBuffSaveLoadHandler : public ECGameClass_
{
public:
	virtual void SaveGame(ECStreamWriter& stream) override
	{
		// DeployBuff 不需要额外的全局状态序列化
		// SITimerLeft、SICountLeft 等已由 WIC 框架自动存档
	}

	virtual void LoadGame(ECStreamReader& stream) override
	{
		// DeployBuff 不需要额外的全局状态反序列化
	}

	virtual size_t GetSizeMax() override
	{
		return 0; // 由 EC 框架管理大小
	}

	virtual void PointerGotInvalid(AbstractClass* const pObject, bool bRemoved) override
	{
		// 没有需要清理的全局指针
	}

	virtual void FinalSwizzle() override
	{
		// 没有需要重排的指针
	}

	virtual void Update() override
	{
		// 纯存档辅助对象，不需要每帧更新
	}
};

// 全局实例 — 构造时自动注册到 EC 框架的 GameClassList
static DeployBuffSaveLoadHandler g_saveLoadHandler;

// ============================================================
// Callbacks
// ============================================================

void OnFirstInit()
{
	// 注册对局结束清理回调
	ECListener::Listen_ClearScenario([]()
	{
		// 不需要清理全局状态
	});
}

void OnOrderedInit()
{
	// 注册自定义 Buff 效果类型（此时 WIC 已可用）
	SIClassManager::RegisterBuff<DeployBuffClass>("Deploy");

	// 可选：注册别名
	// SIClassManager::RegisterBuff<DeployBuffClass>("PeriodicDeploy");
}

// ============================================================
// DllMain
// ============================================================

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		// 初始化 EC/Syringe 框架
		if (!Init::Initialize())
			return FALSE;

		// 声明依赖：需要 WIC（SIWinterIsComing）
		InitDependency dep_WIC{
			"SIWinterIsComing",
			DoNotCheckVersion,
			WIC_LIBRARY_VERSION,
			true // 必需依赖
		};

		// 注册本库
		std::function<void()> fnFirst = OnFirstInit;
		std::function<void()> fnOrdered = OnOrderedInit;
		ECInitLibrary(
			"DeployBuff",          // 库名
			1,                      // 版本
			1,                      // 最低兼容版本
			(UTF8_CString)u8"WIC Deploy Buff - Periodic Unit Deploy Trigger",
			fnFirst,
			fnOrdered,
			{ dep_WIC }
		);
	}

	return TRUE;
}
