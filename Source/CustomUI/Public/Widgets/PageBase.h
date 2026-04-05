// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "PageBase.generated.h"

/**
 *
 */
UCLASS(Abstract, meta = (DeprecatedNode, DeprecationMessage = "Page는 이제 안쓰는 개념입니다."))
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
