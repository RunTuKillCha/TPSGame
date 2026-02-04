 #include "BaseHUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include "Announcement.h"
#include "FactionSelection.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void ABaseHUD::BeginPlay() {
	Super::BeginPlay();

	AddCharacterOverlay();
	AddAnnouncement();
	AddFactionSelection();

	CharacterOverlay->TimeRemaining->SetVisibility(ESlateVisibility::Hidden);
}

void ABaseHUD::AddCharacterOverlay() {
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass) {
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void ABaseHUD::AddAnnouncement(){
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && AnnouncenmentClass) {
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncenmentClass);
		Announcement->AddToViewport();
	}
}

void ABaseHUD::AddFactionSelection(){
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && FactionSelectionClass) {
		FactionSelection = CreateWidget<UFactionSelection>(PlayerController, FactionSelectionClass);
		FactionSelection->AddToViewport();
		FactionSelection->SetIsFocusable(true);
		FactionSelection->SetVisibility(ESlateVisibility::Hidden);
	}
}


void ABaseHUD::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	//FactionSelection->CTButton->IsHovered()
}

void ABaseHUD::DrawHUD(){
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);//获取视口大小
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);//视口中心点
		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairsCenter) {
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, FVector2D(0.f, 0.f), HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsCenter) {
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, FVector2D(-SpreadScaled, 0.f), HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsCenter) {
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, FVector2D(SpreadScaled, 0.f), HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsCenter) {
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, FVector2D(0.f, -SpreadScaled), HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsCenter) {
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, FVector2D(0.f, SpreadScaled), HUDPackage.CrosshairsColor);
		}
	}
	

}

void ABaseHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairsColor){
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);
	DrawTexture(//绘制
		Texture,//纹理
		TextureDrawPoint.X,//位置
		TextureDrawPoint.Y,
		TextureWidth,//尺寸
		TextureHeight,
		0.f, 0.f,
		1.f, 1.f,
		CrosshairsColor//颜色
	);
}

void ABaseHUD::SetHUDPackage(const FHUDPackage& Package){
	HUDPackage = Package;
}
