// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class MYFPSGAME_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	ACasing();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(//命中事件函数
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

private:
	//UPROPERTY(EditAnywhere)
	//class UCapsuleComponent* CollisionCapsule;

	//UPROPERTY(VisibleAnywhere)
	//class UProjectileMovementComponent* ProjectileMobementComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CasingMesh;
	
	UPROPERTY(EditAnywhere)
	float ShellEjectionImpulse;

	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;

	FLatentActionInfo ShellDestroyDelay;

	bool bSoundOnce;

	UFUNCTION()
	void ShellDestroy();
};
