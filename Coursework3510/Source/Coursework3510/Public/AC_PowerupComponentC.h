// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "ItemTypes.h"
#include "AC_PowerupComponentC.generated.h"

class UAC_HealthComponent;
class UAC_ProjectileComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHeldItemChanged, UTexture2D*, Icon, bool, bHasItem);

UENUM(BlueprintType)
enum class EHeldItemKind : uint8 { None, Buff, Projectile };

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COURSEWORK3510_API UAC_PowerupComponentC : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_PowerupComponentC();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items") UDataTable* BuffTable = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items") UDataTable* ProjectileTable = nullptr;

	/* Held item (exactly one). We store one of these + a kind flag. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items") FBuffRow HeldBuff;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items") FProjectileRow HeldProjectile;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items") EHeldItemKind HeldKind = EHeldItemKind::None;

public:
	/* UI notify when slot changes */
	UPROPERTY(BlueprintAssignable, Category = "Events") FOnHeldItemChanged OnHeldItemChanged;

	/* Setup */
	UFUNCTION(BlueprintCallable, Category = "Items") void SetTables(UDataTable* InBuffs, UDataTable* InProjectiles);

	/* Pickup grant */
	UFUNCTION(BlueprintCallable, Category = "Items") bool GiveBuffByName(FName RowName);
	UFUNCTION(BlueprintCallable, Category = "Items") bool GiveProjectileByName(FName RowName);
	UFUNCTION(BlueprintCallable, Category = "Items") bool GiveByHandle(const FDataTableRowHandle& Handle); // can point to either table

	/* Use current item */
	UFUNCTION(BlueprintCallable, Category = "Items") bool ActivateHeld();

	/* UI helpers */
	UFUNCTION(BlueprintPure, Category = "Items") bool HasItem() const { return HeldKind != EHeldItemKind::None; }
	UFUNCTION(BlueprintPure, Category = "Items") EHeldItemKind GetHeldKind() const { return HeldKind; }
	UFUNCTION(BlueprintPure, Category = "Items") UTexture2D* GetHeldIcon(); // loads soft icon sync

private:
	void ClearHeld();
	void NotifyIcon();

	void UseHealth(const FBuffRow& Row);
	void UseProjectile(const FProjectileRow& Row);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
