// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_PointsRules.generated.h"

// Structure defining a single point rule
USTRUCT(BlueprintType)
struct FPointRule
{
    GENERATED_BODY()

	// Reason for awarding or deducting points
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Reason;
	// Amount of points to award (positive) or deduct (negative)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Amount = 0;
};

UCLASS()
class COURSEWORK3510_API UDA_PointsRules : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// Array of point rules defining scoring behavior
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Score")
    TArray<FPointRule> Rules;
};
