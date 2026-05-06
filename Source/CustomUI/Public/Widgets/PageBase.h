// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "WidgetDefines.h"
#include "PageBase.generated.h"

/**
 *
 */
UCLASS(Abstract, meta = (Deprecated, DeprecationMessage = "Page는 이제 안쓰는 개념입니다."))
class CUSTOMUI_API UPageBase : public UWidgetBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
	FPageConfig _PageConfig;

protected:
	virtual void SynchronizeProperties() override;

public:
	UFUNCTION(BlueprintPure)
	const FPageConfig& GetConfig() const { return _PageConfig; }

};
