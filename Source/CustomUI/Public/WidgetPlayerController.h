// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WidgetPlayerController.generated.h"

class UPageBase;

UCLASS(Abstract)
class CUSTOMUI_API AWidgetPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, meta = (DeprecatedProperty, DeprecationMessage = "Page는 이제 안쓰는 개념입니다."))
	TSubclassOf<UPageBase> _InitialPageClass = nullptr;

public:
	TSubclassOf<UPageBase> GetInitialPageClass() const { return _InitialPageClass; }
};
