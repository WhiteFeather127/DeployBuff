#pragma once
#include <WIC.h>

/*
 * DeployBuffClass - 周期部署 Buff
 *
 * Effect.Type=DeployBuff
 * Effect.Timer=120          ; 部署间隔（帧），默认 0
 * Effect.Counts=-1          ; 部署次数，-1 = 无限，默认 -1
 *
 * 行为：
 *   进入 Effect 状态后，按照 Effect.Timer 指定的间隔周期性
 *   触发单位的 Deploy 能力（ForceMission(Mission::Unload)），
 *   共触发 Effect.Counts 次后进入结束状态。
 *   Duration 属性控制 Buff 总持续时间（帧），到期自动移除。
 *
 * 适用于：
 *   - DeployFire=yes 的单位 → 周期发射部署武器
 *   - 步兵部署单位（GI等） → 周期执行部署动画/切换武器
 *
 * 注意：
 *   - 如果单位没有部署能力，ForceMission 不会产生效果
 *   - 部署间隔建议不小于 30 帧（约 1 秒），避免性能问题
 */
class DeployBuffClass final : public SIBuffClass
{
public:
	DeployBuffClass() : SIBuffClass() {}
	~DeployBuffClass() = default;

	// 从 Type 定义读取配置参数
	virtual void EffectDataInit() override;

	// 进入生效状态时初始化计时器和计数器
	virtual void OnEnterState_Effect() override;

	// 每帧处理：倒计时 → 到期触发部署 → 递减次数
	virtual void EffectAI(SIBuffClass_EffectData* effectData) override;

private:
	// 执行部署动作：调用单位的 ForceMission(Mission::Unload)
	void DoDeploy();

	// 部署间隔（帧），从 Type->SIEffect_Timer 读取
	int GetDeployInterval() const
	{
		if (!Type || Type->SIEffect_Timer < 0)
			return 0;
		return Type->SIEffect_Timer;
	}

	// 最大部署次数，从 Type->SIEffect_Counts[0] 读取
	int GetMaxDeployCount() const
	{
		return Type ? Type->SIEffect_Counts.TryGetItem(0, -1) : -1;
	}
};
