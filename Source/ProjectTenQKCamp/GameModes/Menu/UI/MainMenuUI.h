// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENQKCAMP_API UMainMenuUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,Category="切换Level",DisplayName="打开测试场景")
	void BP_EnterTestLevel_1();
};
