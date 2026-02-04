// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "MyFPSGame/Character/BaseCharacter.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MyFPSGame/PlayerController/BasePlayerController.h"

#include "Net/UnrealNetwork.h"//属性复制宏头文件


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;//开启网络复制
	SetReplicateMovement(true);
	//SetReplicatedMovement();

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);//全部阻挡
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);//忽略Pawn
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);//初始不启用碰撞

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);//全部忽略
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);//初始不启用碰撞

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("pickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, WeaponState);
	DOREPLIFETIME(ThisClass, Ammo);
	DOREPLIFETIME(ThisClass, CarryAmmo);
}



void AWeapon::Fire(const FVector& HitTarget){

	if (FireAnimation) {//武器开火动画
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	
	if (CasingClass) {//蛋壳生成
		APawn* InstigatorPawn = Cast<APawn>(GetOwner());
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket) {
			//使用网格需要头文件
			FTransform SocketTransfrom = AmmoEjectSocket->GetSocketTransform(WeaponMesh);

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;//所有者为武器
			SpawnParameters.Instigator = InstigatorPawn;//发起者为玩家

			UWorld* World = GetWorld();

			if (World) {
				World->SpawnActor<ACasing>(
					CasingClass,
					SocketTransfrom.GetLocation(),
					SocketTransfrom.GetRotation().Rotator(),
					SpawnParameters
				);
			}
		}
	}
	
	SpendRound();
}

void AWeapon::Reload(){
	if (ReloadAnimation) {//武器开火动画
		WeaponMesh->PlayAnimation(ReloadAnimation, false);
	}
}

void AWeapon::BeginPlay(){
	Super::BeginPlay();
	if (PickupWidget) {
		PickupWidget->SetVisibility(false);
	}

	//将射速转换为射击延迟
	FireDelay = 1.f / (WeaponRateOfFire / 60.f);

	//(GetLocalRole() == ENetRole::ROLE_Authority) || HasAuthority() 都是检测是否是服务器
	if (HasAuthority()) {
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);//仅在服务器启用碰撞检测
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);//重叠检测
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);//绑定函数
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}


}


void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){
	//重写的重叠函数
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
	if (BaseCharacter&& HasAuthority()) {
		BaseCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
	if (BaseCharacter) {
		BaseCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

}

void AWeapon::ShowPickupWidget(bool bShowWidget){
	if (PickupWidget) {
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::SetWeaponState(EWeaponState State) {//此函数在逻辑上只发生于服务器
	WeaponState = State;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:	
		if (HasAuthority()){
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		ShowPickupWidget(true);
		break;
	case EWeaponState::EWS_Equipped:
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		ShowPickupWidget(false);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


		break;
	case EWeaponState::EWS_Dropped:
		if (HasAuthority()) {
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}

		//ShowPickupWidget(true);
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);


		break;
	default:
		break;
	}
}

void AWeapon::Dropped() {
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachmentRules);

	SetOwner(nullptr);
	OwnerCharacter = nullptr;
	OwnerController = nullptr;
}

bool AWeapon::IsEmpty(){
	return Ammo <= 0;
}

void AWeapon::SetHUDAmmo(){
	OwnerCharacter = OwnerCharacter == nullptr ? Cast<ABaseCharacter>(GetOwner()) : OwnerCharacter;
	if (OwnerCharacter) {
		OwnerController = OwnerController == nullptr ? Cast<ABasePlayerController>(OwnerCharacter->Controller) : OwnerController;
		if (OwnerController) {
			OwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void AWeapon::SetHUDCarryAmmo(){
	OwnerCharacter = OwnerCharacter == nullptr ? Cast<ABaseCharacter>(GetOwner()) : OwnerCharacter;
	if (OwnerCharacter) {
		OwnerController = OwnerController == nullptr ? Cast<ABasePlayerController>(OwnerCharacter->Controller) : OwnerController;
		if (OwnerController) {
			OwnerController->SetHUDCarryAmmo(CarryAmmo);
		}
	}	
}

void AWeapon::SpendRound(){
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	SetHUDAmmo();
}

void AWeapon::OnRep_Ammo(){
	SetHUDAmmo();
}

void AWeapon::OnRep_CarryAmmo(){
	SetHUDCarryAmmo();
}

void AWeapon::OnRep_Owner() {
	Super::OnRep_Owner();
	if (Owner == nullptr) {
		OwnerCharacter = nullptr;
		OwnerController = nullptr;
		return;
	}

	SetHUDAmmo();
	SetHUDCarryAmmo();
}

void AWeapon::WeaponStateUpData() {//此函数仅在客户端执行
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:
		ShowPickupWidget(true);
		break;
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EWeaponState::EWS_Dropped:
		//ShowPickupWidget(true);
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	default:
		break;
	}
}

USkeletalMeshComponent* AWeapon::GetWeaponMesh(){
	return WeaponMesh;
}

float AWeapon::GetZoomedFOV() const{
	return ZoomedFOV;
}

float AWeapon::GetZoomedInterpSpeed() const{
	return ZoomedInterpSpeed;
}

float AWeapon::GetBaseCrosshairSpreads() const
{
	return BaseCrosshairSpreads;
}

float AWeapon::GetFireDelay(){
	return FireDelay;
}

bool AWeapon::GetbAutomatic(){
	return bAutomatic;
}

float AWeapon::GetDamage(){
	return Damage;
}




EEquipmentType AWeapon::GetWeaponType(){
	return WeaponType;
}


