// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAIPath.h"
#include "Components/SplineComponent.h"

// Sets default values
AMyAIPath::AMyAIPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("AI Path"));
}

// Called when the game starts or when spawned
void AMyAIPath::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyAIPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}