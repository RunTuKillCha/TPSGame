// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "MyFPSGame/Character/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyFPSGame/Weapon/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyFPSGame/Enum/AllEnum.h"

void UMyAnimInstance::NativeInitializeAnimation(){
	Super::NativeInitializeAnimation();

	BaseCharacterPtr = Cast<ABaseCharacter>(TryGetPawnOwner());//尝试获取拥有者
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (BaseCharacterPtr == nullptr)BaseCharacterPtr = Cast<ABaseCharacter>(TryGetPawnOwner());//尝试获取拥有者
	if (BaseCharacterPtr == nullptr) return;

	FVector SpeedVector3D = BaseCharacterPtr->GetVelocity();//获取角色速度
	SpeedVector3D.Z = 0.f;
	Speed = SpeedVector3D.Size();

	bIsAir = BaseCharacterPtr->GetCharacterMovement()->IsFalling();
	bIsAccelerating = BaseCharacterPtr->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bIsJump = BaseCharacterPtr->GetIsJump();
	bIsCrouched = BaseCharacterPtr->bIsCrouched;
	bAiming = BaseCharacterPtr->GetAiming();
	AimYaw = BaseCharacterPtr->GetAimYaw();
	AimPitch = BaseCharacterPtr->GetAimPitch();
	//TurningInPlace = BaseCharacterPtr->GetTurningInpace();
	

	Weapon = BaseCharacterPtr->GetEquippedeWeapon();
	if (Weapon) {
		bWeaponIsValid = true;
		EquipmentType = Weapon->GetWeaponType();
	}
	else {
		bWeaponIsValid = false;
		EquipmentType = EEquipmentType::EET_Unarmed;
	}
		


	FRotator AimRotation = BaseCharacterPtr->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BaseCharacterPtr->GetVelocity());
	if (bAiming)
		Direction = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
	else
		Direction = 0.f;
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString(FString::Printf(TEXT("Direction  %f"), Direction)))
	//if (bWeaponEquipped) GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, TEXT("Is bWeaponEquipped"));




	if (EquipmentType != EEquipmentType::EET_Unarmed && Weapon && Weapon->GetWeaponMesh() && BaseCharacterPtr->GetMesh()) {
		LeftHandTransfrom = Weapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);//获取武器插槽在世界空间的位置
		FVector OutPosition;
		FRotator OutRotation;
		BaseCharacterPtr->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransfrom.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);//将武器插槽在世界空间的位置转换为骨骼空间
		LeftHandTransfrom.SetLocation(OutPosition);//存储起来给动画蓝图FABRIK用(反向动力学IK)
		LeftHandTransfrom.SetRotation(FQuat(OutRotation));

		if (BaseCharacterPtr->IsLocallyControlled()) {
			bLocallyControlled = true;
			FTransform RightHandTransfrom = BaseCharacterPtr->GetMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator RightHandRotationXYZ = UKismetMathLibrary::FindLookAtRotation(RightHandTransfrom.GetLocation(), RightHandTransfrom.GetLocation() + (RightHandTransfrom.GetLocation() - BaseCharacterPtr->GetHitTarget()));//使右手朝向命中目标
			FRotator LookAtRotation = FMath::RInterpTo(RightHandRotation, RightHandRotationXYZ, DeltaSeconds, 30.f);
			RightHandRotation = FRotator(LookAtRotation.Pitch, RightHandTransfrom.GetRotation().Rotator().Yaw,  RightHandTransfrom.GetRotation().Rotator().Roll);
		}
	
	}

	CombatState = BaseCharacterPtr->GetECombatState();
	bUseFABRIK = bUseAimOffsets = CombatState != ECombatState::ECS_Reloading;
	 
}
