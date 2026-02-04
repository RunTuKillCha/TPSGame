// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "MyFPSGame/Character/BaseCharacter.h"
#include "Projectile.h"
#include "Casing.h"

void AProjectileWeapon::Fire(const FVector& HitTarget){
	Super::Fire(HitTarget);

	

	if (!HasAuthority()) return;

	//刷新服务器姿势
	ABaseCharacter* CharacterPtr = Cast<ABaseCharacter>(GetOwner());
	if (CharacterPtr && CharacterPtr->GetMesh()) {
		CharacterPtr->GetMesh()->TickPose(0.f, false);
		CharacterPtr->GetMesh()->RefreshBoneTransforms();
	}
	//刷新武器姿势
	GetWeaponMesh()->TickPose(0.f, false);
	GetWeaponMesh()->RefreshBoneTransforms();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket) {
		//使用网格需要头文件
		FTransform SocketTransfrom = GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);//获取插槽变换
		//DrawDebugSphere(GetWorld(), SocketTransfrom.GetLocation(), 5.f, 32, FColor::Red);
		FVector ToTarget = HitTarget - SocketTransfrom.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		if (ProjectileClass && CharacterPtr) {
			UWorld* World = GetWorld();
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;//所有者为武器
			SpawnParameters.Instigator = CharacterPtr;//发起者为玩家
			if (World) {
				World->SpawnActor<AProjectile>(
					ProjectileClass,
					SocketTransfrom.GetLocation(),
					TargetRotation,
					SpawnParameters
				);
			}
		}
	}
}
