#include "DeployBuff.h"
#include <YRPP.h>
#include <GeneralDefinitions.h>
#include <MissionClass.h>
#include <FootClass.h>

// ============================================================
// OnEnterState_Active - 进入生效状态时初始化运行时计数器
// 每当 Buff 进入「生效」状态时重置 Delay/Count，
// 确保部署周期从进入生效状态开始计算。
// ============================================================
void DeployBuffClass::OnEnterState_Active()
{
	// 直接从 Type 初始化运行时计数器（这些字段会自动存读档）
	SITimerLeft = Type->SIEffect_Delay;
	// 当 Counts 为空列表时默认 -1（无限）
	SICountLeft = Type->SIEffect_Counts.GetItem(0);
}

// ============================================================
// EffectAI - 每帧处理：Delay 倒计时 → 归零触发部署
// ============================================================
void DeployBuffClass::EffectAI(SIBuffClass_EffectData* effectData)
{
	// Type 已被清除（单位已销毁等），安全退出
	if (!Type)
		return;

	// Delay 倒计时：每帧减 1，归零时触发部署
	if (SITimerLeft > 0)
	{
		--SITimerLeft;
		return;
	}

	// SITimerLeft == 0 → 执行部署动作
	DoDeploy();

	// 递减剩余次数（-1 表示无限，不减）
	if (SICountLeft > 0)
		--SICountLeft;

	// 次数耗尽 → 进入结束状态
	if (SICountLeft == 0)
	{
		EnterState(SIBuffClass_State::结束);
		return;
	}

	// 重置 Delay：每隔 Effect.Delay 帧部署一次
	SITimerLeft = Type->SIEffect_Delay;
}

// ============================================================
// DoDeploy - 触发单位的 Deploy 能力
// ============================================================
void DeployBuffClass::DoDeploy()
{
	TechnoClass* owner = GetOwnerTechno();
	if (!owner)
		return;

	// 对于 DeployFire 单位，Force Unload 使命触发部署开火
	// 对于其他单位，Force Unload 不会产生有害效果
	owner->ForceMission(Mission::Unload);
}
