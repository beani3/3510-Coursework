// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GIC_RacingGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class COURSEWORK3510_API UGIC_RacingGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
	bool bIsServer = false;
	
};
