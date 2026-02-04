// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class MYFPSGAME_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;
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

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHit(UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;
	
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMobementComponent;
	
	UPROPERTY()
	class UParticleSystemComponent* TracerComponent;//粒子系统组件

	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;//子弹曳光

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;//命中特效
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticlesCreature;//命中特效生物

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSoundKevlar;

	UPROPERTY(EditAnywhere)
	UDataTable* DamageWeightTable;
};




