// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PowerItemDef.h"
#include "BuffDef.h"
#include "ProjectileDef.h"
#include "AC_PowerupComponentC.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHeldItemChanged, UTexture2D*, Icon, bool, bHasItem);

UENUM(BlueprintType)
enum class EHeldItemKind : uint8 { None, Buff, Projectile };

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COURSEWORK3510_API UAC_PowerupComponentC : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_PowerupComponentC();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Items")
	const UPowerItemDef* HeldItem = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Items")
	EHeldItemKind HeldKind = EHeldItemKind::None;

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHeldItemChanged OnHeldItemChanged;

	UFUNCTION(BlueprintCallable, Category = "Items")
	bool GiveItem(const UPowerItemDef* Def);   // called by pickup

	UFUNCTION(BlueprintCallable, Category = "Items")
	bool ActivateHeld();                       // bind to input

	UFUNCTION(BlueprintPure, Category = "Items")
	bool HasItem() const { return HeldKind != EHeldItemKind::None && HeldItem; }

	UFUNCTION(BlueprintPure, Category = "Items")
	UTexture2D* GetHeldIcon() const;

private:
	void ClearHeld();
	void NotifyIcon();
};
