// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_PowerupComponentC.h"
#include "PowerUpUserInterface.h"

UAC_PowerupComponentC::UAC_PowerupComponentC()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UAC_PowerupComponentC::GiveItem(const UPowerItemDef* Def)
{
	if (HasItem() || !Def) return false;

	HeldItem = Def;
	HeldKind = (Def->Kind == EPowerItemKind::Projectile) ? EHeldItemKind::Projectile :
		(Def->Kind == EPowerItemKind::Buff) ? EHeldItemKind::Buff : EHeldItemKind::None;

	NotifyIcon();
	return true;
}

bool UAC_PowerupComponentC::ActivateHeld()
{
	if (!HasItem()) return false;
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->GetClass()->ImplementsInterface(UPowerUpUserInterface::StaticClass())) return false;

	if (HeldKind == EHeldItemKind::Buff)
	{
		if (const UBuffDef* B = Cast<UBuffDef>(HeldItem))
			IPowerUpUserInterface::Execute_ApplyBuff(Owner, B);
	}
	else if (HeldKind == EHeldItemKind::Projectile)
	{
		if (const UProjectileDef* P = Cast<UProjectileDef>(HeldItem))
			IPowerUpUserInterface::Execute_FireProjectileFromDef(Owner, P);
	}

	ClearHeld();
	return true;
}

UTexture2D* UAC_PowerupComponentC::GetHeldIcon() const
{
	if (!HasItem()) return nullptr;
	return HeldItem->Icon.IsNull() ? nullptr : HeldItem->Icon.LoadSynchronous();
}

void UAC_PowerupComponentC::ClearHeld()
{
	HeldItem = nullptr;
	HeldKind = EHeldItemKind::None;
	NotifyIcon();
}

void UAC_PowerupComponentC::NotifyIcon()
{
	OnHeldItemChanged.Broadcast(GetHeldIcon(), HasItem());
}
