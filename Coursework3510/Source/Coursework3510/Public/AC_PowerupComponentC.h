// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PowerItemDef.h"
#include "BuffDef.h"
#include "ProjectileDef.h"
#include "AC_PowerupComponentC.generated.h"

// Delegate for held item change (icon, has item)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHeldItemChanged, UTexture2D*, Icon, bool, bHasItem);

// Kind of held item
UENUM(BlueprintType)
enum class EHeldItemKind : uint8 { None, Buff, Projectile };


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COURSEWORK3510_API UAC_PowerupComponentC : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_PowerupComponentC();

protected:
	// held item
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Items")
	const UPowerItemDef* HeldItem = nullptr;

	// kind of held item
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Items")
	EHeldItemKind HeldKind = EHeldItemKind::None;

public:
	// held item changed event
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHeldItemChanged OnHeldItemChanged;

	// Give an item to hold
	UFUNCTION(BlueprintCallable, Category = "Items")
	bool GiveItem(const UPowerItemDef* Def);   // called by pickup

	// Activate held item
	UFUNCTION(BlueprintCallable, Category = "Items")
	bool ActivateHeld();                       // bind to input

	// Check if has held item
	UFUNCTION(BlueprintPure, Category = "Items")
	bool HasItem() const { return HeldKind != EHeldItemKind::None && HeldItem; }

	// Get held item icon
	UFUNCTION(BlueprintPure, Category = "Items")
	UTexture2D* GetHeldIcon() const;

private:
	// Clear held item
	void ClearHeld();
	// Notify icon changed
	void NotifyIcon();
};
