// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WidgetRegistryDataAsset.generated.h"

class UWidgetBase;

/**
 * 
 */
UCLASS(BlueprintType)
class CUSTOMUI_API UWidgetRegistryDataAsset : public UDataAsset
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere)
    TMap<FName, TSubclassOf<UWidgetBase>> _WidgetClassMap;

protected:
#if WITH_EDITOR
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& _property_changed_event) override;
#endif

public:
    const  TMap<FName, TSubclassOf<UWidgetBase>>& GetWidgetClassMap() const { return _WidgetClassMap; }
};
