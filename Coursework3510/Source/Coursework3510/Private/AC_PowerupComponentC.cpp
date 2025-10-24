// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_PowerupComponentC.h"
#include "AC_HealthComponent.h"
#include "AC_ProjectileComponent.h"
#include "Kismet/GameplayStatics.h"

UAC_PowerupComponentC::UAC_PowerupComponentC()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAC_PowerupComponentC::SetTables(UDataTable* InBuffs, UDataTable* InProjectiles)
{
	BuffTable = InBuffs;
	ProjectileTable = InProjectiles;
}

bool UAC_PowerupComponentC::GiveBuffByName(FName RowName)
{
	if (HasItem() || !BuffTable || RowName == NAME_None) return false;
	if (const FBuffRow* Row = BuffTable->FindRow<FBuffRow>(RowName, TEXT("GiveBuff")))
	{
		HeldBuff = *Row; if (HeldBuff.Name.IsNone()) HeldBuff.Name = RowName;
		HeldKind = EHeldItemKind::Buff; NotifyIcon(); return true;
	}
	return false;
}

bool UAC_PowerupComponentC::GiveProjectileByName(FName RowName)
{
	if (HasItem() || !ProjectileTable || RowName == NAME_None) return false;
	if (const FProjectileRow* Row = ProjectileTable->FindRow<FProjectileRow>(RowName, TEXT("GiveProj")))
	{
		HeldProjectile = *Row; if (HeldProjectile.Name.IsNone()) HeldProjectile.Name = RowName;
		HeldKind = EHeldItemKind::Projectile; NotifyIcon(); return true;
	}
	return false;
}

bool UAC_PowerupComponentC::GiveByHandle(const FDataTableRowHandle& Handle)
{
	if (HasItem() || !Handle.DataTable || Handle.RowName == NAME_None) return false;

	/* Identify which row struct the handle points to */
	if (Handle.DataTable->GetRowStruct() == FBuffRow::StaticStruct())
	{
		if (const FBuffRow* Row = Handle.DataTable->FindRow<FBuffRow>(Handle.RowName, TEXT("GiveByHandle")))
		{
			HeldBuff = *Row; if (HeldBuff.Name.IsNone()) HeldBuff.Name = Handle.RowName;
			HeldKind = EHeldItemKind::Buff; NotifyIcon(); return true;
		}
	}
	else if (Handle.DataTable->GetRowStruct() == FProjectileRow::StaticStruct())
	{
		if (const FProjectileRow* Row = Handle.DataTable->FindRow<FProjectileRow>(Handle.RowName, TEXT("GiveByHandle")))
		{
			HeldProjectile = *Row; if (HeldProjectile.Name.IsNone()) HeldProjectile.Name = Handle.RowName;
			HeldKind = EHeldItemKind::Projectile; NotifyIcon(); return true;
		}
	}
	return false;
}

bool UAC_PowerupComponentC::ActivateHeld()
{
	if (!HasItem()) return false;

	switch (HeldKind)
	{
	case EHeldItemKind::Buff: UseHealth(HeldBuff); break;
	case EHeldItemKind::Projectile: UseProjectile(HeldProjectile); break;
	default: return false;
	}

	ClearHeld();
	return true;
}

void UAC_PowerupComponentC::UseHealth(const FBuffRow& Row)
{
	if (AActor* Owner = GetOwner())
	{
		if (UAC_HealthComponent* HC = Owner->FindComponentByClass<UAC_HealthComponent>())
		{
			HC->Heal(Row.HealAmount);
		}
	}
}

void UAC_PowerupComponentC::UseProjectile(const FProjectileRow& Row)
{
	if (AActor* Owner = GetOwner())
	{
		if (UAC_ProjectileComponent* PC = Owner->FindComponentByClass<UAC_ProjectileComponent>())
		{
			PC->FireByRow(Row, /*HomingTarget=*/ nullptr);
		}
	}
}

void UAC_PowerupComponentC::ClearHeld()
{
	HeldBuff = FBuffRow{};
	HeldProjectile = FProjectileRow{};
	HeldKind = EHeldItemKind::None;
	NotifyIcon();
}

UTexture2D* UAC_PowerupComponentC::GetHeldIcon()
{
	if (!HasItem()) return nullptr;
	if (HeldKind == EHeldItemKind::Buff)       return HeldBuff.Icon.IsNull() ? nullptr : HeldBuff.Icon.LoadSynchronous();
	if (HeldKind == EHeldItemKind::Projectile)  return HeldProjectile.Icon.IsNull() ? nullptr : HeldProjectile.Icon.LoadSynchronous();
	return nullptr;
}

void UAC_PowerupComponentC::NotifyIcon()
{
	OnHeldItemChanged.Broadcast(GetHeldIcon(), HasItem());
}


// Called when the game starts
void UAC_PowerupComponentC::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAC_PowerupComponentC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

