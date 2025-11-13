

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_RaceManager.generated.h"

// different name to avoid duplicate symbol
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRaceManagerEvent);

class APS_PlayerState;

UCLASS()
class COURSEWORK3510_API AGM_RaceManager : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Race|Events")
	FOnRaceManagerEvent OnStarted;

	UPROPERTY(BlueprintAssignable, Category = "Race|Events")
	FOnRaceManagerEvent OnFinished;

	// Start a race with a shared countdown for all players
	UFUNCTION(BlueprintCallable, Category = "Race")
	void StartRaceWithCountdown(float Seconds);

	// Force race to end after an optional courtesy time
	UFUNCTION(BlueprintCallable, Category = "Race")
	void ForceFinishRace(float CourtesySeconds);

	// Reset all race state (doesn't auto-start)
	UFUNCTION(BlueprintCallable, Category = "Race")
	void RestartRace();

	// Called by cars when a player has completed all laps
	UFUNCTION(BlueprintCallable, Category = "Race")
	void NotifyPlayerFinished(APS_PlayerState* FinishedPlayer);

protected:
	void HandleCountdownFinished();
	void HandleCourtesyEnd();

private:
	FTimerHandle TH_CountdownDone;
	FTimerHandle TH_CourtesyEnd;

	// Config – you can expose these later if you want
	int32 Config_TotalLaps = 2;
	float Config_CourtesySeconds = 15.f;

	// So only the first finisher starts the courtesy timer
	bool bCourtesyStarted = false;
};
