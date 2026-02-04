// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Struct_All.generated.h"

UCLASS()
class MYFPSGAME_API AStruct_All : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStruct_All();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};


USTRUCT(BlueprintType)
struct FDamageWeight :public FTableRowBase {
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Table")
	float DamageWeight = 1.f;
};