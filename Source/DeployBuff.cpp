#include "DeployBuff.h"
#include <YRPP.h>
#include <GeneralDefinitions.h>
#include <MissionClass.h>
#include <FootClass.h>

// ============================================================
// EffectDataInit - 初始化计时器/计数器并缓存 Type 配置（只读一次）
// 存档不会重新读取 INI，故在创建时完成所有初始化
// ============================================================
void DeployBuffClass::EffectDataInit()
{
	// 直接从 Type 初始化运行时计数器（这些字段会自动存读档）
	SIDelayLeft = Type->SIEffect_Delay;
	SITimerLeft = Type->SIEffect_Timer;
	SICountLeft = Type->SIEffect_Counts.GetItem(0);
}

// ============================================================
// EffectAI - 每帧处理：SITimerLeft 倒计时 → 归零触发部署
// ============================================================
void DeployBuffClass::EffectAI(SIBuffClass_EffectData* effectData)
{
	// 仅在 Effect 状态（生效状态）下运行
	if (SIBuffState != SIBuffClass_State::生效)
		return;

	// 初始延迟倒计时：Delay 期间不执行部署
	if (SIDelayLeft > 0)
	{
		--SIDelayLeft;
		return;
	}

	// Timer 倒计时：每帧减 1，归零时触发部署
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

	// 重置 Timer：每隔 Effect.Timer 帧部署一次
	SITimerLeft = Type->SIEffect_Timer;
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
