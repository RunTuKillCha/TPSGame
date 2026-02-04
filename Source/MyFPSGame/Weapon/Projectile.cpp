
#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyFPSGame/Character/BaseCharacter.h"
#include "Particles/ParticleSystemComponent.h"//粒子系统组件
#include "Particles/ParticleSystem.h"//粒子系统
#include "Sound/SoundCue.h"//声音头文件
#include "Kismet/GameplayStatics.h"
#include "ProjectileWeapon.h"

#include "MyFPSGame/MyFPSGame.h"
#include "MyFPSGame/Config/Struct_All.h"

AProjectile::AProjectile(){
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);//先忽略所有通道碰撞
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);//阻挡可见性通道
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);//阻挡静态网格体通道
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);
	//CollisionBox->SetEnableGravity(false);

	ProjectileMobementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMobementComponent->bRotationFollowsVelocity = true;//旋转和速度保持一致 || 转向速度方向
	ProjectileMobementComponent->ProjectileGravityScale = 0.f;//重力为0
}

void AProjectile::BeginPlay(){
	Super::BeginPlay();
	if (Tracer) {
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(//生成粒子特效
			Tracer,//生成的粒子特效
			CollisionBox,//绑定的父组件
			FName(),//如果想绑定到骨骼上输入骨骼名字
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition//保持世界位置
		);
	}
	if (HasAuthority()) {
		CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);//绑定函数
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){

	
	//伤害
	AWeapon* Weapon = Cast<AWeapon>(GetOwner());
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetInstigator());
	
	if (Weapon && BaseCharacter) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Hit.BoneName.ToString());

		if (DamageWeightTable) {
			FDamageWeight* _data = DamageWeightTable->FindRow<FDamageWeight>(Hit.BoneName, TEXT("Test"), false);
			if (_data) {
				UGameplayStatics::ApplyDamage(OtherActor, Weapon->GetDamage() * _data->DamageWeight, BaseCharacter->GetController(), this, UDamageType::StaticClass());
			}
			else {
				UGameplayStatics::ApplyDamage(OtherActor, Weapon->GetDamage(), BaseCharacter->GetController(), this, UDamageType::StaticClass());
			}
		}

		
	}

	//多播命中
	MulticastOnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

	//销毁子弹
	Destroy();
}

void AProjectile::MulticastOnHit_Implementation(UPrimitiveComponent* HitComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit){
	UParticleSystem* ParticleSystemImpact = nullptr;
	USoundCue* SoundCueImpact = nullptr;

	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
	if (BaseCharacter) {
		ParticleSystemImpact = ImpactParticlesCreature;
		SoundCueImpact = ImpactSoundKevlar;
	}
	else {
		ParticleSystemImpact = ImpactParticles;
		SoundCueImpact = ImpactSound;
	}

	if (ParticleSystemImpact) {
		UGameplayStatics::SpawnEmitterAtLocation(//生成粒子发射器
			GetWorld(),
			ParticleSystemImpact,
			GetActorTransform()
		);
	}
	if (SoundCueImpact) {
		UGameplayStatics::PlaySoundAtLocation(
			this,
			SoundCueImpact,
			GetActorLocation()
		);
	}
}


void AProjectile::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

}

