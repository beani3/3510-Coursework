

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_RaceManager.generated.h"

// different name to avoid duplicate symbol
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRaceManagerEvent);

UCLASS()
class COURSEWORK3510_API AGM_RaceManager : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Race|Events")
	FOnRaceManagerEvent OnStarted;

	UPROPERTY(BlueprintAssignable, Category = "Race|Events")
	FOnRaceManagerEvent OnFinished;

	UFUNCTION(BlueprintCallable)
	void StartRaceWithCountdown(float Seconds);

	UFUNCTION(BlueprintCallable)
	void ForceFinishRace(float CourtesySeconds);

	UFUNCTION(BlueprintCallable)
	void RestartRace();

protected:
	void HandleCountdownFinished();
	void HandleCourtesyEnd();

private:
	FTimerHandle TH_CountdownDone;
	FTimerHandle TH_CourtesyEnd;

	int32 Config_TotalLaps = 2;
	float Config_CourtesySeconds = 15.f;
};
