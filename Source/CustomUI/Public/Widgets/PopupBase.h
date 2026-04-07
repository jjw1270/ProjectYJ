// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "PopupBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CUSTOMUI_API UPopupBase : public UWidgetBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
	FPopupConfig _PopupConfig;

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintPure)
	const FPopupConfig& GetConfig() const { return _PopupConfig; }
};
