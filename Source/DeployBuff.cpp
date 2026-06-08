#include "DeployBuff.h"
#include <YRPP.h>
#include <GeneralDefinitions.h>
#include <MissionClass.h>
#include <FootClass.h>

// ============================================================
// EffectDataInit - 从 Type 定义读取配置参数（只读一次）
// ============================================================
void DeployBuffClass::EffectDataInit()
{
	// 此处不做缓存操作，运行时直接从 Type 读取 Effect.Timer 和 Effect.Counts
	// 这样确保存档/读档后仍然使用最新的 Type 定义值
}

// ============================================================
// OnEnterState_Effect - 进入生效状态时初始化计时器和计数器
// ============================================================
void DeployBuffClass::OnEnterState_Effect()
{
	// 初始化部署倒计时 = Effect.Timer（帧）
	SITimerLeft = GetDeployInterval();

	// 初始化剩余部署次数 = Effect.Counts
	SICountLeft = GetMaxDeployCount();
}

// ============================================================
// EffectAI - 每帧处理：SITimerLeft 倒计时 → 归零触发部署
// ============================================================
void DeployBuffClass::EffectAI(SIBuffClass_EffectData* effectData)
{
	// 仅在 Effect 状态（生效状态）下运行
	if (SIBuffState != SIBuffClass_State::生效)
		return;

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
	// Timer=0 时至少间隔 1 帧，避免每帧都触发部署
	int interval = GetDeployInterval();
	SITimerLeft = interval > 0 ? interval : 1;
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
