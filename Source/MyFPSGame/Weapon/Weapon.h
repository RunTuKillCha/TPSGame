// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyFPSGame/Enum/AllEnum.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8 {
	EWS_Initial UMETA(DisplayName = "初始状态"),
	EWS_Equipped UMETA(DisplayName = "装备状态"),
	EWS_NoEquipped UMETA(DisplayName = "未装备状态"),
	EWS_Dropped UMETA(DisplayName = "丢弃状态"),

	EWS_Max UMETA(DisplayName = "枚举最大数值")
};

UCLASS()
class MYFPSGAME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	/*
		虚幻系统函数
	*/
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const	override;
	virtual void OnRep_Owner() override;
	/*
		自定义功能函数
	*/
	void ShowPickupWidget(bool bShowWidget);//显示拾取
	virtual void Fire(const FVector& HitTarget);
	virtual void Reload();
	void Dropped();
	bool IsEmpty();

	/*
		自定义Set函数
	*/
	void SetWeaponState(EWeaponState State);
	void SetHUDAmmo();
	void SetHUDCarryAmmo();
	void SetAmmo(int32 SetAmmo) { Ammo = SetAmmo; }
	void SetCarryAmmo(int32 SetCarryAmmo) { CarryAmmo = SetCarryAmmo; }

	/*
		自定义Get函数
	*/
	EEquipmentType GetWeaponType();

	USkeletalMeshComponent* GetWeaponMesh();
	float GetZoomedFOV() const;
	float GetZoomedInterpSpeed() const;
	float GetBaseCrosshairSpreads() const;
	float GetFireDelay();
	bool GetbAutomatic();
	float GetDamage();
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapcity() const { return MagCapacity; }
	FORCEINLINE int32 GetCarryAmmo() const { return CarryAmmo; }

	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(//重叠事件
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap(//结束重叠事件
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

private:
	/*
		组件及其资源引用
	*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;//武器网格体
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;//重叠区域

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;//拾取部件

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;//开火动画

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* ReloadAnimation;//换弹动画

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;//蛋壳网格

	/*
		自定义参数
	*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties", ReplicatedUsing = WeaponStateUpData)
	EWeaponState WeaponState;//武器状态

	UPROPERTY(EditAnywhere, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EEquipmentType WeaponType;//武器类型

	UPROPERTY(EditAnywhere, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float BaseCrosshairSpreads;
	
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomedInterpSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float WeaponRateOfFire = 600;

	float FireDelay;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bAutomatic = true;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo = 30;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_CarryAmmo)
	int32 CarryAmmo = 120;

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;


	UPROPERTY()
	class ABaseCharacter* OwnerCharacter;

	UPROPERTY()
	class ABasePlayerController* OwnerController;




	void SpendRound();

	UFUNCTION()
	void OnRep_Ammo();

	UFUNCTION()
	void OnRep_CarryAmmo();

	UFUNCTION()
	void WeaponStateUpData();
	
};
