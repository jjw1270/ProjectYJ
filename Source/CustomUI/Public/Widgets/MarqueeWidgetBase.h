// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/WidgetBase.h"
#include "MarqueeWidgetBase.generated.h"


UENUM(BlueprintType)
enum class EMarqueeType : uint8
{
	NA = 0,
	Scroll				UMETA(Tooltip = "사라질 때까지 흘러갑니다"),
	AlternateScroll		UMETA(Tooltip = "영역 안에서 왔다갔다 흐릅니다"),
};

UCLASS(Abstract)
class CUSTOMUI_API UMarqueeWidgetBase : public UWidgetBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> CanvasPanel = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UNamedSlot> NS_Content = nullptr;
	
protected:
	UPROPERTY(EditAnywhere, Category = "MarqueeTextBlock")
	EMarqueeType _MarqueeType = EMarqueeType::NA;

	UPROPERTY(EditAnywhere, Category = "MarqueeTextBlock")
	TEnumAsByte<EOrientation> _Orientation = EOrientation::Orient_Horizontal;

	UPROPERTY(EditAnywhere, Category = "MarqueeTextBlock")
	bool _IsReverse = false;

	UPROPERTY(EditAnywhere, Category = "MarqueeTextBlock")
	float _MarqueeSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = "MarqueeTextBlock")
	float _DelayTime = 3.0f;
	float _DelayTimer = 0.0f;

protected:
	bool _CanMarqueeing = false;
	bool _IsMarqueeing = false;

	FVector2D _StartPos = FVector2D();
	FVector2D _EndPos = FVector2D();
	FVector2D _CurrentPos = FVector2D();

	FVector2D _PrevWidgetSize = FVector2D();
	FVector2D _PrevContentSize = FVector2D();

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual void SynchronizeProperties() override;
	virtual void NativeTick(const FGeometry& _geo, float _delta) override;

protected:
	void UpdateTargetPos(const FGeometry& _geo);

	void DriveScroll(float _delta);
	void DriveAlternateScroll(float _delta);
};
