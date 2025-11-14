// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GS_RaceState.generated.h"

// Delegate of race start
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRaceStateEvent);
// Delegate of race countdown start
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRaceCountdownEvent);


USTRUCT(BlueprintType)
struct FRaceResultRow
{
	GENERATED_BODY()

	// Name of the player
	UPROPERTY(BlueprintReadOnly, Category = "Race")
	FString PlayerName;

	// Final position
	UPROPERTY(BlueprintReadOnly, Category = "Race")
	int32 RacePosition = 0;

	// Number of completed laps
	UPROPERTY(BlueprintReadOnly, Category = "Race")
	bool bHasFinished = false;

	// Total time taken to finish the race
	UPROPERTY(BlueprintReadOnly, Category = "Race")
	float FinishTimeSeconds = 0.f;

	// Best single-lap time for this racer
	UPROPERTY(BlueprintReadOnly, Category = "Race")
	float BestLapTimeSeconds = 0.f;
};




UCLASS()
class COURSEWORK3510_API AGS_RaceState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AGS_RaceState();

	// is the race currently running?
	UPROPERTY(ReplicatedUsing = OnRep_RaceFlags, BlueprintReadOnly)
	bool bRaceRunning = false;

	// has the race finished?
	UPROPERTY(ReplicatedUsing = OnRep_RaceFlags, BlueprintReadOnly)
	bool bRaceFinished = false;

	// total number of laps in the race set by race manager
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 TotalLaps = 2;

	// server time when countdown ends (0 = no countdown)
	UPROPERTY(ReplicatedUsing = OnRep_Countdown, BlueprintReadOnly)
	double CountdownEndServerTime = 0.0;

	// servertime when race started
	UPROPERTY(Replicated, BlueprintReadOnly)
	double RaceStartServerTime = 0.0;

	// servertime when race ended
	UPROPERTY(Replicated, BlueprintReadOnly)
	double RaceEndServerTime = 0.0;


	// race results array
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	TArray<FRaceResultRow> RaceResults;

	// fastest lap info 
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	float FastestLapTimeSeconds = 0.f;

	// player name of fastest lap
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	FString FastestLapPlayerName;


	
	UPROPERTY(BlueprintAssignable, Category = "Race|Events")
	FOnRaceStateEvent OnRaceStarted;

	UPROPERTY(BlueprintAssignable, Category = "Race|Events")
	FOnRaceStateEvent OnRaceFinished;

	UPROPERTY(BlueprintAssignable, Category = "Race|Events")
	FOnRaceCountdownEvent OnCountdownStarted;

	UFUNCTION() void OnRep_RaceFlags();
	UFUNCTION() void OnRep_Countdown();

	// get countdown seconds remaining
	UFUNCTION(BlueprintPure)
	float GetCountdownSecondsRemaining() const;


	// get elapsed race time
	UFUNCTION(BlueprintPure)
	float GetElapsedRaceSeconds() const;

	// notify countdown started
	UFUNCTION(BlueprintCallable, Category = "Race|Events")
	void NotifyCountdownStarted();

	// is countdown active?
	UFUNCTION(BlueprintPure, Category = "Race|Time")
	bool IsCountdownActive() const { return GetCountdownSecondsRemaining() > 0.f && !bRaceRunning; }

	// rebuild race results from player states
	UFUNCTION(BlueprintCallable, Category = "Race")
	void RebuildRaceResultsFromPlayerStates();


	//get race results array and fastest lap info 
	UFUNCTION(BlueprintPure, Category = "Race")
	void GetRaceResults(
		TArray<FRaceResultRow>& OutResults,
		float& OutFastestLapTime,
		FString& OutFastestLapPlayer) const
	{
		OutResults          = RaceResults;
		OutFastestLapTime   = FastestLapTimeSeconds;
		OutFastestLapPlayer = FastestLapPlayerName;
	}



	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

private:
	bool bPrevRaceRunning = false;
	bool bPrevRaceFinished = false;
	bool bPrevCountdownActive = false;
};
