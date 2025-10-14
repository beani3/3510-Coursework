// Fill out your copyright notice in the Description page of Project Settings.


/*This is going to be the base class of which every single obstacle will inherit from
* There will be two seperate types of obstacles, those that are static and those that are interactable
* Static obstacles will be things like walls, ramps, barriers, etc
* Interactable obstacles will be things like oil slicks, signs, cones, etc
* Static obstacles can be placed in the level by the designer, the interactables can also be placed but could also have a randomised element
* The interactable obstacles will have a collision effect that causes them to break a part, get sent flying and possibly have an effect on the car.
* Others may have a flattening effect.
* Static obstacles will be a solid mesh with a collision box, they will not move, but can still apply damage and effects to the car.
*/


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "ObstaclesBase.generated.h"

UCLASS()
class COURSEWORK3510_API AObstaclesBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObstaclesBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ObstacleComponents")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObstacleVFX")
	float ImpactVFXMinSpeed; //minimum speed required to spawn VFX on hit.

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObstacleVFX")
	float CooldownBetweenVFX; //cooldown between VFX spawns.

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObstacleGameplay")
	float DamageToVehicle; //optional damage to apply on hit.

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ObstacleGameplay")
	bool bAffectVehicleOnHit; //may apply effects like knockback, slowing, etc.

	UPROPERTY(Transient)
	float LastVFXTime; //last time VFX was spawned.

	virtual void HandleVehicleHit(AActor* OtherActor, const FHitResult& Hit, float OtherSpeed); //handles what happens when the vehicle hits the obstacle.

	UFUNCTION(BlueprintImplementableEvent, Category = "ObstacleVFX")
	void PlayHitEffects(float OtherSpeed, FVector WorldLocation, FVector WorldNormal); //called when want to play hit SFX and VFX

	static float GetVehicleSpeed(AActor* VehicleActor); //gets the speed of the vehicle that hit the obstacle.

	bool IsAuth() const { return HasAuthority(); } //checks if the current instance is authoritative (server).

	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit); //called when the mesh is hit.

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
