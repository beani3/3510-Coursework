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

	// Request to set paused state
	UFUNCTION(BlueprintCallable, Category = "Race|UI")
	void RequestSetPaused(bool bPause);

	// Show immediate win screen (for players who did not finish the race)
	UFUNCTION(Client, Reliable)
	void ClientShowImmediateWinScreen();

protected:
	virtual void BeginPlay() override;

	// Handle paused state change
	UFUNCTION(Server, Reliable)
	void ServerSetPaused(bool bPause);

	// Apply paused state on all clients
	UFUNCTION(NetMulticast, Reliable)
	void MulticastApplyPaused(bool bPause);	

	//pause menu widget
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	//countdown widget
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> CountdownWidgetClass;
	
	// win screen widget
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> WinScreenWidgetClass;

private:
	UPROPERTY() UUserWidget* PauseMenuWidget = nullptr;
	UPROPERTY() UUserWidget* CountdownWidget = nullptr;
	UPROPERTY() UUserWidget* WinScreenWidget = nullptr;

	//show/hide pause menu
	void ShowPauseMenu();
	void HidePauseMenu();

	//countdown started
	UFUNCTION()
	void HandleCountdownStarted();
	 
	//race started
	UFUNCTION()
	void OnRaceStarted();

	//	race finished
	UFUNCTION()
	void OnRaceFinished();



};

