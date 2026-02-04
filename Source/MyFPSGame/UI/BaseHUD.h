// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;
	FLinearColor CrosshairsColor;
	float CrosshairSpread;
};
/**
 * 
 */
UCLASS()
class MYFPSGAME_API ABaseHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;
	virtual void DrawHUD() override;
	void AddCharacterOverlay();
	void AddAnnouncement();
	void AddFactionSelection();

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	class UCharacterOverlay* CharacterOverlay;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> AnnouncenmentClass;

	class UAnnouncement* Announcement;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> FactionSelectionClass;

	class UFactionSelection* FactionSelection;

protected:
	virtual void BeginPlay() override;
	

private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairsColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
public:
	void SetHUDPackage(const FHUDPackage& Package);
	
};
