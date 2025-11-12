// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PC_RaceController.generated.h"

class UUserWidget;
class AGS_RaceState;

UCLASS()
class COURSEWORK3510_API APC_RaceController : public APlayerController
{
	GENERATED_BODY()

public:
	APC_RaceController();

	/** Call this from input (ESC / Start button etc.) */
	UFUNCTION(BlueprintCallable, Category = "Race|UI")
	void RequestSetPaused(bool bPause);

protected:
	virtual void BeginPlay() override;

	/* ===== Networking ===== */
	UFUNCTION(Server, Reliable)
	void ServerSetPaused(bool bPause);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastApplyPaused(bool bPause);

	/* ===== UI ===== */
	/** Pause menu widget class (assign in BP/Defaults) */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	/** Countdown widget class if you’re also using it here (optional) */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> CountdownWidgetClass;

private:
	UPROPERTY() UUserWidget* PauseMenuWidget = nullptr;
	UPROPERTY() UUserWidget* CountdownWidget = nullptr;

	/* Helpers to open/close pause menu consistently */
	void ShowPauseMenu();
	void HidePauseMenu();

	/* Optional: Race state hooks if you want to hide countdown on start/finish */
	UFUNCTION()
	void OnRaceStarted();
	UFUNCTION()
	void OnRaceFinished();
};
