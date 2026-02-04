// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "MyFPSGame/Character/BaseCharacter.h"
#include "Engine/SkeletalMeshSocket.h"//网格体插槽头文件
#include "MyFPSGame/Weapon/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyFPSGame/PlayerController/BasePlayerController.h"
#include "Camera/CameraComponent.h"
#include "Sound/SoundCue.h"

#include "Net/UnrealNetwork.h"//属性复制宏头文件

#include "TimerManager.h"

#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"//调试球

UCombatComponent::UCombatComponent(){
	PrimaryComponentTick.bCanEverTick = true;
	EquippedWeapon = MainWeapon = Pistol = Knife = C4 = nullptr;
}

void UCombatComponent::BeginPlay(){
	Super::BeginPlay();


	if (Character && Character->GetCamera()) {
		DefaultFOV = Character->GetCamera()->FieldOfView;
		CurrentFOV = DefaultFOV;
	}

	GenerateDefaultLoadout();
}

void UCombatComponent::GenerateDefaultLoadout(){

	if (DefaultCreateMainWeapon && Character && Character->HasAuthority()) {
		
		ABaseCharacter* InstigatorPawn = Cast<ABaseCharacter>(GetOwner());
		const USkeletalMeshSocket* HandSocket = InstigatorPawn->GetMesh()->GetSocketByName(FName("RightHandRifleSocket"));
		if (HandSocket) {
			//UE_LOG(LogTemp, Warning, TEXT("GenerateDefaultLoadout"));
			//使用网格需要头文件
			FTransform SocketTransfrom = HandSocket->GetSocketTransform(InstigatorPawn->GetMesh());
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = GetOwner();//所有者为武器
			SpawnParameters.Instigator = InstigatorPawn;//发起者为玩家
			AWeapon* Weapon;
			UWorld* World = GetWorld();
			if (World) {
				Weapon = World->SpawnActor<AWeapon>(
					DefaultCreateMainWeapon,
					SocketTransfrom.GetLocation(),
					SocketTransfrom.GetRotation().Rotator(),
					SpawnParameters);
				if (Weapon) {
					EquippedWeapon = MainWeapon = Weapon;
					Weapon->SetHUDAmmo();
					Weapon->SetHUDCarryAmmo();
					Weapon->ShowPickupWidget(false);

					Weapon->SetWeaponState(EWeaponState::EWS_Equipped);
					if (HandSocket) {
						HandSocket->AttachActor(Weapon, Character->GetMesh());
					}
				}
			}
		}
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled()) {
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;
		ServerSetHitTarget(HitTarget);

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
}



void UCombatComponent::SetAiming(bool bIsAiming) {//ServerAndMyClient
	bAiming = bIsAiming;
	//MainSpringArm->bUsePawnControlRotation = false;//使用相机控制器控制旋转

	//Character->GetCharacterMovement()->bOrientRotationToMovement = !bAiming;//设置角色朝向旋转运动方向
	//Character->bUseControllerRotationYaw = bAiming;
	Server_SetAiming(bIsAiming);
}


void UCombatComponent::ReloadButtonPressed() {
	if (EquippedWeapon && EquippedWeapon->GetCarryAmmo() > 0 && EquippedWeapon->GetAmmo() != EquippedWeapon->GetMagCapcity() && CombatState != ECombatState::ECS_Reloading) {
		ServerReload();
	}
}


void UCombatComponent::ServerReload_Implementation(){
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	

	CombatState = ECombatState::ECS_Reloading;
	if (Character) {
		Character->PlayReloadMontage();
		EquippedWeapon->Reload();
	}
	//MulticastReload();

}

int32 UCombatComponent::AmountToReload() {
	if (EquippedWeapon == nullptr)	return -1;
	int32 RoomInMag = EquippedWeapon->GetMagCapcity() - EquippedWeapon->GetAmmo();
	int32 AmountCarride = EquippedWeapon->GetCarryAmmo();
	int32 Least = FMath::Min(RoomInMag, AmountCarride);


	return FMath::Clamp(RoomInMag, 0, Least);
}


void UCombatComponent::OnRep_CombatState(){
	switch (CombatState){
	case ECombatState::ECS_Unoccupied:
		break;
	case ECombatState::ECS_Reloading:
		if (Character && EquippedWeapon) {
			Character->PlayReloadMontage();
			EquippedWeapon->Reload();
		}
		break;
	default:
		break;
	}
}

void UCombatComponent::FinishReloading() {
	if (Character && Character->HasAuthority()) {
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValues();
	}
}

void UCombatComponent::UpdateAmmoValues(){
	if (EquippedWeapon == nullptr) return;
	int32 ReloadAmount = AmountToReload();
	if (ReloadAmount > 0) {
		int32 CarryAmmo = EquippedWeapon->GetCarryAmmo();
		EquippedWeapon->SetCarryAmmo(CarryAmmo - ReloadAmount);
		int32 Ammo = EquippedWeapon->GetAmmo();
		EquippedWeapon->SetAmmo(FMath::Clamp(Ammo + ReloadAmount, 0, EquippedWeapon->GetMagCapcity()));
	}
}

void UCombatComponent::FireButtonPressed_Implementation(bool bPressed) {
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed && EquippedWeapon) {
		Fire();
	}
}

void UCombatComponent::ServerSetHitTarget_Implementation(const FVector_NetQuantize& TraceHitTarget) {
	if (HitTarget != TraceHitTarget) {
		HitTarget = TraceHitTarget;
	}
}
void UCombatComponent::Fire(){
	if (CanFire()) {
		bCanFire = false;
		MulticastFire(HitTarget);//多播开火
		if (EquippedWeapon) {
			CrosshairShootingFactor = 1.1;
		}
		StartFireTimer();
	}
}

void UCombatComponent::StartFireTimer() {
	if (EquippedWeapon && Character) {
		Character->GetWorldTimerManager().SetTimer(
			FireTime,
			this,
			&ThisClass::FireTimerFinished,
			EquippedWeapon->GetFireDelay()
		);
	}
}

void UCombatComponent::FireTimerFinished() {
	bCanFire = true;
	if (EquippedWeapon) {
		if (bFireButtonPressed && EquippedWeapon->GetbAutomatic()) {
			Fire();
		}
		if (EquippedWeapon->IsEmpty()) {
			ReloadButtonPressed();
		}
	}
}

bool UCombatComponent::CanFire(){
	if (EquippedWeapon == nullptr)return false;
	
	return !EquippedWeapon->IsEmpty() && bCanFire && CombatState != ECombatState::ECS_Reloading;
}


void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget) {//多播RPC
	if (EquippedWeapon == nullptr) return;
	if (Character && CombatState == ECombatState::ECS_Unoccupied) {
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime){//设置准星扩散
	if (Character == nullptr || Character->Controller == nullptr) return;
	Controller = Controller == nullptr ? Cast<ABasePlayerController>(Character->Controller) : Controller;
	if (Controller) {
		HUD = HUD == nullptr ? Cast<ABaseHUD>(Controller->GetHUD()) : HUD;
		if (HUD) {
			
			if (EquippedWeapon && CrosshairsCenter && CrosshairsLeft && CrosshairsRight && CrosshairsTop && CrosshairsBottom) {
				BaseCrosshairSpreads = EquippedWeapon->GetBaseCrosshairSpreads();
				HUDPackage.CrosshairsCenter = CrosshairsCenter;
				HUDPackage.CrosshairsLeft = CrosshairsLeft;
				HUDPackage.CrosshairsRight = CrosshairsRight;
				HUDPackage.CrosshairsTop = CrosshairsTop;
				HUDPackage.CrosshairsBottom = CrosshairsBottom;
			}
			else {
				BaseCrosshairSpreads = 0.f;
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}

			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);//蹲未考虑
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			float Speed = FVector2D(Character->GetVelocity().X, Character->GetVelocity().Y).Size();

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Speed);
			if (Character->GetCharacterMovement()->IsFalling()) {
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else {
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 50.f);
			}
			if (bAiming) {
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, -0.7f, DeltaTime, 30.f);
			}
			else {
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			HUDPackage.CrosshairSpread = 
				BaseCrosshairSpreads + 
				CrosshairVelocityFactor + 
				CrosshairInAirFactor +
				CrosshairAimFactor +
				CrosshairShootingFactor;

			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 5.f);

			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::InterpFOV(float DeltaTime){
	if (EquippedWeapon == nullptr) return;

	if (bAiming) {
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomedInterpSpeed());
	}
	else {
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	if (Character && Character->GetCamera()) {
		Character->GetCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::Server_SetAiming_Implementation(bool bIsAiming){//Server
	//检测合法
	bAiming = bIsAiming;
	//OtherClient_SetAiming(bIsAiming);
}

void UCombatComponent::OtherClient_SetAiming_Implementation(bool bIsAiming) {//OtherClient
	//Character->GetCharacterMovement()->bOrientRotationToMovement = !bAiming;//设置角色朝向旋转运动方向
	//Character->bUseControllerRotationYaw = bAiming;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, EquippedWeapon);
	DOREPLIFETIME(ThisClass, MainWeapon);
	DOREPLIFETIME(ThisClass, Pistol);
	DOREPLIFETIME(ThisClass, Knife);
	DOREPLIFETIME(ThisClass, C4);
	DOREPLIFETIME(ThisClass, bAiming);
	DOREPLIFETIME(ThisClass, CombatState);
	DOREPLIFETIME(ThisClass, bFireButtonPressed);
	
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip) {//装备武器
	if (Character == nullptr || WeaponToEquip == nullptr) return;//都有效执行装备

	

	const USkeletalMeshSocket* HandSocket;
	switch (WeaponToEquip->GetWeaponType()) {//装备武器到槽位并检测
	case EEquipmentType::EET_MainWeapon:
		if (MainWeapon != nullptr) {
			DroppedWeapon(MainWeapon);
		}
		MainWeapon = WeaponToEquip;
		HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandRifleSocket"));
		break;
	case EEquipmentType::EET_Pistol:
		if (Pistol != nullptr) {
			DroppedWeapon(Pistol);
		}
		Pistol = WeaponToEquip;
		HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandRifleSocket"));
		break;
	case EEquipmentType::EET_Knife:
		if (Knife != nullptr) {
			DroppedWeapon(Pistol);
		}
		Knife = WeaponToEquip;
		HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandKnifeSocket"));
		break;
	default:
		HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandC4Socket"));
		break;
	}

	WeaponToEquip->SetOwner(Character);
	WeaponToEquip->SetHUDAmmo();
	WeaponToEquip->SetHUDCarryAmmo();
	WeaponToEquip->ShowPickupWidget(false);

	if (EquippedWeapon == nullptr){ //手中没有武器时自动装备
		EquippedWeapon = WeaponToEquip;
		if (EquippedWeapon->EquipSound) {
			UGameplayStatics::PlaySoundAtLocation(
				this,
				EquippedWeapon->EquipSound,
				Character->GetActorLocation()
			);
		}
	}

	

	WeaponToEquip->SetWeaponState(EWeaponState::EWS_Equipped);
	if (HandSocket) {
		HandSocket->AttachActor(WeaponToEquip, Character->GetMesh());
	}
	
}

void UCombatComponent::OnRep_EquippedWeapon() {
	if (EquippedWeapon == nullptr) {
		Controller = Controller == nullptr ? Cast<ABasePlayerController>(Character->Controller) : Controller;
		if (Controller) {
			Controller->SetHUDWeaponAmmo(0);
			Controller->SetHUDCarryAmmo(0);
		}

		return;
	}

	const USkeletalMeshSocket* HandSocket;
	switch (EquippedWeapon->GetWeaponType()) {//装备武器到槽位并检测
	case EEquipmentType::EET_MainWeapon:
		HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandRifleSocket"));
		break;
	case EEquipmentType::EET_Pistol:
		HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandRifleSocket"));
		break;
	case EEquipmentType::EET_Knife:
		HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandKnifeSocket"));
		break;
	default:
		HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandC4Socket"));
		break;
	}

	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	if (HandSocket) {
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	if (EquippedWeapon->EquipSound) {
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquippedWeapon->EquipSound,
			Character->GetActorLocation()
		);
	}
}

void UCombatComponent::DroppedWeapon(AWeapon* WeaponToDropped) {
	WeaponToDropped->Dropped();

	if(EquippedWeapon == WeaponToDropped){
		Controller = Controller == nullptr ? Cast<ABasePlayerController>(Character->Controller) : Controller;
		if (Controller) {
			Controller->SetHUDWeaponAmmo(0);
			Controller->SetHUDCarryAmmo(0);
		}
		EquippedWeapon = nullptr;
	}
	
	switch (WeaponToDropped->GetWeaponType()) {
	case EEquipmentType::EET_MainWeapon:
		MainWeapon = nullptr;
		break;
	}

	
}

//获取瞄准点
void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult) {//此函数只有自制代理和服务器的本地控制玩家有效,因为模拟代理的角色没有视口

	if (EquippedWeapon) {
		//FTransform SocketTransfrom = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);//获取插槽变换
		//DrawDebugSphere(GetWorld(), SocketTransfrom.GetLocation(), 5.f, 32, FColor::Red);
	}

	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);//获取视口大小
		const FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);//视口中心点
		FVector CrosshairWorldPosition;//准星位置
		FVector CrosshairWorldDirection;//准星方向
		bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(//从视口2D转换到世界空间3D
			UGameplayStatics::GetPlayerController(this, 0),//提供一个要转换的玩家控制器 参数1是当前世界的任意物体
			CrosshairLocation,//输入参数
			CrosshairWorldPosition,//输出参数
			CrosshairWorldDirection//输出参数
		);

		if (bScreenToWorld) {//如果转换成功
			FVector Start = CrosshairWorldPosition;//开始端点
			if (Character) {
				float CameraToCharacter = (Character->GetActorLocation() - Start).Size();
				Start += CrosshairWorldDirection * (CameraToCharacter + 100.f);

			}

			FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;//结束端点
			bool bLineTraceSingle = GetWorld()->LineTraceSingleByChannel(//射线检测
				TraceHitResult,Start,End,ECollisionChannel::ECC_Visibility);

			if (!bLineTraceSingle) {//如果线追踪失败
				TraceHitResult.ImpactPoint = FVector_NetQuantize(End);
			}

			if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>()) {//设置准星颜色
				HUDPackage.CrosshairsColor = FLinearColor::Red;//如果可以获取到命中目标的Actor且该Actor实现了接口则设置红色
			}
			else {
				HUDPackage.CrosshairsColor = FLinearColor::White;
			}
		}
	}

}

//void UCombatComponent::SwitchWeapon(EWeaponType WeaponToSwitch){
//	if (Character == nullptr) return;//都有效执行
//	switch (WeaponToSwitch)
//	{
//	case EWeaponType::EWS_C4:
//		if (C4)EquippedWeapon = C4;
//		break;
//	case EWeaponType::EWS_MainWeapon:
//		if (MainWeapon)EquippedWeapon = MainWeapon;
//		break;
//	case EWeaponType::EWS_Pistol:
//		if (Pistol)EquippedWeapon = Pistol;
//		break;
//	case EWeaponType::EWS_Knife:
//		if (Knife)EquippedWeapon = Knife;
//		break;
//	case EWeaponType::EWS_Max:
//		break;
//	default:
//		break;
//	}
//}

