// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UI/MainMenuUI.h"
#include "MenuGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENQKCAMP_API AMenuGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere,Category="菜单UI")
	TSubclassOf<UUserWidget> MenuUIClass;
	TObjectPtr<UUserWidget> MenuUI;
	
	virtual void BeginPlay() override;
	
};
