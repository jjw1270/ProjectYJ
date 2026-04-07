// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StringTableSubsystem.generated.h"

/*
 * 
 */
UCLASS()
class CUSTOMUI_API UStringTableSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	// <table name, string table id>
	static TMap<FString, FName> _NameIDMap;

public:
	virtual void Initialize(FSubsystemCollectionBase& _collection) override;

public:
	static FText FindTextFromTable(const FString& _table_name, const FString& _key);
};
