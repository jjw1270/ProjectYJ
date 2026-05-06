// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WidgetHelper.generated.h"

class UWidgetBase;
class UPageBase;
class UPopupBase;

template<typename T>
concept CONCEPT_PageBase = TIsDerivedFrom<T, UPageBase>::IsDerived;

template<typename T>
concept  CONCEPT_PopupBase = TIsDerivedFrom<T, UPopupBase>::IsDerived;

UCLASS()
class CUSTOMUI_API UWidgetHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (WorldContext = "_widget"))
	static bool IsDesignTime(const UUserWidget* _widget);

	UFUNCTION(BlueprintPure)
	static FText GetStringTableText(const FString& _table_name, const FString& _key);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "_world_ctx"))
	static UWidgetBase* GetRegisteredWidget(const UObject* _world_ctx, FName _widget_name);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region Page
public:
	template<CONCEPT_PageBase T>
	static T* OpenPage(const UObject* _world_ctx, TSubclassOf<T> _page_class)
	{
		return Cast<T>(OpenPage_Internal(_world_ctx, _page_class));
	}

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "_world_ctx", DeprecatedFunction, DeprecationMessage = "Page는 이제 안쓰는 개념입니다."))
	static  UPageBase* OpenPage(const UObject* _world_ctx, TSubclassOf<UPageBase> _page_class)
	{
		return OpenPage_Internal(_world_ctx, _page_class);
	}

private:
	static UPageBase* OpenPage_Internal(const UObject* _world_ctx, TSubclassOf<UPageBase> _page_class);

#pragma endregion Page
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region Popup
public:
	template<CONCEPT_PopupBase T>
	static T* OpenPopup(const UObject* _world_ctx, TSubclassOf<T> _popup_class)
	{
		return Cast<T>(OpenPopup_Internal(_world_ctx, _popup_class));
	}

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "_world_ctx"))
	static UPopupBase* OpenPopup(const UObject* _world_ctx, TSubclassOf<UPopupBase> _popup_class)
	{
		return OpenPopup_Internal(_world_ctx, _popup_class);
	}

private:
	static UPopupBase* OpenPopup_Internal(const UObject* _world_ctx, TSubclassOf<UPopupBase> _popup_class);

public:
	UFUNCTION(BlueprintCallable)
	static void ClosePopup(UPopupBase* _popup, bool _is_skip_anim);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "_world_ctx"))
	static UPopupBase* GetTopPopup(const UObject* _world_ctx);

#pragma endregion Popup
};

#define GETTEXT(_table_name, _key) UWidgetHelper::GetStringTableText(_table_name, _key);
