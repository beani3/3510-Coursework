// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_PowerupComponentC.h"
#include "PowerUpUserInterface.h"

UAC_PowerupComponentC::UAC_PowerupComponentC()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// give an item if none held
bool UAC_PowerupComponentC::GiveItem(const UPowerItemDef* Def)
{
	if (HasItem() || !Def) return false;

	HeldItem = Def;
	// kind of powerup
	HeldKind = (Def->Kind == EPowerItemKind::Projectile) ? EHeldItemKind::Projectile :
		(Def->Kind == EPowerItemKind::Buff) ? EHeldItemKind::Buff : EHeldItemKind::None; 

	NotifyIcon(); 
	return true;
}

bool UAC_PowerupComponentC::ActivateHeld() // activate and clear held item
{
	if (!HasItem()) return false;
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->GetClass()->ImplementsInterface(UPowerUpUserInterface::StaticClass())) return false;

	if (HeldKind == EHeldItemKind::Buff) // apply buff
	{
	
		if (const UBuffDef* B = Cast<UBuffDef>(HeldItem))
			IPowerUpUserInterface::Execute_ApplyBuff(Owner, B);
	}
	else if (HeldKind == EHeldItemKind::Projectile) // fire projectile
	{
		if (const UProjectileDef* P = Cast<UProjectileDef>(HeldItem))
			IPowerUpUserInterface::Execute_FireProjectileFromDef(Owner, P);
	}

	ClearHeld(); // clear after use
	return true;
}

UTexture2D* UAC_PowerupComponentC::GetHeldIcon() const // get icon of held item
{
	if (!HasItem()) return nullptr;
	return HeldItem->Icon.IsNull() ? nullptr : HeldItem->Icon.LoadSynchronous(); // load powerup icon if held 
}

void UAC_PowerupComponentC::ClearHeld() // clear held item
{
	HeldItem = nullptr; // clear held item
	HeldKind = EHeldItemKind::None; // clear kind
	NotifyIcon();	// update UI Image
}

void UAC_PowerupComponentC::NotifyIcon()
{
	OnHeldItemChanged.Broadcast(GetHeldIcon(), HasItem()); // broadcast held item change
}
