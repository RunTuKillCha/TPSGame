// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyFPSGame/UI/BaseHUD.h"
#include "MyFPSGame/Enum/AllEnum.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 99999.f

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYFPSGAME_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ABaseCharacter;//声明友元类
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const	override;

	void EquipWeapon(class AWeapon* WeaponToEquip);
	void DroppedWeapon(AWeapon* WeaponToDropped);

	UFUNCTION(Server, Reliable)
	void FireButtonPressed(bool bPressed);

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	//void SwitchWeapon(EWeaponType WeaponToSwitch);

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsCenter;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsBottom;
	
protected:
	virtual void BeginPlay() override;

	void GenerateDefaultLoadout();


	void SetAiming(bool bIsAiming);
	void SetHUDCrosshairs(float DeltaTime);


	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void ReloadButtonPressed();
	void Fire();
	int32 AmountToReload();

	UFUNCTION(Server, Reliable)
	void Server_SetAiming(bool bIsAiming);
	UFUNCTION(NetMulticast, Reliable)
	void OtherClient_SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetHitTarget(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable)
	void ServerReload();

private:

	/*
		组件及其资源引用
	*/
	UPROPERTY()
	class ABaseCharacter* Character;
	UPROPERTY()
	class ABasePlayerController* Controller;
	UPROPERTY()
	class ABaseHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	AWeapon* MainWeapon;

	UPROPERTY(Replicated)
	AWeapon* Pistol;

	UPROPERTY(Replicated)
	AWeapon* Knife;

	UPROPERTY(Replicated)
	AWeapon* C4;

	UPROPERTY(EditAnywhere, Category = "DefaultCreateWeapons")
	TSubclassOf<class AWeapon> DefaultCreateMainWeapon;

	/*
		自定义参数
	*/
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState;

	UPROPERTY()
	FVector HitTarget;//命中点
	UPROPERTY()
	FHUDPackage HUDPackage;
	UPROPERTY()
	FTimerHandle FireTime;

	UPROPERTY(Replicated,BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bAiming;//是否瞄准
	UPROPERTY(Replicated)
	bool bFireButtonPressed;//是否开火
	

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFov = 30.f;//放大视角

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;//放大速度

	UPROPERTY()
	float DefaultFOV;//默认视角
	UPROPERTY()
	float CurrentFOV;//当前视角

	UPROPERTY()
	float BaseCrosshairSpreads;
	UPROPERTY()
	float CrosshairVelocityFactor;
	UPROPERTY()
	float CrosshairInAirFactor;
	UPROPERTY()
	float CrosshairAimFactor;
	UPROPERTY()
	float CrosshairShootingFactor;
	UPROPERTY()
	bool bCanFire = true;


	

	void StartFireTimer();
	void FireTimerFinished();
	void InterpFOV(float DeltaTime);
	bool CanFire();
	void UpdateAmmoValues();

	/*
		复制回调
	*/
	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_CombatState();
};
