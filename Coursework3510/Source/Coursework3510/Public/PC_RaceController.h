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


	UFUNCTION(BlueprintCallable)
	void ResumeGame();


protected:
	virtual void BeginPlay() override;



	// Handle paused state change
	UFUNCTION(Server, Reliable)
	void ServerSetPaused(bool bPause, APlayerController* InstigatorPC);

	// Multicast paused state change
	UFUNCTION(NetMulticast, Reliable)
	void MulticastApplyPaused(bool bPause, APlayerController* InstigatorPC);

	//pause menu widget
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	//pause menu for other players
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> OtherPlayerPauseWidgetClass;

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
	UPROPERTY() UUserWidget* OtherPlayerPauseWidget = nullptr;

	//show/hide pause menu
	void ShowPauseMenu();
	void HidePauseMenu();
	
	//show/hide other player paused widget
	void ShowOtherPlayerPausedWidget();
	void HideOtherPlayerPausedWidget();


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

