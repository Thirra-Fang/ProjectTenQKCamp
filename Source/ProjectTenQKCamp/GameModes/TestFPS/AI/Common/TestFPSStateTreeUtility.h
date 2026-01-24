#pragma once
#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreeConditionBase.h"

#include "TestFPSStateTreeUtility.generated.h"

class AShooterNPC;
class AAIController;
class AShooterAIController;

USTRUCT()
struct FStateTreeTestSenseEnemiesInstanceData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AShooterAIController> Controller;
	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AShooterNPC> Character;
	UPROPERTY(EditAnywhere, Category = Output)
	TObjectPtr<AActor> TargetActor;
	UPROPERTY(EditAnywhere, Category = Output)
	FVector InvestigateLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Category = Output)
	bool bHasTarget = false;
	UPROPERTY(EditAnywhere, Category = Output)
	bool bHasInvestigateLocation = false;
	UPROPERTY(EditAnywhere, Category = Parameter)
	FName SenseTag = FName("Player");
	UPROPERTY(EditAnywhere, Category = Parameter)
	float DirectLineOfSightCone = 70.0f;
	UPROPERTY(EditAnywhere)
	TSet<TObjectPtr<AActor>> PerceivedActors;
};

USTRUCT(meta=(DisplayName="感知敌人", Category="TestFPS"))
struct FStateTreeTestFPSSenseEnemiesTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FStateTreeTestSenseEnemiesInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif // WITH_EDITOR
};

