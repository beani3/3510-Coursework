// Fill out your copyright notice in the Description page of Project Settings.



#include "AC_PointsComponent.h"
#include "Net/UnrealNetwork.h"

// Points component added to actors to manage scoring
UAC_PointsComponent::UAC_PointsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	  SetIsReplicatedByDefault(true);
}

void UAC_PointsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAC_PointsComponent, Score); // Replicate Score variable
}

void UAC_PointsComponent::OnRep_Score(int32 OldScore)   // Called on clients when Score is replicated
{
    const int32 Delta = Score - OldScore;
    BroadcastScoreChanged(OldScore, Delta, TEXT("Replicated"), nullptr);
}

void UAC_PointsComponent::BroadcastScoreChanged(int32 OldScore, int32 Delta, FName Reason, AActor* Instigator) // Notify listeners about score change
{
    OnScoreChanged.Broadcast(Score, Delta, Reason, Instigator);
}

void UAC_PointsComponent::AddPoints(int32 Amount, FName Reason, AActor* Instigator)     // Add points to the current score
{
    const int32 Old = Score;
	Score += Amount; // if negative, points are subtracted 
    BroadcastScoreChanged(Old, Amount, Reason, Instigator);
}



void UAC_PointsComponent::SetScore(int32 NewScore, FName Reason, AActor* Instigator) // Set the current score
{
    const int32 Old = Score;
	Score = NewScore; // Set new score
	BroadcastScoreChanged(Old, Score - Old, Reason, Instigator); //	Notify listeners
}





void UAC_PointsComponent::BeginPlay()
{
	Super::BeginPlay();	
}
void UAC_PointsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

