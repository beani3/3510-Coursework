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

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> CountdownWidgetClass;

private:
	UPROPERTY() UUserWidget* PauseMenuWidget = nullptr;
	UPROPERTY() UUserWidget* CountdownWidget = nullptr;

	
	void ShowPauseMenu();
	void HidePauseMenu();

	UFUNCTION()
	void HandleCountdownStarted();
	UFUNCTION()
	void OnRaceStarted();
	UFUNCTION()
	void OnRaceFinished();
};
