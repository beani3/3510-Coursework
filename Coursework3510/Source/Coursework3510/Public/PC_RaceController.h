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

	// Called by pawn or UI to request pause/unpause
	UFUNCTION(BlueprintCallable, Category = "Race|UI")
	void RequestSetPaused(bool bPause);

	// Show immediate win screen (for players who did not finish the race)
	UFUNCTION(Client, Reliable)
	void ClientShowImmediateWinScreen();

	UFUNCTION(BlueprintCallable, Category = "Race|UI")
	void ResumeGame();

protected:
	virtual void BeginPlay() override;

	// Server: apply pause state and notify all players
	UFUNCTION(Server, Reliable)
	void ServerSetPaused(bool bPause);

	// Client: apply pause visuals for this player
	UFUNCTION(Client, Reliable)
	void ClientApplyPaused(bool bPause, bool bIsInstigator);

	// Main pause menu for the pausing player
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	// Overlay that *everyone* sees when the game is paused
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PausedOverlayWidgetClass;

	// Countdown widget
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> CountdownWidgetClass;

	// Win screen widget
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> WinScreenWidgetClass;

private:
	UPROPERTY() UUserWidget* PauseMenuWidget = nullptr;
	UPROPERTY() UUserWidget* CountdownWidget = nullptr;
	UPROPERTY() UUserWidget* WinScreenWidget = nullptr;
	UPROPERTY() UUserWidget* PausedOverlayWidget = nullptr;

	// Pause menu for instigator
	void ShowPauseMenu();
	void HidePauseMenu();

	// Overlay for everyone
	void ShowPausedOverlay();
	void HidePausedOverlay();

	// Countdown started
	UFUNCTION()
	void HandleCountdownStarted();

	// Race started
	UFUNCTION()
	void OnRaceStarted();

	// Race finished
	UFUNCTION()
	void OnRaceFinished();
};
