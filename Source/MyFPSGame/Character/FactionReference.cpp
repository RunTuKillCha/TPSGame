// Fill out your copyright notice in the Description page of Project Settings.


#include "FactionReference.h"


// Sets default values
AFactionReference::AFactionReference()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SetRootComponent(SkeletalMeshComponent);
}

// Called when the game starts or when spawned
void AFactionReference::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFactionReference::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

