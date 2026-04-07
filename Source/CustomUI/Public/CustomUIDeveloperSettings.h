// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CustomUIDeveloperSettings.generated.h"


UCLASS(Config = Game, DefaultConfig)
class CUSTOMUI_API UCustomUIDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Config, Category = "StringTable")
	TArray<TSoftObjectPtr<class UStringTable>> _StringTables;

	UPROPERTY(EditAnywhere, Config, Category = "WidgetRegistry")
	TSoftObjectPtr<class UWidgetRegistryDataAsset> _WidgetRegistryDataAsset = nullptr;
};
