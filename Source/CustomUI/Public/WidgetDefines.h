// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetDefines.generated.h"


UENUM(BlueprintType)
enum class EInputMode : uint8
{
	GameAndUI,
	GameOnly,
	UIOnly,
};

USTRUCT(BlueprintType)
struct CUSTOMUI_API FPageConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	bool CanCached = false;

	UPROPERTY(EditAnywhere, Category="InputMode")
	EInputMode InputMode = EInputMode::GameAndUI;

	UPROPERTY(EditAnywhere, Category = "InputMode", meta = (EditCondition = "InputMode==EInputMode::UIOnly || InputMode==EInputMode::GameAndUI"))
	bool SetFocus = true;

	UPROPERTY(EditAnywhere, Category = "InputMode", meta = (EditCondition = "InputMode==EInputMode::UIOnly || InputMode==EInputMode::GameAndUI"))
	EMouseLockMode InMouseLockMode = EMouseLockMode::DoNotLock;

	UPROPERTY(EditAnywhere, Category = "InputMode", meta = (EditCondition = "InputMode==EInputMode::GameAndUI"))
	bool HideCursorDuringCapture = true;

	UPROPERTY(EditAnywhere, Category = "InputMode")
	bool FlushInput = false;

	UPROPERTY(EditAnywhere)
	bool ShowMouseCursor = true;

	UPROPERTY(EditAnywhere, meta=(Tooltip="Remain On Level Changed"))
	bool RemainOnLevelChanged = false;

};

UENUM(BlueprintType)
enum class EWidgetZOrder : uint8
{
	Zero = 0,
	Page			UMETA(Hidden, Deprecated),
	Popup			UMETA(Hidden),
};

USTRUCT(BlueprintType)
struct CUSTOMUI_API FPopupConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	bool IgnoreOtherPopup = false;

	UPROPERTY(EditAnywhere)
	bool AllowDuplicate = false;

	UPROPERTY(EditAnywhere)
	bool ShowMouseCursor = true;

	UPROPERTY(EditAnywhere)
	bool RemainOnLevelChanged = false;

};

USTRUCT(BlueprintType)
struct FTextBlockConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FSlateFontInfo Font = FSlateFontInfo();

	UPROPERTY(EditAnywhere)
	FVector2D ShadowOffset = FVector2D();

	UPROPERTY(EditAnywhere)
	FLinearColor ShadowColor = FLinearColor();

	UPROPERTY(EditAnywhere)
	ETextTransformPolicy TransformPolicy = ETextTransformPolicy::None;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ETextJustify::Type> Justification = ETextJustify::Center;
};
