// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Menu/MenuGameModeBase.h"

#include "Kismet/GameplayStatics.h"

void AMenuGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	MenuUI = CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), MenuUIClass);
	MenuUI->AddToViewport(0);
}
