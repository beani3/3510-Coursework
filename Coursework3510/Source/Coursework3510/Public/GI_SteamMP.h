// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "GI_SteamMP.generated.h"

/**
 * 
 */
UCLASS()
class COURSEWORK3510_API UGI_SteamMP : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()
	

public:

	virtual void Init() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
	bool bIsServer = false;
};
