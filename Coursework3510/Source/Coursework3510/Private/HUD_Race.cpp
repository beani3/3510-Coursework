// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD_Race.h"
#include "Blueprint/UserWidget.h"

void AHUD_Race::BeginPlay()
{
	Super::BeginPlay();

	if (MainHUDClass && !MainHUD)
	{
		MainHUD = CreateWidget<UUserWidget>(GetWorld(), MainHUDClass);
		if (MainHUD)
		{
			MainHUD->AddToViewport(10); 
			MainHUD->SetVisibility(ESlateVisibility::Visible);
			
			
		}
	}
}

void AHUD_Race::SetHUDVisible(bool bVisible)
{
	if (!MainHUD) return;
	MainHUD->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible
		: ESlateVisibility::Hidden);
}
