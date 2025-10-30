// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PowerItemDef.generated.h"

// Kind of powerup items
UENUM(BlueprintType)
enum class EPowerItemKind : uint8 { Buff, Projectile };

// Data asset defining a powerup item
UCLASS(Abstract, BlueprintType)
class COURSEWORK3510_API UPowerItemDef : public UDataAsset
{
	GENERATED_BODY()
public:
	// Kind of powerup item
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPowerItemKind Kind = EPowerItemKind::Buff;

	// icon representing the powerup item
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;
};