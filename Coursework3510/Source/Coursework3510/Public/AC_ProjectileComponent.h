// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemTypes.h"
#include "ProjectileDef.h"
#include "AC_ProjectileComponent.generated.h"

class AProjectile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COURSEWORK3510_API UAC_ProjectileComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAC_ProjectileComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FTransform MuzzleOffset;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	bool FireByDef(const UProjectileDef* Def, USceneComponent* HomingTarget = nullptr);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	
		
};
