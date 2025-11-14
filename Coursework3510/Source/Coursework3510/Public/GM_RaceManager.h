

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_RaceManager.generated.h"

// 
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRaceManagerEvent);

class APS_PlayerState;

UCLASS()
class COURSEWORK3510_API AGM_RaceManager : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Delegate for when the race starts
	UPROPERTY(BlueprintAssignable, Category = "Race|Events")
	FOnRaceManagerEvent OnStarted;

	// Delegate for when the race finishes
	UPROPERTY(BlueprintAssignable, Category = "Race|Events")
	FOnRaceManagerEvent OnFinished;

	// Start the race with a countdown of specified seconds
	UFUNCTION(BlueprintCallable, Category = "Race")
	void StartRaceWithCountdown(float Seconds);

	// Force race to end after an courtesy time
	UFUNCTION(BlueprintCallable, Category = "Race")
	void ForceFinishRace(float CourtesySeconds);

	// Restart the race
	UFUNCTION(BlueprintCallable, Category = "Race")
	void RestartRace();

	// Called by cars when a player has completed all laps
	UFUNCTION(BlueprintCallable, Category = "Race")
	void NotifyPlayerFinished(APS_PlayerState* FinishedPlayer);

	// Configuration of lap number
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race|Config")
	int32 Config_TotalLaps = 2;
	// Configuration of courtesy seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race|Config")
	float Config_CourtesySeconds = 15.f;

protected:
	void HandleCountdownFinished();
	void HandleCourtesyEnd();

private:
	// Timer handles
	FTimerHandle TH_CountdownDone;
	// Timer handle for courtesy period
	FTimerHandle TH_CourtesyEnd;
	// Whether courtesy period has started or not
	bool bCourtesyStarted = false;
};
