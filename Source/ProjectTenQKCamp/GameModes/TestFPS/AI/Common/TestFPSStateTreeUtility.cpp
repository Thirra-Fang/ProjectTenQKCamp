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
	// have we transitioned from another state?
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		// get the instance data
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
		

		// bind the perception updated delegate on the controller
		InstanceData.Controller->OnShooterPerceptionUpdated.BindLambda(
			[WeakContext = Context.MakeWeakExecutionContext()](AActor* SensedActor, const FAIStimulus& Stimulus)
			{
				UE_LOG(LogTemp,Error,TEXT("监测发生更新"));
				// get the instance data inside the lambda
				const FStateTreeStrongExecutionContext StrongContext = WeakContext.MakeStrongExecutionContext();
				if (FInstanceDataType* LambdaInstanceData = StrongContext.GetInstanceDataPtr<FInstanceDataType>())
				{
					if (SensedActor->ActorHasTag(LambdaInstanceData->SenseTag))
					{
						bool bDirectLOS = false;

						// calculate the direction of the stimulus
						const FVector StimulusDir = (Stimulus.StimulusLocation - LambdaInstanceData->Character->GetActorLocation()).GetSafeNormal();

						// infer the angle from the dot product between the character facing and the stimulus direction
						const float DirDot = FVector::DotProduct(StimulusDir, LambdaInstanceData->Character->GetActorForwardVector());
						const float MaxDot = FMath::Cos(FMath::DegreesToRadians(LambdaInstanceData->DirectLineOfSightCone));

						// is the direction within our perception cone?
						if (DirDot >= MaxDot)
						{
							// run a line trace between the character and the sensed actor
							FCollisionQueryParams QueryParams;
							QueryParams.AddIgnoredActor(LambdaInstanceData->Character);
							QueryParams.AddIgnoredActor(SensedActor);

							FHitResult OutHit;

							// we have direct line of sight if this trace is unobstructed
							bDirectLOS = !LambdaInstanceData->Character->GetWorld()->LineTraceSingleByChannel(OutHit, LambdaInstanceData->Character->GetActorLocation(), SensedActor->GetActorLocation(), ECC_Visibility, QueryParams);

						}

						// check if we have a direct line of sight to the stimulus
						if (bDirectLOS)
						{
							// set the controller's target
							LambdaInstanceData->Controller->SetCurrentTarget(SensedActor);

							// set the task output
							LambdaInstanceData->TargetActor = SensedActor;

							// set the flags
							LambdaInstanceData->bHasTarget = true;
							LambdaInstanceData->bHasInvestigateLocation = false;

						// no direct line of sight to target
						} else {

							// if we already have a target, ignore the partial sense and keep on them
							if (!IsValid(LambdaInstanceData->TargetActor))
							{
								// is this stimulus stronger than the last one we had?
								if (Stimulus.Strength > LambdaInstanceData->LastStimulusStrength)
								{
									// update the stimulus strength
									LambdaInstanceData->LastStimulusStrength = Stimulus.Strength;

									// set the investigate location
									LambdaInstanceData->InvestigateLocation = Stimulus.StimulusLocation;

									// set the investigate flag
									LambdaInstanceData->bHasInvestigateLocation = true;
								}
							}
						}
					}
				}
			}
		);

		// bind the perception forgotten delegate on the controller
		InstanceData.Controller->OnShooterPerceptionForgotten.BindLambda(
			[WeakContext = Context.MakeWeakExecutionContext()](AActor* SensedActor)
			{
				UE_LOG(LogTemp,Error,TEXT("监测已遗忘"));
				// get the instance data inside the lambda
				const FStateTreeStrongExecutionContext StrongContext = WeakContext.MakeStrongExecutionContext();
				if (FInstanceDataType* LambdaInstanceData = StrongContext.GetInstanceDataPtr<FInstanceDataType>())
				{
					bool bForget = false;

					// are we forgetting the current target?
					if (SensedActor == LambdaInstanceData->TargetActor)
					{
						bForget = true;
					}
					else 
					{
						// are we forgetting about a partial sense?
						if (!IsValid(LambdaInstanceData->TargetActor))
						{
							bForget = true;
						}
					}

					if (bForget)
					{
						// clear the target
						LambdaInstanceData->TargetActor = nullptr;

						// clear the flags
						LambdaInstanceData->bHasInvestigateLocation = false;
						LambdaInstanceData->bHasTarget = false;

						// reset the stimulus strength
						LambdaInstanceData->LastStimulusStrength = 0.0f;

						// clear the target on the controller
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
	// have we transitioned to another state?
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		// get the instance data
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

		// unbind the perception delegates
		InstanceData.Controller->OnShooterPerceptionUpdated.Unbind();
		InstanceData.Controller->OnShooterPerceptionForgotten.Unbind();
	}
}

EStateTreeRunStatus FStateTreeTestFPSSenseEnemiesTask::Tick(FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	DrawDebugLine(
			InstanceData.Character->GetWorld(),
			InstanceData.Character->GetActorLocation(),
			InstanceData.Character->GetActorLocation()+500.0f*InstanceData.Character->GetActorForwardVector(),
			FColor::Red,
			false,
			-1.0f,
			10,
			5.0f
		);
	if (InstanceData.bHasTarget)
	{
		DrawDebugLine(
			InstanceData.Character->GetWorld(),
			InstanceData.Character->GetActorLocation(),
			InstanceData.TargetActor->GetActorLocation(),
			FColor::Green,
			false,
			-1.0f,
			10,
			5.0f
		);
	}
	if (InstanceData.bHasInvestigateLocation)
	{
		DrawDebugLine(
			InstanceData.Character->GetWorld(),
			InstanceData.Character->GetActorLocation(),
			InstanceData.InvestigateLocation,
			FColor::Yellow,
			false,
			-1.0f,
			10,
			5.0f
		);
	}
	DrawDebugCone(
		InstanceData.Character->GetWorld(),
		InstanceData.Character->GetActorLocation(),
		InstanceData.Character->GetActorForwardVector(),
		500.0f,
		FMath::DegreesToRadians(InstanceData.DirectLineOfSightCone),
		FMath::DegreesToRadians(InstanceData.DirectLineOfSightCone),
		16,
		FColor::Red,
		false,
		-1.0f,
		10,
		1.0f
		);
	return EStateTreeRunStatus::Running;
}


#if WITH_EDITOR
FText FStateTreeTestFPSSenseEnemiesTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting /*= EStateTreeNodeFormatting::Text*/) const
{
	return FText::FromString(TEXT("<b>感知敌人</b>"));
}
#endif // WITH_EDITOR