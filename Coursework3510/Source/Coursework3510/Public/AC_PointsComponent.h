// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DA_PointsRules.h"
#include "AC_PointsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnScoreChanged,	int32, NewScore, int32, Delta, FName, Reason, AActor*, Instigator);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COURSEWORK3510_API UAC_PointsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_PointsComponent();


    // Current score (replicated)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Score, Category = "Score")
    int32 Score = 0;

    // Called whenever Score changes (server and clients after OnRep)
    UPROPERTY(BlueprintAssignable, Category = "Score")
    FOnScoreChanged OnScoreChanged;

	// Add points to the current score
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddPoints(int32 Amount, FName Reason = NAME_None, AActor* Instigator = nullptr);

    
	// Set the current score
    UFUNCTION(BlueprintCallable, Category = "Score")
    void SetScore(int32 NewScore, FName Reason = TEXT("SetScore"), AActor* Instigator = nullptr);

    // Get the current score
    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetScore() const { return Score; }

 

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Replication setup
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void OnRep_Score(int32 OldScore);

private:
	// Broadcast score change event
    void BroadcastScoreChanged(int32 OldScore, int32 Delta, FName Reason, AActor* Instigator);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
