// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BPI_ScoreReceiver.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBPI_ScoreReceiver : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COURSEWORK3510_API IBPI_ScoreReceiver
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Give/Take points to the receiver 
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Score")
    void GivePoints(int32 Amount, FName Reason, AActor* Causer);

    
};
