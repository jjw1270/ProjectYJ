// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WidgetDefines.h"
#include "WidgetPlayerController.generated.h"

class UPageBase;

UCLASS(Abstract)
class CUSTOMUI_API AWidgetPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "InputMode")
	EInputMode _InitialInputMode = EInputMode::GameOnly;

	UPROPERTY(EditAnywhere, Category = "InputMode", meta = (EditCondition = "_InitialInputMode==EInputMode::UIOnly || _InitialInputMode==EInputMode::GameAndUI"))
	EMouseLockMode _InMouseLockMode = EMouseLockMode::DoNotLock;

	UPROPERTY(EditAnywhere, Category = "InputMode", meta = (EditCondition = "_InitialInputMode==EInputMode::GameAndUI"))
	bool _HideCursorDuringCapture = false;

	UPROPERTY(EditAnywhere, Category = "InputMode", meta = (EditCondition = "_InitialInputMode==EInputMode::GameOnly"))
	bool _ConsumeCaptureMouseDown = false;

	UPROPERTY(EditAnywhere)
	bool _ShowMouseCursor = false;

protected:
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(EditAnywhere, meta = (DeprecatedProperty, DeprecationMessage = "Page는 이제 안쓰는 개념입니다."))
	TSubclassOf<UPageBase> _InitialPageClass = nullptr;

public:
	TSubclassOf<UPageBase> GetInitialPageClass() const { return _InitialPageClass; }
};
