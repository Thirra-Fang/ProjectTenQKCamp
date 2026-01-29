// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Menu/UI/MainMenuUI.h"

#include "Kismet/GameplayStatics.h"

void UMainMenuUI::BP_EnterTestLevel_1()
{
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraFade(0.f,1.f,1.f,FLinearColor::Black,true,true);
	FTimerHandle timer;
	GetWorld()->GetTimerManager().SetTimer(timer,[this]()
	{
		UGameplayStatics::OpenLevel(this,FName("Lvl_TestLevel_1"));
	},1.0f,false);
}
