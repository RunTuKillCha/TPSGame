// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

//组件
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "EnhancedInputComponent.h"//增强输入组件

#include "MyFPSGame/Weapon/Weapon.h"
#include "MyFPSGame/PlayerController/BasePlayerController.h"
#include "MyFPSGame/MyFPSGame.h"
#include "MyFPSGame/GameMode/ServerGameMode.h"
#include "MyFPSGame/PlayerState/BasePlayerState.h"
#include "Camera/CameraActor.h"

#include "Net/UnrealNetwork.h"//属性复制宏头文件

//子系统
#include "EnhancedInputSubsystems.h"//增强输入子系统

//动画
#include "MyAnimInstance.h"

//数学库
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#define MAX_WALK_SPEED 450.f
#define MAX_RUN_SPEED 450.f
#define MAX_AIM_SPEED 160.f
#define MAX_CROUCH_SPEED 250.f
#define MAX_PRONE_SPEED 65.f

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	MainSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("MainSpringArm"));
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	NameWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameWidget"));

	//RootComponent = XXX;设置根组件为XXX
	MainSpringArm->SetupAttachment(GetRootComponent());//设置MainSpringArm的根组件为RootComponent
	MainCamera->SetupAttachment(MainSpringArm);
	NameWidget->SetupAttachment(GetRootComponent());
	NameWidget->SetVisibility(false);
	//MainHand_StaticMesh->SetupAttachment(GetMesh(), TEXT("hand_r插槽"));

	//设置标签
	MainCamera->ComponentTags.Add(FName(TEXT("MyCamera")));



	

	//GetMesh();获取并校准网格体
	FQuat quat = GetMesh()->GetComponentQuat();//GetComponentQuat()返回组件在世界空间中旋转四元数
	//GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -80.0f), quat * FQuat(FVector(0.f, 0.f, 1.f), -PI / 2.f));//SetRelativeLocationAndRotation设置相对坐标和旋转
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);
	//设置类基础属性
	//GetCharacterMovement()等于GetMovementComponent()
	GetCharacterMovement()->bOrientRotationToMovement = true;//设置角色朝向旋转运动方向
	GetCharacterMovement()->RotationRate = FRotator(0.f, 900.f, 0.f);//设置角色Yaw轴旋转速率为800
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;//设置角色可蹲伏
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;//蹲伏时可走出平台
	GetCharacterMovement()->MaxWalkSpeedCrouched = MAX_CROUCH_SPEED;//设置蹲伏时最大速度
	GetCharacterMovement()->SetCrouchedHalfHeight(60.f);//设置蹲伏半高为60
	GetCharacterMovement()->MaxWalkSpeed = MAX_WALK_SPEED;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	MainSpringArm->bUsePawnControlRotation = true;//使用相机控制器控制旋转
	MainSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 60.f));//设置弹簧臂相对于胶囊体位置
	MainSpringArm->TargetArmLength = 150.f;//设置弹簧臂长度
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	//初始化自定义属性
	AimYaw = AimPitch = 0.f;
	StopJumpDelay = FLatentActionInfo(0, FMath::Rand(), TEXT("StopJump"), this);//延时

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	Combat->SetIsReplicated(true);//组件复制


	NetUpdateFrequency = 128.f;
	MinNetUpdateFrequency = 64.f;
	//静态加载资源
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh>TempSkeletalMesh(TEXT("/Script/Engine.StaticMesh'/Game/枪/M4/20.20'"));
	// 动态加载资源
	//MainMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/InputMap.InputMap'"));
	//USkeletalMesh* TempSkeletalMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Script/Engine.SkeletalMesh'/Game/TPSFPSCharacterSystemV2/Demo/Characters/TP_Mannequin_FullBody/Mesh/SK_TP_Mannequin_FullBody_Mesh.SK_TP_Mannequin_FullBody_Mesh'"));
	//GetMesh()->SetSkeletalMeshAsset(TempSkeletalMesh);
	
}

ABaseCharacter::~ABaseCharacter(){
	if (Combat && Combat->EquippedWeapon) {
		Combat->EquippedWeapon->Destroy();
		//Combat->DroppedWeapon(Combat->EquippedWeapon);
	}
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME_CONDITION		
	//COND_InitialOnly					该属性仅在初始数据组尝试发送		
	//COND_OwnerOnly					该属性仅发送至 actor 的所有者
	//COND_SkipOwner					该属性将发送至除所有者之外的每个连接
	//COND_SimulatedOnly				该属性仅发送至模拟 actor
	//COND_AutonomousOnly				该属性仅发送给自治 actor
	//COND_SimulatedOrPhysics			该属性将发送至模拟或 bRepPhysics actor
	//COND_InitialOrOwner				该属性将发送初始数据包，或者发送至 actor 所有者
	//COND_Custom						该属性没有特定条件，但需要通过 SetCustomIsActiveOverride 得到开启 / 关闭能力
	//DOREPLIFETIME(ABaseCharacter, OverlappingWeapon);//同步方式
	DOREPLIFETIME_CONDITION(ABaseCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABaseCharacter, AimYaw, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ABaseCharacter, AimPitch, COND_SimulatedOnly);
	DOREPLIFETIME(ABaseCharacter, Health);
	DOREPLIFETIME(ABaseCharacter, MaxWalkSpeed);
	DOREPLIFETIME(ABaseCharacter, bDisableCharacterMove);
	DOREPLIFETIME(ABaseCharacter, bCharacterHidden);
	
	
}

void ABaseCharacter::PostInitializeComponents(){
	Super::PostInitializeComponents();
	if (Combat) {
		Combat->Character = this;
		if (HasAuthority()) {
			//Combat->AllWeapons = new FAllWeapons;
		}
	}
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {//获取本地玩家控制器
		//将本地控制器转换为增强输入本地玩家子系统
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(MainMappingContext, 0);
		}
	}

	if (HasAuthority()) {
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}

	UpdateHUDGealth();
	if (Combat && Combat->EquippedWeapon == nullptr) {
		BasePlayerController = BasePlayerController == nullptr ? Cast<ABasePlayerController>(Controller) : BasePlayerController;
		if (BasePlayerController) {
			BasePlayerController->SetHUDWeaponAmmo(0);
			BasePlayerController->SetHUDCarryAmmo(0);
		}
	}

}

void ABaseCharacter::MulticastSetMesh_Implementation(EFaction InFaction) {
	//UE_LOG(LogTemp, Warning, TEXT("MulticastSetMesh_Implementation"));
	switch (InFaction) {
	case EFaction::EF_NULL:
		//UE_LOG(LogTemp, Warning, TEXT("NULL_Faction_Mesh"));
		if (NULL_Faction_Mesh) {
			GetMesh()->SetSkeletalMeshAsset(NULL_Faction_Mesh);
		}
		break;
	case EFaction::EF_CT:
		//UE_LOG(LogTemp, Warning, TEXT("CT_Faction_Mesh"));
		if (CT_Faction_Mesh) {
			GetMesh()->SetSkeletalMeshAsset(CT_Faction_Mesh);
		}
		break;
	case EFaction::EF_T:
		//UE_LOG(LogTemp, Warning, TEXT("T_Faction_Mesh"));
		if (T_Faction_Mesh) {
			GetMesh()->SetSkeletalMeshAsset(T_Faction_Mesh);
		}
		break;
	}

}




void ABaseCharacter::PollInit(){
	BasePlayerState = BasePlayerState == nullptr ? GetPlayerState<ABasePlayerState>() : BasePlayerState;
	if (BasePlayerState) {
		BasePlayerState->AddToScore(0.f);
		BasePlayerState->AddToDefeats(0);
		
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);
	HideCameraIfCharacterColse();

	if (Controller != nullptr) {
		ControllerRotator = Controller->GetControlRotation();
		VectorX = FRotationMatrix(FRotator(0, ControllerRotator.Yaw, 0)).GetUnitAxis(EAxis::X);//获取向前向量
		VectorY = FRotationMatrix(FRotator(0, ControllerRotator.Yaw, 0)).GetUnitAxis(EAxis::Y);//获取向右向量
	}

	if (BasePlayerController && Combat) {//将控制器转换为ABasePlayerController控制器
		
		BasePlayerController->SetHUDWeapon(Combat->EquippedWeapon != nullptr ? true : false, Combat->MainWeapon != nullptr ? true : false);
	}

	BasePlayerController = BasePlayerController == nullptr ? Cast<ABasePlayerController>(Controller) : BasePlayerController;
	if (!HasAuthority() && GetMesh() && BasePlayerController && BasePlayerController->bChooseFaction == false) {
		BasePlayerController->ServerDebug();
		BasePlayerController->bChooseFaction = true;
		BasePlayerController->SetInputMode(FInputModeUIOnly());
		BasePlayerController->SetShowMouseCursor(true);
	}
	

	

	PollInit();
}

void ABaseCharacter::PlayFireMontage(bool bAiming) {
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage) {
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName = FName("RifleAim");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}


void ABaseCharacter::PlayReloadMontage(){
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadWeaponMontage) {
		AnimInstance->Montage_Play(ReloadWeaponMontage);
		FName SectionName = FName("Reload");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABaseCharacter::HideCameraIfCharacterColse(){
	if (IsLocallyControlled()) {
		if ((MainCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold) {
			GetMesh()->SetVisibility(false);
			if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh()) {
				Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
			}
		}
		else {
			GetMesh()->SetVisibility(true);
			if (Combat && Combat->EquippedWeapon) {
				Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
			}
		}
	}
}



void ABaseCharacter::AimOffset(float DeltaTime) {
	if (!Combat) return;

	//if (StartingAimRotation.Yaw == 0.f) StartingAimRotation = StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
	FRotator AimRotator = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation());
	FRotator MyAimRotator = FRotator(AimPitch, AimYaw, 0.f);
	MyAimRotator = UKismetMathLibrary::RInterpTo(MyAimRotator, AimRotator, DeltaTime, 10.f);

	if (Combat->bAiming) {
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else {
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}


	ENetRole LocalRole = GetLocalRole();//获取角色所有权
	switch (LocalRole){
	case ROLE_Authority://如果是服务器或者自治代理则同步
	case ROLE_AutonomousProxy:
		if (Combat->bAiming)AimPitch = MyAimRotator.Pitch;
		else AimPitch = 0.f;
		break;

	case ROLE_SimulatedProxy://如果是模拟代理则等待服务器同步
		break;
	}
	
	if (AimPitch > 90.f && !IsLocallyControlled()) {//转换回-90 - 90 
		//源码CharacterMovementComponent.cpp GetPackedAngles()函数
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AimPitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimPitch);
	}
	
}
void ABaseCharacter::Jump(){
	if (bIsCrouched) {
		UnCrouch();
	}
	else {
		Super::Jump();
	}
}

void ABaseCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatoContoller, AActor* DamageCauser){
	if (bElim) return;
	ABasePlayerController* AttackController = Cast<ABasePlayerController>(InstigatoContoller);
	if (AttackController) {
		ABasePlayerState* AttackPlayerState = Cast<ABasePlayerState>(AttackController->PlayerState);
		if (AttackPlayerState && BasePlayerState) {
			if (AttackPlayerState->GetFaction() == BasePlayerState->GetFaction()) {
				return;
			}
			Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);

			if (Health <= 0.f) {
				bElim = true;
				AServerGameMode* ServerGameMode = GetWorld()->GetAuthGameMode<AServerGameMode>();
				if (ServerGameMode) {
					BasePlayerController = BasePlayerController == nullptr ? Cast<ABasePlayerController>(Controller) : BasePlayerController;
					ServerGameMode->PlayerEliminated(this, BasePlayerController, AttackController);
					GetMesh()->SetSimulatePhysics(true);
				}
			}

		}
	}
	
}

void ABaseCharacter::OnRep_Health() {

	if (Health <= 0.f) {
		GetMesh()->SetSimulatePhysics(true);
	}

	UpdateHUDGealth();//更新HUD中的健康
}

void ABaseCharacter::OnRep_MaxWalkSpeed(){
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void ABaseCharacter::UpdateHUDGealth() {
	BasePlayerController = BasePlayerController == nullptr ? Cast<ABasePlayerController>(Controller) : BasePlayerController;
	if (BasePlayerController) {//将控制器转换为ABasePlayerController控制器
		BasePlayerController->SetHUDHealth(Health, MaxHealth);//设置血量
	}
}

void ABaseCharacter::Elim() {
	
	if (Combat && Combat->EquippedWeapon) {
		Combat->DroppedWeapon(Combat->EquippedWeapon);

	}

	MulticastElim();

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	if (BasePlayerController) {
		DisableInput(BasePlayerController);
	}
}

void ABaseCharacter::MulticastElim_Implementation() {

	if (BasePlayerController) {
		DisableInput(BasePlayerController);
	}
}













// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {//绑定输入Action和触发函数以及触发方式
		if (MoveAction)							EnhancedInputComponent->BindAction(MoveAction,					ETriggerEvent::Triggered, this, &ABaseCharacter::StartMove);
		if (RetreatAction)						EnhancedInputComponent->BindAction(RetreatAction,				ETriggerEvent::Triggered, this, &ABaseCharacter::StartRetreat);
		if (LeftAction)							EnhancedInputComponent->BindAction(LeftAction,					ETriggerEvent::Triggered, this, &ABaseCharacter::StartLeft);
		if (RightAction)						EnhancedInputComponent->BindAction(RightAction,					ETriggerEvent::Triggered, this, &ABaseCharacter::StartRight);
		if (LookAction)							EnhancedInputComponent->BindAction(LookAction,					ETriggerEvent::Triggered, this, &ABaseCharacter::Look);
		if (AimAction)							EnhancedInputComponent->BindAction(AimAction,					ETriggerEvent::Triggered, this, &ABaseCharacter::IsAim);
		if (CameraScaledAction)					EnhancedInputComponent->BindAction(CameraScaledAction,			ETriggerEvent::Triggered, this, &ABaseCharacter::CameraScaled);
		if (JumpAction)							EnhancedInputComponent->BindAction(JumpAction,					ETriggerEvent::Triggered, this, &ABaseCharacter::CharacterJump);
		if (UserAction)							EnhancedInputComponent->BindAction(UserAction,					ETriggerEvent::Triggered, this, &ABaseCharacter::EquipButton);
		if (CrouchAction)						EnhancedInputComponent->BindAction(CrouchAction,				ETriggerEvent::Triggered, this, &ABaseCharacter::CharacterCrouch);
		if (Num1Action)							EnhancedInputComponent->BindAction(Num1Action,					ETriggerEvent::Triggered, this, &ABaseCharacter::Num1);
		if (FireButtonAction)					EnhancedInputComponent->BindAction(FireButtonAction,			ETriggerEvent::Triggered, this, &ABaseCharacter::FireButtonPressed);
		if (DroppedAction)						EnhancedInputComponent->BindAction(DroppedAction,				ETriggerEvent::Triggered, this, &ABaseCharacter::DroppedPressed);
		if (ReloadAction)						EnhancedInputComponent->BindAction(ReloadAction,				ETriggerEvent::Triggered, this, &ABaseCharacter::ReloadButtonPressed);
		
	}
}


void ABaseCharacter::StartMove(const FInputActionValue& Vaule) {
	if (bDisableCharacterMove) return;
	if (Controller != nullptr)	AddMovementInput(VectorX, Vaule.Get<float>());
}
void ABaseCharacter::StartRetreat(const FInputActionValue& Vaule) {
	if (bDisableCharacterMove) return;
	if (Controller != nullptr)	AddMovementInput(VectorX, Vaule.Get<float>());
}
void ABaseCharacter::StartLeft(const FInputActionValue& Vaule) {
	if (bDisableCharacterMove) return;
	if (Controller != nullptr)	AddMovementInput(VectorY, Vaule.Get<float>());
}
void ABaseCharacter::StartRight(const FInputActionValue& Vaule) {
	if (bDisableCharacterMove) return;
	if (Controller != nullptr)	AddMovementInput(VectorY, Vaule.Get<float>());
}

void ABaseCharacter::Look(const FInputActionValue& Vaule) {
	FVector2D LookVector = Vaule.Get<FVector2D>();
	if (Controller != nullptr) {
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}

}
void ABaseCharacter::CameraScaled(const FInputActionValue& Vaule){
	float MouseValue = 0.f - Vaule.Get<float>();
	//FString DebugStr = FString::SanitizeFloat(MouseValue);
	if ((MainSpringArm->TargetArmLength > 114.f && MouseValue < 0) ||
		(MainSpringArm->TargetArmLength < 400.f && MouseValue>0))
		MainSpringArm->TargetArmLength += MouseValue;

}

void ABaseCharacter::CharacterJump(const FInputActionValue& Vaule){
	if (bDisableCharacterMove) return;
	if (bIsCrouched) {
		UnCrouch();
		bIsJump = true;
		Jump();
		// Delay  X秒后延迟后执行DelayFinish方法
		UKismetSystemLibrary::Delay(this, 0.03f, StopJumpDelay);
		//Super::Crouch();
	}
	else {
		bIsJump = true;
		Jump();
		// Delay  X秒后延迟后执行DelayFinish方法
		UKismetSystemLibrary::Delay(this, 0.03f, StopJumpDelay);
	}

}
void ABaseCharacter::StopJump(){
	bIsJump = false;
	StopJumping();
}

void ABaseCharacter::EquipButton(const FInputActionValue& Vaule){
	if (Combat) {
		if (HasAuthority())
			Combat->EquipWeapon(OverlappingWeapon);
		else
			ServerEquipButtorPressed();
	}
}

void ABaseCharacter::ServerEquipButtorPressed_Implementation() {//服务器RPC
	if (Combat) {
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ABaseCharacter::DroppedPressed(const FInputActionValue& Vaule) {
	if (Vaule.Get<bool>() && Combat && Combat->EquippedWeapon) {
		if (HasAuthority())
			Combat->DroppedWeapon(Combat->EquippedWeapon);
		else
			ServerDroppedPressed();
		
	}
}


void ABaseCharacter::ServerDroppedPressed_Implementation(){
	if (Combat && Combat->EquippedWeapon) {
		Combat->DroppedWeapon(Combat->EquippedWeapon);
	}
}

void ABaseCharacter::ReloadButtonPressed(const FInputActionValue& Vaule) {
	if (bDisableCharacterMove) return;
	if (Vaule.Get<bool>() && Combat) {
		Combat->ReloadButtonPressed();
	}
}

void ABaseCharacter::CharacterCrouch(const FInputActionValue& Vaule){
	if (bDisableCharacterMove) return;
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Crouch"));
	if (Vaule.Get<float>()) {
		Crouch();
	}
	else{
		UnCrouch();
	}
	
}

void ABaseCharacter::Num1(const FInputActionValue& Vaule){
	if (Vaule.Get<bool>() && BasePlayerController && !HasAuthority()){
		BasePlayerController->ServerDebug();
		BasePlayerController->SetInputMode(FInputModeUIOnly());
		BasePlayerController->SetShowMouseCursor(true);
	}
	

	if(Combat->EquippedWeapon)
	{
		//int32 a = Combat->EquippedWeapon->GetWeaponType(0);
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("a=%d"), a));
	}
}

void ABaseCharacter::FireButtonPressed(const FInputActionValue& Vaule){
	if (bDisableCharacterMove) return;
	if (Combat) {
		Combat->FireButtonPressed(Vaule.Get<bool>());
		//UE_LOG(LogTemp, Warning, TEXT("%d"), Vaule.Get<bool>());
	}
}

void ABaseCharacter::IsAim(const FInputActionValue& Vaule) {
	if (Combat) {
		Combat->SetAiming(Vaule.Get<bool>());
		if (HasAuthority()) {
			
			if (Vaule.Get<bool>()) {
				GetCharacterMovement()->MaxWalkSpeed = MAX_AIM_SPEED;
				MaxWalkSpeed = MAX_AIM_SPEED;
			}
			else {
				GetCharacterMovement()->MaxWalkSpeed = MAX_WALK_SPEED;
				MaxWalkSpeed = MAX_WALK_SPEED;
			}
		}
		else {
			ServerAiming(Vaule.Get<bool>());
		}
	}
	
}

void ABaseCharacter::ServerAiming_Implementation(bool bAiming) {
	if (Combat) {
			if (bAiming) {
				GetCharacterMovement()->MaxWalkSpeed = MAX_AIM_SPEED;
				MaxWalkSpeed = MAX_AIM_SPEED;
			}
			else {
				GetCharacterMovement()->MaxWalkSpeed = MAX_WALK_SPEED;
				MaxWalkSpeed = MAX_WALK_SPEED;
			}
		}

}

void ABaseCharacter::Show_Weapon(AWeapon* LastWeaon){
	//如果有效则显示拾取部件
	if (OverlappingWeapon)	OverlappingWeapon->ShowPickupWidget(true);
	//如果上一次更新有本次无效则隐藏
	else if (LastWeaon)	LastWeaon->ShowPickupWidget(false);
}


void ABaseCharacter::SetOverlappingWeapon(AWeapon* Weapon){
	//OverlappingWeapon有效传入空则需要服务器隐藏
	if (OverlappingWeapon && !Weapon)	OverlappingWeapon->ShowPickupWidget(false);
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled() && OverlappingWeapon) OverlappingWeapon->ShowPickupWidget(true);
}



AWeapon* ABaseCharacter::GetEquippedeWeapon(){
	if (Combat&& Combat->EquippedWeapon)
		return Combat->EquippedWeapon;
	else
		return nullptr;
}

bool ABaseCharacter::GetIsJump(){
	return bIsJump;
}

bool ABaseCharacter::GetAiming(){
	return (Combat && Combat->bAiming);
}

float ABaseCharacter::GetAimYaw(){
	return AimYaw;
}

float ABaseCharacter::GetAimPitch(){
	return AimPitch;
}



FVector ABaseCharacter::GetHitTarget() const{
	if(Combat == nullptr)
		return FVector();
	return Combat->HitTarget;
}

UCameraComponent* ABaseCharacter::GetCamera() const{
	return MainCamera;
}

ECombatState ABaseCharacter::GetECombatState(){
	if (Combat) {
		return Combat->CombatState;
	}

	return ECombatState::ECS_Max;
}

UCombatComponent* ABaseCharacter::GetCombat() const
{
	return Combat;
}

float ABaseCharacter::GetHealth() const
{
	return Health;
}

void ABaseCharacter::OnRep_bCharacterHidden(){
	//GetMesh()->SetHiddenInGame(bCharacterHidden);
	SetActorHiddenInGame(bCharacterHidden);
	if (Combat && Combat->EquippedWeapon) {
		Combat->EquippedWeapon->SetActorHiddenInGame(bCharacterHidden);
	}
}