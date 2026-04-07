// Copyright (c) 2026 장윤제. All rights reserved.


#include "StringTableSubsystem.h"
#include "CommonUtils.h"
#include "CustomUIDeveloperSettings.h"
#include "Internationalization/StringTable.h"

TMap<FString, FName> UStringTableSubsystem::_NameIDMap;

void UStringTableSubsystem::Initialize(FSubsystemCollectionBase& _collection)
{
	Super::Initialize(_collection);

	_NameIDMap.Empty();

	auto dev_settings = GetDefault<UCustomUIDeveloperSettings>();
	if (IsInvalid(dev_settings))
		return;

	for (auto string_table_soft_obj : dev_settings->_StringTables)
	{
		auto string_table = string_table_soft_obj.LoadSynchronous();
		if (IsInvalid(string_table))
		{
			TRACE_WARNING(TEXT("String Table 로드 실패 : %s"), *string_table_soft_obj.ToString());
			continue;
		}

		_NameIDMap.Add(string_table->GetName(), string_table->GetStringTableId());
	}
}

FText UStringTableSubsystem::FindTextFromTable(const FString& _table_name, const FString& _key)
{
	FName table_id;

	auto table_id_ptr = _NameIDMap.Find(_table_name);
	if (IsValid(table_id_ptr))
	{
		table_id = *table_id_ptr;
	}

	return FText::FromStringTable(table_id, _key);
}
