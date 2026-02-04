// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "MyFPSGame/Component/CombatComponent.h"
#include "MyFPSGame/Interfaces/InteractWithCrosshairsInterface.h"
#include "MyFPSGame/Enum/AllEnum.h"
#include "BaseCharacter.generated.h"

UENUM(BlueprintType)
enum class EEquippedWeapon : uint8 {
	EWS_MainWeapon UMETA(DisplayName = "主武器"),
	EWS_Pistol UMETA(DisplayName = "手枪"),
	EWS_Knife UMETA(DisplayName = "刀"),
	EWS_Hand UMETA(DisplayName = "手"),
	EWS_C4 UMETA(DisplayName = "C4"),

	EWS_Max UMETA(DisplayName = "枚举最大数值")
};

UCLASS()
class MYFPSGAME_API ABaseCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	/*
		虚幻系统函数
	*/
	ABaseCharacter();
	~ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const	override;
	virtual void PostInitializeComponents()	override;

	/*
		自定义功能函数
	*/
	UFUNCTION()
	void Show_Weapon(AWeapon* LastWeaon);
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void Elim();
	void HideCameraIfCharacterColse();//隐藏角色和武器

	UFUNCTION(NetMulticast,Reliable)
	void MulticastElim();//角色死亡

	UFUNCTION(NetMulticast,Reliable)
	void MulticastSetMesh(EFaction InFaction);

	/*
		自定义Set函数
	*/
	void SetOverlappingWeapon(AWeapon* Weapon);//将触发了重叠的武器传递给角色


	/*
		自定义Get函数
	*/
	bool GetIsJump();
	bool GetAiming();
	float GetAimYaw();
	float GetAimPitch();

	AWeapon* GetEquippedeWeapon();
	FVector GetHitTarget() const;
	class UCameraComponent* GetCamera() const;
	ECombatState GetECombatState();
	class UCombatComponent* GetCombat() const;

	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UPROPERTY(ReplicatedUsing = OnRep_bCharacterHidden)
	bool bCharacterHidden;

protected:
	virtual void BeginPlay() override;
	void PollInit();//初始化HUD
	/*
		操作回调函数
	*/
	void StartMove(const FInputActionValue& Vaule);
	void StartRetreat(const FInputActionValue& Vaule);
	void StartLeft(const FInputActionValue& Vaule);
	void StartRight(const FInputActionValue& Vaule);
	void IsAim(const FInputActionValue& Vaule);
	void Look(const FInputActionValue& Vaule);
	void CameraScaled(const FInputActionValue& Vaule);
	void CharacterJump(const FInputActionValue& Vaule);
	UFUNCTION()
	void StopJump();
	void CharacterCrouch(const FInputActionValue& Vaule);

	void EquipButton(const FInputActionValue& Vaule);
	void Num1(const FInputActionValue& Vaule);
	void FireButtonPressed(const FInputActionValue& Vaule);
	void DroppedPressed(const FInputActionValue& Vaule);
	void ReloadButtonPressed(const FInputActionValue& Vaule);

	void AimOffset(float DeltaTime);
	virtual void Jump() override;

	//更新HUD健康
	void UpdateHUDGealth();

	//伤害回调函数
	UFUNCTION()
	void ReceiveDamage(//应用伤害函数
		AActor* DamagedActor, 
		float Damage, 
		const UDamageType* DamageType, 
		class AController* InstigatoContoller, 
		AActor* DamageCauser
	);
	
private:

	/*
		组件及其资源引用
	*/
	UPROPERTY(VisibleAnywhere, Category = "MySceneComponent")
	class USpringArmComponent* MainSpringArm;//声明弹簧臂组件
	UPROPERTY(VisibleAnywhere, Category = "MySceneComponent")
	class UCameraComponent* MainCamera;//声明摄像机组件
	UPROPERTY(VisibleAnywhere, Category = "MySceneComponent", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* NameWidget;//声明小窗口
	UPROPERTY(ReplicatedUsing = Show_Weapon)
	class AWeapon* OverlappingWeapon;//发生重叠的武器
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ReloadWeaponMontage;
	UPROPERTY()
	class ABasePlayerController* BasePlayerController;
	UPROPERTY()
	class ABasePlayerState* BasePlayerState;

	UPROPERTY(EditAnywhere, Category = "FactionMesh")
	class USkeletalMesh* NULL_Faction_Mesh;
	UPROPERTY(EditAnywhere, Category = "FactionMesh")
	class USkeletalMesh* CT_Faction_Mesh;
	UPROPERTY(EditAnywhere, Category = "FactionMesh")
	class USkeletalMesh* T_Faction_Mesh;
	
	/*
		自定义参数
	*/
	FLatentActionInfo StopJumpDelay;//跳跃延时
	FRotator ControllerRotator;//控制器旋转
	FVector VectorX;//X方向移动
	FVector VectorY;//Y方向移动

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	bool bIsJump;//是否跳跃
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats", meta = (AllowPrivateAccess = "true"))
	bool bIsAim;//是否瞄准
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Replicated)
	float AimYaw;//瞄准Yaw
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Replicated)
	float AimPitch;//瞄准Pitch

	UPROPERTY(ReplicatedUsing = OnRep_MaxWalkSpeed)
	float MaxWalkSpeed;

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 100.f;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Health, Category = "Player Stats")
	float Health = 100.f;

	bool bElim;



	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_MaxWalkSpeed();

	UFUNCTION()
	void OnRep_bCharacterHidden();


private:
	

	UFUNCTION(Server,Reliable)//此为RPC函数,不需要定义
	void ServerEquipButtorPressed();

	UFUNCTION(Server, Reliable)//此为RPC函数,不需要定义
	void ServerDroppedPressed();

	UFUNCTION(Server, Reliable)//此为RPC函数,不需要定义
	void ServerAiming(bool bAiming);


public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "映射上下文"))
	class	 UInputMappingContext* MainMappingContext;//声明映射上下文
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "角色前进"))
	class	 UInputAction* MoveAction;//角色前进
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "角色后退"))
	UInputAction* RetreatAction;//角色后退
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "角色向左"))
	UInputAction* LeftAction;//角色向左
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "角色向右"))
	UInputAction* RightAction;//角色向右
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "角色视角控制"))
	UInputAction* LookAction;//角色视角控制
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "角色视角缩放"))
	UInputAction* CameraScaledAction;//角色视角缩放
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "跳"))
	UInputAction* JumpAction;//跳
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "使用"))
	UInputAction* UserAction;//使用
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "蹲伏"))
	UInputAction* CrouchAction;//蹲伏
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "静步"))
	UInputAction* QuietAction;//静步
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "跑"))
	UInputAction* RunAction;//跑
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "瞄准"))
	UInputAction* AimAction;//瞄准
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "1"))
	UInputAction* Num1Action;//1
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "开火"))
	UInputAction* FireButtonAction;//Fire
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "丢弃"))
	UInputAction* DroppedAction;//Dropped
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (ToolTip = "换弹"))
	UInputAction* ReloadAction;//换弹

	UPROPERTY(Replicated)
	bool bDisableCharacterMove = false;

};

