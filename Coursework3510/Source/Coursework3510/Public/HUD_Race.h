// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUD_Race.generated.h"

class UUserWidget;
UCLASS()
class COURSEWORK3510_API AHUD_Race : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainHUDClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* MainHUD = nullptr;

	// NEW: control HUD visibility (called by PlayerController)
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetHUDVisible(bool bVisible);

protected:
	virtual void BeginPlay() override;
};

