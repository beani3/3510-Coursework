// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PS_PlayerState.generated.h"

UCLASS()
class COURSEWORK3510_API APS_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	// Current lap number
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	int32 CurrentLap = 1;
	// Current checkpoint index
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	int32 CurrentCheckpoint = 0;

	// 0..1 current lap progress
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	float LapProgress01 = 0.f;

	// Current race position (1 = leading)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	int32 RacePosition = 1;

	// Has the player finished
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	bool bHasFinished = false;

	// Time from race start to finish for this player 
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	float FinishTimeSeconds = 0.f;

	// Best single-lap time for this player
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	float BestLapTimeSeconds = 0.f;

	// Time of the most recently completed lap for this player
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	float LastLapTimeSeconds = 0.f;

	// All lap times for this player 
	UPROPERTY()
	TArray<float> LapTimesSeconds;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void RegisterLapTime(float LapTimeSeconds);
};
