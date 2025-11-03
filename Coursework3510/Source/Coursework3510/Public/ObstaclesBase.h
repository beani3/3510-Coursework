#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObstacleData.h"
#include "AC_HealthComponent.h"
#include "ObstaclesBase.generated.h"


/*
* Base class for all obstacles in the game.
* Handles mesh, collision, health, and hit effects.
* Looking to use a points system for obstacles in future.
* Trying to make it as modular and reusable as possible.
*/

class UNiagaraSystem; //forward declaration for Niagara
class UStaticMeshComponent; //same thing for static mesh

UCLASS()
class COURSEWORK3510_API AObstaclesBase : public AActor
{
	GENERATED_BODY()
	
public:	

	AObstaclesBase();

protected:

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override; // apply mesh asset to component in editor

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obstacle")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle", meta = (DisplayName = "Static Mesh Asset")) //The mesh asset for this obstacle, can be set per instance
	UStaticMesh* MeshAsset = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obstacle|Collision") //Collision box for obstacle hit detection
	UBoxComponent* Collision;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Obstacle|Data")
	UObstacleData* Data = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obstacle|Health")
	UAC_HealthComponent* HealthComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|FX") //Plays Sound on hit, optional, can be set per obstacle instance
	USoundBase* HitSFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|FX") //Plays VFX on hit, optional, can be set per obstacle instance
	UNiagaraSystem* HitVFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|FX", meta = (ClampMin = "0.01", UIMin = "0.1")) //For when we want bigger explosions
	float VFXScale = 1.f; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Hit") //Stops obstacle from playing hit FX more than once, when true
	bool bFXPlayed = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle|Collision") //If true, collision box will auto size to mesh bounds on begin play
	bool bAutoSizeCollisionToMeshBounds = true;

	uint64 LastHitFrame = 0; //To prevent multiple FX plays in a single frame

	UFUNCTION()
	virtual void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit); //Handles obstacle hit events

	virtual void HandlePostEffectsOnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FVector& NormalImpulse, const FHitResult& Hit); //Handles playing FX on hit

	void PlayEffectsAtHit(const FHitResult& Hit); //Plays the hit FX at the hit location

	void ApplyData();
	UFUNCTION()
	void OnDied_Handle();



public:	
	UFUNCTION(BlueprintCallable, Category = "Obstacle") //Allows access to collision component
	UBoxComponent* GetCollision() const { return Collision; }

	UFUNCTION(BlueprintCallable, Category = "Obstacle")
	UStaticMeshComponent* GetMesh() const { return Mesh; } //Returns the obstacle mesh
};
