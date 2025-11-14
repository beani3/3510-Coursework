// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_PowerupComponent.h"

// this was the original PowerupComponet however it is currently unused as it went weird
UAC_PowerupComponent::UAC_PowerupComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAC_PowerupComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void UAC_PowerupComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

