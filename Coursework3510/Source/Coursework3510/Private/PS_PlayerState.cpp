// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PlayerState.h"
#include "Net/UnrealNetwork.h"

void APS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS_PlayerState, CurrentLap);
	DOREPLIFETIME(APS_PlayerState, CurrentCheckpoint);
	DOREPLIFETIME(APS_PlayerState, LapProgress01);
	DOREPLIFETIME(APS_PlayerState, RacePosition);
	DOREPLIFETIME(APS_PlayerState, bHasFinished);
	DOREPLIFETIME(APS_PlayerState, FinishTimeSeconds);
	DOREPLIFETIME(APS_PlayerState, BestLapTimeSeconds);
	DOREPLIFETIME(APS_PlayerState, LastLapTimeSeconds);
}
void APS_PlayerState::RegisterLapTime(float LapTimeSeconds)
{
	LastLapTimeSeconds = LapTimeSeconds;
	LapTimesSeconds.Add(LapTimeSeconds);

	if (BestLapTimeSeconds <= 0.f || LapTimeSeconds < BestLapTimeSeconds)
	{
		BestLapTimeSeconds = LapTimeSeconds;
	}
}