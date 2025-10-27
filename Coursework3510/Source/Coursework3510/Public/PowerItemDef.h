// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PowerItemDef.generated.h"

UENUM(BlueprintType)
enum class EPowerItemKind : uint8 { Buff, Projectile };

UCLASS(Abstract, BlueprintType)
class COURSEWORK3510_API UPowerItemDef : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPowerItemKind Kind = EPowerItemKind::Buff;

	/** UI icon shown when held */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;
};