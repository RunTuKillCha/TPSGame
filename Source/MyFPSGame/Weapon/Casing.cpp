// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"
#include "Sound/SoundCue.h"//声音头文件
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;


	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionCapsule"));
	SetRootComponent(CasingMesh);
	CasingMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CasingMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	CasingMesh->SetSimulatePhysics(true);//模拟物理
	//CasingMesh->SetNotifyRigidBodyCollision(true);



	ShellDestroyDelay = FLatentActionInfo(0, FMath::Rand(), TEXT("ShellDestroy"), this);//延时


	ShellEjectionImpulse = -40.f;
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
	
	CasingMesh->AddImpulse(GetActorForwardVector()* ShellEjectionImpulse);//添加冲量

	CasingMesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);

	UKismetSystemLibrary::Delay(this, 3.f, ShellDestroyDelay);
	bSoundOnce = true;
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){
	
	if (ShellSound && bSoundOnce) {
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ShellSound,
			GetActorLocation()
		);
	}
	
	bSoundOnce = false;
}

void ACasing::ShellDestroy(){
	Destroy();
}

