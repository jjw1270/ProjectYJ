// Fill out your copyright notice in the Description page of Project Settings.

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
