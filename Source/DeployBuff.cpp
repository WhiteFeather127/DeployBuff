#include "DeployBuff.h"
#include "Debug.h"
#include <YRPP.h>
#include <GeneralDefinitions.h>
#include <MissionClass.h>
#include <FootClass.h>
#include <MessageListClass.h>

// ============================================================
// OnEnterState_Active - 进入生效状态时初始化运行时计数器
// 每当 Buff 进入「生效」状态时重置 Delay/Count，
// 确保部署周期从进入生效状态开始计算。
// ============================================================
void DeployBuffClass::OnEnterState_Active()
{
	DEBUG_LOG("[Deploy] %s: enter\n", __FUNCTION__);

	// 直接从 Type 初始化运行时计数器（这些字段会自动存读档）
	SITimerLeft = Type->SIEffect_Delay;
	// 当 Counts 为空列表时默认 -1（无限）
	SICountLeft = Type->SIEffect_Counts.GetItem(0);

	DEBUG_LOG("[Deploy] %s: SITimerLeft=%d  SICountLeft=%d\n",
		__FUNCTION__, SITimerLeft, SICountLeft);
}

// ============================================================
// EffectAI - 每帧处理：Delay 倒计时 → 归零触发部署
// ============================================================
void DeployBuffClass::EffectAI(SIBuffClass_EffectData* effectData)
{
	// Type 已被清除（单位已销毁等），安全退出
	if (!Type)
	{
		DEBUG_LOG("[Deploy] %s: Type==null, return\n", __FUNCTION__);
		return;
	}

	// Delay 倒计时：每帧减 1，归零时触发部署
	if (SITimerLeft > 0)
	{
		--SITimerLeft;
		DEBUG_LOG("[Deploy] %s: timer-- -> %d\n", __FUNCTION__, SITimerLeft);
		if (SITimerLeft > 0)
			return;
	}

	// SITimerLeft == 0 → 执行部署动作
	DEBUG_LOG("[Deploy] %s: timer elapsed, deploying\n", __FUNCTION__);
	DoDeploy();

	// 在游戏聊天框输出日志
	{
		wchar_t msg[256];
		TechnoClass* owner = GetOwnerTechno();
		const wchar_t* unitName = owner ? owner->GetUIName() : L"Unknown";
		swprintf(msg, L"[DeployBuff] %ls - Deploy! Count=%d",
			unitName, SICountLeft > 0 ? SICountLeft : -1);
		MessageListClass::Instance.PrintMessage(msg, 150);
	}

	// 递减剩余次数（-1 表示无限，不减）
	if (SICountLeft > 0)
		--SICountLeft;

	// 次数耗尽 → 进入结束状态
	if (SICountLeft == 0)
	{
		DEBUG_LOG("[Deploy] %s: count exhausted, enter End state\n", __FUNCTION__);
		EnterState(SIBuffClass_State::结束);

		// 输出结束日志
		wchar_t msg[256];
		TechnoClass* owner = GetOwnerTechno();
		const wchar_t* unitName = owner ? owner->GetUIName() : L"Unknown";
		swprintf(msg, L"[DeployBuff] %ls - Finished!", unitName);
		MessageListClass::Instance.PrintMessage(msg, 150);

		return;
	}

	// 重置 Delay：每隔 Effect.Delay 帧部署一次
	SITimerLeft = Type->SIEffect_Delay;
	DEBUG_LOG("[Deploy] %s: reset timer to %d\n", __FUNCTION__, SITimerLeft);
}

// ============================================================
// DoDeploy - 触发单位的 Deploy 能力
// ============================================================
void DeployBuffClass::DoDeploy()
{
	TechnoClass* owner = GetOwnerTechno();
	if (!owner)
	{
		DEBUG_LOG("[Deploy] %s: owner==null\n", __FUNCTION__);
		return;
	}

	DEBUG_LOG("[Deploy] %s: deploying unit=%p\n", __FUNCTION__, (void*)owner);

	// 对于 DeployFire 单位，Force Unload 使命触发部署开火
	// 对于其他单位，Force Unload 不会产生有害效果
	owner->ForceMission(Mission::Unload);
}
