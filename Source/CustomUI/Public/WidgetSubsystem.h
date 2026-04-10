// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "WidgetPlayerController.h"
#include "WidgetSubsystem.generated.h"

class UWidgetBase;
class UPageBase;
class UPopupBase;

UCLASS()
class CUSTOMUI_API UWidgetSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

friend class UWidgetHelper;

public:
	virtual void Initialize(FSubsystemCollectionBase& _collection) override;
	virtual void Deinitialize() override;
	virtual void PlayerControllerChanged(APlayerController* _new_pc) override;

private:
	AWidgetPlayerController* GetLocalPlayerController() const;

protected:
	void ClearAllWidgets(bool _clear_close_events);
	void RebuildWidgets(AWidgetPlayerController* _pc);

#pragma region Button
	void InitButtonSettings();
#pragma endregion Button
///////////////////////////////////////////////////////////////////
#pragma region WidgetRegistry
// 전역 레벨에서 공용으로 사용하는 위젯을 관리하는 용도
protected:
	UPROPERTY()
	TObjectPtr<class UWidgetRegistryDataAsset> _WidgetRegistryDataAsset = nullptr;

	UPROPERTY()
	TMap<FName, TObjectPtr<UWidgetBase>> _RegisteredWidgetMap;  // map jump에도 사라지지 않음

public:
	UWidgetBase* GetRegisteredWidget(FName _widget_name);

protected:
	void InitRegistryWidgets();

#pragma endregion WidgetRegistry
///////////////////////////////////////////////////////////////////
#pragma region Page
// viewport에 오직 하나의 Page만 존재할 수 있음
protected:
	UPROPERTY()
	TSubclassOf<UPageBase> _RemainingPageClass = nullptr;

	UPROPERTY()
	TObjectPtr<UPageBase> _CurrentPage = nullptr;

	UPROPERTY()
	TSet<TObjectPtr<UPageBase>> _CachedPageList; // 재사용을 위함. map jump시 초기화

protected:
	UPageBase* OpenPage(TSubclassOf<UPageBase> _page_class);
	UPageBase* FindOrCreatePage(TSubclassOf<UPageBase> _page_class);

// getter
protected:
	UPageBase* GetCurrentPage() const { return _CurrentPage; }

#pragma endregion Page
///////////////////////////////////////////////////////////////////
#pragma region Popup
protected:
	bool _DefaultShowMouseCursor = false;

	UPROPERTY()
	TArray<TSubclassOf<UPopupBase>> _RemainingPopupClasses;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UPopupBase>> _CurrentPopups;

protected:
	UPopupBase* OpenPopup(TSubclassOf<UPopupBase> _popup_class);

	UFUNCTION() void OnPopupClose(UWidgetBase* _widget);

protected:
	UPopupBase* GetTopPopup() const;

	bool CheckCanOpenPopup(TSubclassOf<UPopupBase> _popup_class) const;
	bool IsPopupOpened(TSubclassOf<UPopupBase> _popup_class) const;

#pragma endregion Popup

private:
	class UGameInstance* GetGameInstance() const;
};
