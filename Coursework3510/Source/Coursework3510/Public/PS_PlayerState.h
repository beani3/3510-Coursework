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
	// === Replicated race data ===
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	int32 CurrentLap = 1;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	int32 CurrentCheckpoint = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	float LapProgress01 = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	int32 RacePosition = 1;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	bool bHasFinished = false;

	// Time from race start to finish for this player (seconds)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Race")
	float FinishTimeSeconds = 0.f;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
