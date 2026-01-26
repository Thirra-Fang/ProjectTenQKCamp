#include "TestFPSStateTreeUtility.h"
#include "StateTreeExecutionContext.h"
#include "ShooterNPC.h"
#include "Camera/CameraComponent.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "ShooterAIController.h"
#include "StateTreeAsyncExecutionContext.h"

EStateTreeRunStatus FStateTreeTestFPSSenseEnemiesTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// 是否从另一个状态过渡而来？
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		// 获取实例数据
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
		

		// 将感知更新委托绑定到控制器上
		InstanceData.Controller->OnShooterPerceptionUpdated.BindLambda(
			[WeakContext = Context.MakeWeakExecutionContext()](AActor* SensedActor, const FAIStimulus& Stimulus)
			{
				UE_LOG(LogTemp,Error,TEXT("监测发生更新"));
				// 获取lambda内部的实例数据
				const FStateTreeStrongExecutionContext StrongContext = WeakContext.MakeStrongExecutionContext();
				if (FInstanceDataType* LambdaInstanceData = StrongContext.GetInstanceDataPtr<FInstanceDataType>())
				{
					if (SensedActor->ActorHasTag(LambdaInstanceData->SenseTag))
					{
						//加入感知到的Actor集
						LambdaInstanceData->PerceivedActors.Emplace(SensedActor);
						if (!LambdaInstanceData->bHasTarget)
						{
							// 设置调查地点
							LambdaInstanceData->InvestigateLocation = Stimulus.StimulusLocation;
							// 设置调查标志
							LambdaInstanceData->bHasInvestigateLocation = true;
						}
					}
				}
			}
		);

		// 将感知绑定到控制器上被遗忘的委托
		InstanceData.Controller->OnShooterPerceptionForgotten.BindLambda(
			[WeakContext = Context.MakeWeakExecutionContext()](AActor* SensedActor)
			{
				UE_LOG(LogTemp,Error,TEXT("监测已遗忘"));
				// 获取lambda内部的实例数据
				const FStateTreeStrongExecutionContext StrongContext = WeakContext.MakeStrongExecutionContext();
				if (FInstanceDataType* LambdaInstanceData = StrongContext.GetInstanceDataPtr<FInstanceDataType>())
				{
					LambdaInstanceData->PerceivedActors.Remove(SensedActor);
					// 我们是否忘记了当前的目标？
					if (SensedActor == LambdaInstanceData->TargetActor)
					{
						// 清除目标
						LambdaInstanceData->TargetActor = nullptr;
						// 清除标志
						LambdaInstanceData->bHasTarget = false;
						// 清除控制器上的目标
						LambdaInstanceData->Controller->ClearCurrentTarget();
						LambdaInstanceData->Controller->ClearFocus(EAIFocusPriority::Gameplay);
					}
				}
			}
		);
	}
	return EStateTreeRunStatus::Running;
}

void FStateTreeTestFPSSenseEnemiesTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// 是否已进入另一种状态？
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		// 获取实例数据
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
		// 解除感知委托
		InstanceData.Controller->OnShooterPerceptionUpdated.Unbind();
		InstanceData.Controller->OnShooterPerceptionForgotten.Unbind();
	}
}

EStateTreeRunStatus FStateTreeTestFPSSenseEnemiesTask::Tick(FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	//以下部分Tick检查是否能看到Actor
	if (!InstanceData.bHasTarget)
	{
		for (auto& actor:InstanceData.PerceivedActors)
		{
			FVector TargetDir=(actor->GetActorLocation()-InstanceData.Character->GetActorLocation()).GetSafeNormal();
			// 根据角色朝向与刺激方向之间的点积推断角度
			const float DirDot = FVector::DotProduct(TargetDir, InstanceData.Character->GetActorForwardVector());
			const float MaxDot = FMath::Cos(FMath::DegreesToRadians(InstanceData.DirectLineOfSightCone));
			// 是否在我们的感知锥范围内？
			if (DirDot >= MaxDot)
			{
				// 在角色与感知到的演员之间绘制一条线条
				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(InstanceData.Character);
				QueryParams.AddIgnoredActor(actor);
				FHitResult OutHit;
				// 检查我们是否与刺激源存在直接视线连接
				if (!InstanceData.Character->GetWorld()->LineTraceSingleByChannel(OutHit, InstanceData.Character->GetActorLocation(), actor->GetActorLocation(), ECC_Visibility, QueryParams))
				{
					// 设置控制器的目标
					InstanceData.Controller->SetCurrentTarget(actor);
					// 设置任务输出
					InstanceData.TargetActor = actor;
					// 设置标志
					InstanceData.bHasTarget = true;
					InstanceData.bHasInvestigateLocation = false;
				} 
			}
		}
	}
	
	return EStateTreeRunStatus::Running;
}


#if WITH_EDITOR
FText FStateTreeTestFPSSenseEnemiesTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	return FText::FromString(TEXT("<b>感知敌人</b>"));
}
#endif // WITH_EDITOR