// Fill out your copyright notice in the Description page of Project Settings.



#include "AC_PointsComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UAC_PointsComponent::UAC_PointsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	  SetIsReplicatedByDefault(true);
}

void UAC_PointsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UAC_PointsComponent, Score);
}

void UAC_PointsComponent::OnRep_Score(int32 OldScore)
{
    const int32 Delta = Score - OldScore;
    BroadcastScoreChanged(OldScore, Delta, TEXT("Replicated"), nullptr);
}

void UAC_PointsComponent::BroadcastScoreChanged(int32 OldScore, int32 Delta, FName Reason, AActor* Instigator)
{
    OnScoreChanged.Broadcast(Score, Delta, Reason, Instigator);
}

void UAC_PointsComponent::AddPoints(int32 Amount, FName Reason, AActor* Instigator)
{
    const int32 Old = Score;
    Score += Amount;
    BroadcastScoreChanged(Old, Amount, Reason, Instigator);
}



void UAC_PointsComponent::SetScore(int32 NewScore, FName Reason, AActor* Instigator)
{
    const int32 Old = Score;
    Score = NewScore;
    BroadcastScoreChanged(Old, Score - Old, Reason, Instigator);
}





// Called when the game starts
void UAC_PointsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAC_PointsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

