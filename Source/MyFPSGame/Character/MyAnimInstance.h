// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyFPSGame/Enum/AllEnum.h"
#include "MyAnimInstance.generated.h"



UCLASS()                                                                                                    
class MYFPSGAME_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
private:
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess="true"))
	class ABaseCharacter* BaseCharacterPtr;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;//速度

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAir;//坠落

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;//运动

	UPROPERTY(BlueprintReadOnly, Category = "MyValue", meta = (AllowPrivateAccess = "true"))
	bool bIsJump;//跳

	UPROPERTY(BlueprintReadOnly, Category = "MyValue", meta = (AllowPrivateAccess = "true"))
	class AWeapon* Weapon;//当前武器

	UPROPERTY(BlueprintReadOnly, Category = "MyValue", meta = (AllowPrivateAccess = "true"))
	bool bWeaponIsValid;//当前武器是否有效

	UPROPERTY(BlueprintReadOnly, Category = "MyValue", meta = (AllowPrivateAccess = "true"))
	EEquipmentType EquipmentType;//当前武器类型

	UPROPERTY(BlueprintReadOnly, Category = "MyValue", meta = (AllowPrivateAccess = "true"))
	bool bIsCrouched;//蹲

	UPROPERTY(BlueprintReadOnly, Category = "MyValue", meta = (AllowPrivateAccess = "true"))
	bool bAiming;//瞄准

	UPROPERTY(BlueprintReadOnly, Category = "MyValue", meta = (AllowPrivateAccess = "true"))
	float Direction;//方向

	UPROPERTY(BlueprintReadOnly, Category = "MyValue", meta = (AllowPrivateAccess = "true"))
	float AimYaw;//方向

	UPROPERTY(BlueprintReadOnly, Category = "MyValue", meta = (AllowPrivateAccess = "true"))
	float AimPitch;//方向

	UPROPERTY(BlueprintReadOnly, Category = "MyValue", meta = (AllowPrivateAccess = "true"))
	FTransform LeftHandTransfrom;//左手附着

	UPROPERTY(BlueprintReadOnly, Category = "MyValue", meta = (AllowPrivateAccess = "true"))
	FRotator RightHandRotation;

	UPROPERTY(BlueprintReadOnly, Category = "MyValue", meta = (AllowPrivateAccess = "true"))
	bool bLocallyControlled;

	UPROPERTY(BlueprintReadOnly, Category = "MyValue", meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	UPROPERTY(BlueprintReadOnly, Category = "MyValue", meta = (AllowPrivateAccess = "true"))
	bool bUseFABRIK;

	UPROPERTY(BlueprintReadOnly, Category = "MyValue", meta = (AllowPrivateAccess = "true"))
	bool bUseAimOffsets;
};
