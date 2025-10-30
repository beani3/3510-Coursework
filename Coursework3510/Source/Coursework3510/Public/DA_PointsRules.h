// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_PointsRules.generated.h"

USTRUCT(BlueprintType)
struct FPointRule
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Reason;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Amount = 0;
};

UCLASS()
class COURSEWORK3510_API UDA_PointsRules : public UDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Score")
    TArray<FPointRule> Rules;
};
