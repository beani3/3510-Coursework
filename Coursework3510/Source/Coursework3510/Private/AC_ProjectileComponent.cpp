// Fill out your copyright notice in the Description page of Project Settings.

#include "AC_ProjectileComponent.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "MyPlayerCar.h" // For RacePosition

UAC_ProjectileComponent::UAC_ProjectileComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UAC_ProjectileComponent::BeginPlay()
{
    Super::BeginPlay();
}

bool UAC_ProjectileComponent::FireByDef(const UProjectileDef* Def, USceneComponent* HomingTarget)
{
    if (!Def || !ProjectileClass || !GetOwner() || !GetWorld())
        return false;

    if (!GetOwner()->HasAuthority())
    {
        ServerFireByDef(Def, HomingTarget);
        return true;
    }

    if (Def->Behavior == EProjBehavior::Homing && !HomingTarget)
        HomingTarget = PickAheadHomingTarget();

    const FTransform SpawnTM = BuildSpawnTM();

    AProjectile* Proj = GetWorld()->SpawnActorDeferred<AProjectile>(
        ProjectileClass,
        SpawnTM,
        GetOwner(),
        Cast<APawn>(GetOwner()),
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

    if (!Proj) return false;

    Proj->InitFromDef(Def, GetOwner(), HomingTarget);
    Proj->SetDefPathFromDef(Def);

    UGameplayStatics::FinishSpawningActor(Proj, SpawnTM);

    PlayMuzzleFX(SpawnTM);

    return true;
}

void UAC_ProjectileComponent::ServerFireByDef_Implementation(const UProjectileDef* Def, USceneComponent* HomingTarget)
{
    FireByDef(Def, HomingTarget);
}

USceneComponent* UAC_ProjectileComponent::ResolveMuzzle() const
{
    if (MuzzleComponent && MuzzleComponent->IsRegistered())
        return MuzzleComponent;

    AActor* Owner = GetOwner();
    if (!Owner) return nullptr;

    for (UActorComponent* C : Owner->GetComponents())
    {
        if (USceneComponent* SC = Cast<USceneComponent>(C))
        {
            if (SC->GetFName() == TEXT("Muzzle") || SC->ComponentHasTag(TEXT("Muzzle")))
                return SC;
        }
    }

    return Owner->GetRootComponent();
}

FTransform UAC_ProjectileComponent::BuildSpawnTM() const
{
    const USceneComponent* Muzzle = ResolveMuzzle();
    FTransform BaseTM = Muzzle ? Muzzle->GetComponentTransform()
        : (GetOwner() ? GetOwner()->GetActorTransform() : FTransform::Identity);
    return MuzzleOffset * BaseTM;
}

void UAC_ProjectileComponent::PlayMuzzleFX(const FTransform& SpawnTM)
{
    // Example placeholder: add SFX/VFX
}

USceneComponent* UAC_ProjectileComponent::PickAheadHomingTarget() const
{
    const AMyPlayerCar* Shooter = Cast<AMyPlayerCar>(GetOwner());
    if (!Shooter) return nullptr;

    const int32 MyPos = Shooter->RacePosition;
    if (MyPos <= 1) return nullptr;

    AMyPlayerCar* Best = nullptr;
    int32 BestPos = MyPos;

    TArray<AActor*> Cars;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyPlayerCar::StaticClass(), Cars);

    for (AActor* A : Cars)
    {
        AMyPlayerCar* Car = Cast<AMyPlayerCar>(A);
        if (!IsValid(Car) || Car == Shooter) continue;

        const int32 Pos = Car->RacePosition;
        if (Pos > 0 && Pos < MyPos)
        {
            if (!Best || Pos < BestPos)
            {
                Best = Car;
                BestPos = Pos;
            }
        }
    }

    return Best ? Best->GetRootComponent() : nullptr;
}
