// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_RaceManager.generated.h"

UCLASS()
class COURSEWORK3510_API AGM_RaceManager : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGM_RaceManager();

	// Number of laps required to finish (player lap counting is 1-based in your pawn)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	int32 TotalLaps = 4;

	// Elapsed race time in seconds (updated while race running)
	UPROPERTY(BlueprintReadOnly, Category = "Race")
	float ElapsedTime = 0.f;

	// Race state
	UPROPERTY(BlueprintReadOnly, Category = "Race")
	bool bRaceRunning = false;

	UPROPERTY(BlueprintReadOnly, Category = "Race")
	bool bRaceFinished = false;

	// Optional: maximum allowed race duration in seconds (0 = disabled)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	float MaxRaceTime = 0.f;

	// After first finisher, how many seconds before forcing race end
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	float CourtesyTime = 10.f;

	// Players bookkeeping
	UPROPERTY(BlueprintReadOnly, Category = "Race")
	int32 TotalPlayers = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Race")
	int32 PlayersFinished = 0;

	// API
	UFUNCTION(BlueprintCallable, Category = "Race")
	void StartRace();

	UFUNCTION(BlueprintCallable, Category = "Race")
	void StartRaceWithCountdown(float CountdownSeconds);

	UFUNCTION(BlueprintCallable, Category = "Race")
	void NotifyPlayerFinished();

	UFUNCTION(BlueprintCallable, Category = "Race")
	void EndRace();


	UFUNCTION(BlueprintPure, Category = "Race|UI")
	float GetCountdownRemaining() const;

	// Blueprint hooks
	UFUNCTION(BlueprintImplementableEvent, Category = "Race")
	void BP_OnRaceStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Race")
	void BP_OnRaceEnded();

	UFUNCTION(BlueprintImplementableEvent, Category = "Race")
	void BP_OnPlayerFinished(int32 PlayerFinishOrder, float FinishTime);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FTimerHandle TimerHandle_Tick;
	FTimerHandle TimerHandle_RaceDuration;
	FTimerHandle TimerHandle_Countdown;
	FTimerHandle TimerHandle_Courtesy;

	// internal tick to advance ElapsedTime
	void TickTimer();

	// timers callbacks
	void OnRaceDurationExpired();
	void OnCourtesyExpired();
	void OnCountdownComplete();

	// internal finish helper
	void FinishRaceInternal();
};
