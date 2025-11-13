// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GS_RaceState.generated.h"

//  make this name unique for this header
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRaceStateEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRaceCountdownEvent);


USTRUCT(BlueprintType)
struct FRaceResultRow
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Race")
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly, Category = "Race")
	int32 RacePosition = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Race")
	bool bHasFinished = false;

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

	UPROPERTY(ReplicatedUsing = OnRep_RaceFlags, BlueprintReadOnly)
	bool bRaceRunning = false;

	UPROPERTY(ReplicatedUsing = OnRep_RaceFlags, BlueprintReadOnly)
	bool bRaceFinished = false;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 TotalLaps = 2;

	UPROPERTY(ReplicatedUsing = OnRep_Countdown, BlueprintReadOnly)
	double CountdownEndServerTime = 0.0;

	UPROPERTY(Replicated, BlueprintReadOnly)
	double RaceStartServerTime = 0.0;

	UPROPERTY(Replicated, BlueprintReadOnly)
	double RaceEndServerTime = 0.0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	TArray<FRaceResultRow> RaceResults;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	float FastestLapTimeSeconds = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	FString FastestLapPlayerName;


	// use the renamed delegate type
	UPROPERTY(BlueprintAssignable, Category = "Race|Events")
	FOnRaceStateEvent OnRaceStarted;

	UPROPERTY(BlueprintAssignable, Category = "Race|Events")
	FOnRaceStateEvent OnRaceFinished;

	UPROPERTY(BlueprintAssignable, Category = "Race|Events")
	FOnRaceCountdownEvent OnCountdownStarted;

	UFUNCTION() void OnRep_RaceFlags();
	UFUNCTION() void OnRep_Countdown();

	UFUNCTION(BlueprintPure)
	float GetCountdownSecondsRemaining() const;

	UFUNCTION(BlueprintPure)
	float GetElapsedRaceSeconds() const;

	UFUNCTION(BlueprintCallable, Category = "Race|Events")
	void NotifyCountdownStarted();

	UFUNCTION(BlueprintPure, Category = "Race|Time")
	bool IsCountdownActive() const { return GetCountdownSecondsRemaining() > 0.f && !bRaceRunning; }

	UFUNCTION(BlueprintCallable, Category = "Race")
	void RebuildRaceResultsFromPlayerStates();

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
