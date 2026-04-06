// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetSubsystem.h"
#include "CommonUtils.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "CustomUIDeveloperSettings.h"
#include "WidgetRegistryDataAsset.h"
#include "Widgets/PageBase.h"
#include "Widgets/PopupBase.h"

void UWidgetSubsystem::Initialize(FSubsystemCollectionBase& _collection)
{
	InitRegistryWidgets();
}

void UWidgetSubsystem::Deinitialize()
{
	ClearAllWidgets(true);
}

AWidgetPlayerController* UWidgetSubsystem::GetLocalPlayerController() const
{
	auto world = GetWorld();
	auto local_player = GetLocalPlayer();
	if (IsAllValid(world, local_player))
	{
		return Cast<AWidgetPlayerController>(local_player->GetPlayerController(world));
	}

	return nullptr;
}

void UWidgetSubsystem::PlayerControllerChanged(APlayerController* _new_pc)
{
	_DefaultShowMouseCursor = _new_pc->ShouldShowMouseCursor();

	ClearAllWidgets(true);
	RebuildWidgets(Cast<AWidgetPlayerController>(_new_pc));
}

void UWidgetSubsystem::ClearAllWidgets(bool _clear_close_event)
{
#if FEATURE_PAGE
	if (IsValid(_CurrentPage))
	{
		if (_clear_close_event)
			_CurrentPage->_OnCloseEvent.Clear();

		_CurrentPage->Close(true);
		_CurrentPage = nullptr;
	}

	_CachedPageList.Empty();
#endif

	for (UPopupBase* popup : _CurrentPopups)
	{
		if (IsValid(popup))
		{
			if (_clear_close_event)
				popup->_OnCloseEvent.Clear();

			popup->Close(true);
		}
	}
	_CurrentPopups.Empty();
}

void UWidgetSubsystem::RebuildWidgets(AWidgetPlayerController* _pc)
{
	if (IsInvalid(_pc))
		return;

#if FEATURE_PAGE
	// page
	if (IsValid(_RemainingPageClass))
	{
		OpenPage(_RemainingPageClass);
	}
	else
	{
		auto initial_page_class = _pc->GetInitialPageClass();
		if (IsValid(initial_page_class))
		{
			OpenPage(initial_page_class);
		}
	}
#endif

	// popups
	for (const auto& remaining_popup_class : _RemainingPopupClasses)
	{
		OpenPopup(remaining_popup_class);
	}
}

void UWidgetSubsystem::InitRegistryWidgets()
{
	auto dev_settings = GetDefault<UCustomUIDeveloperSettings>();
	if (IsInvalid(dev_settings))
		return;

	_WidgetRegistryDataAsset = dev_settings->_WidgetRegistryDataAsset.LoadSynchronous();
	if (IsInvalid(_WidgetRegistryDataAsset))
	{
		TRACE_ERROR(TEXT("CustomUIDeveloperSettings 에서 _WidgetRegistryDataAsset 를 설정하세요."));
		return;
	}

	auto game_inst = GetGameInstance();
	if (IsInvalid(game_inst))
		return;

	for (const auto& widget_class_pair : _WidgetRegistryDataAsset->GetWidgetClassMap())
	{
		const auto& widget_name = widget_class_pair.Key;
		const auto& widget_class = widget_class_pair.Value;

		if (widget_name.IsNone() == false && IsValid(widget_class))
		{
			auto widget = CreateWidget<UWidgetBase>(game_inst, widget_class);
			if (IsValid(widget))
			{
				_RegisteredWidgetMap.Add(widget_name, widget);
			}
		}
	}
}

UWidgetBase* UWidgetSubsystem::GetRegisteredWidget(FName _widget_name)
{
	auto widget_ptr = _RegisteredWidgetMap.Find(_widget_name);
	if (IsInvalid(widget_ptr))
	{
		TRACE_ERROR(TEXT("등록된 위젯이 없습니다 : %s"), *_widget_name.ToString());
		return nullptr;
	}

	return *widget_ptr;
}

UPageBase* UWidgetSubsystem::OpenPage(TSubclassOf<UPageBase> _page_class)
{
#if !FEATURE_PAGE
	return nullptr;

#else
	auto pc = GetLocalPlayerController();
	if (IsInvalid(pc))
		return nullptr;

	if (IsValid(_CurrentPage) && _CurrentPage->IsInViewport())
	{
		if (_CurrentPage->GetClass() == _page_class.Get())
		{
			// 이미 열려있는 경우
			return _CurrentPage;
		}
		else
		{
			// 이전 page 닫아줌
			_CurrentPage->Close(true);
			_CurrentPage = nullptr;
		}
	}

	if (IsInvalid(_page_class))
	{
		TRACE_ERROR(TEXT("_page_class Invalid!!"));
		return nullptr;
	}

	_CurrentPage = FindOrCreatePage(_page_class);
	if (IsInvalid(_CurrentPage))
		return nullptr;

	_CurrentPage->AddToViewport((int32)EWidgetZOrder::Page);

	// set is remain on level changed
	if (_CurrentPage->GetConfig().RemainOnLevelChanged)
	{
		_RemainingPageClass = _page_class;
	}
	else
	{
		_RemainingPageClass = nullptr;
	}

	// set input mode
	switch (_CurrentPage->GetConfig().InputMode)
	{
	case EInputMode::GameAndUI:
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(pc,
			_CurrentPage->GetConfig().SetFocus ? _CurrentPage : nullptr,
			_CurrentPage->GetConfig().InMouseLockMode,
			_CurrentPage->GetConfig().HideCursorDuringCapture,
			_CurrentPage->GetConfig().FlushInput);
		break;
	case EInputMode::GameOnly:
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc,
			_CurrentPage->GetConfig().FlushInput);
		break;
	case EInputMode::UIOnly:
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(pc,
			_CurrentPage->GetConfig().SetFocus ? _CurrentPage : nullptr,
			_CurrentPage->GetConfig().InMouseLockMode,
			_CurrentPage->GetConfig().FlushInput);
		break;
	}

	_CurrentPage->SetCursor(_CurrentPage->GetConfig().ShowMouseCursor ? EMouseCursor::Default : EMouseCursor::None);
	pc->SetShowMouseCursor(_CurrentPage->GetConfig().ShowMouseCursor);

	return _CurrentPage;
#endif
}

UPageBase* UWidgetSubsystem::FindOrCreatePage(TSubclassOf<UPageBase> _page_class)
{
#if !FEATURE_PAGE
	return nullptr;

#else
	if (IsInvalid(_page_class))
		return nullptr;

	bool page_can_cached = false;

	auto page_cdo = _page_class.GetDefaultObject();
	if (IsValid(page_cdo))
	{
		page_can_cached = page_cdo->GetConfig().CanCached;
	}

	if (page_can_cached)
	{
		// 재사용 체크
		for (auto cached_page : _CachedPageList)
		{
			if (IsValid(cached_page))
			{
				if (cached_page->IsA(_page_class))
				{
					return cached_page;
				}
			}
		}
	}

	auto pc = GetLocalPlayerController();
	if (IsValid(pc))
	{
		auto new_page = CreateWidget<UPageBase>(pc, _page_class);
		if (IsValid(new_page))
		{
			if(page_can_cached)
			{
				_CachedPageList.Add(new_page);
			}

			return new_page;
		}
	}

	return nullptr;
#endif
}

UPopupBase* UWidgetSubsystem::OpenPopup(TSubclassOf<UPopupBase> _popup_class)
{
	auto pc = GetLocalPlayerController();
	if (IsInvalid(pc))
		return nullptr;

	if (CheckCanOpenPopup(_popup_class) == false)
		return nullptr;

	auto popup = CreateWidget<UPopupBase>(pc, _popup_class);
	if (IsValid(popup))
	{
		popup->_OnCloseEvent.AddDynamic(this, &UWidgetSubsystem::OnPopupClosed);

		popup->AddToViewport((int32)EWidgetZOrder::Popup);

		// set is remain on level changed
		if (popup->GetConfig().RemainOnLevelChanged)
		{
			_RemainingPopupClasses.AddUnique(_popup_class);
		}

		pc->SetShowMouseCursor(popup->GetConfig().ShowMouseCursor);

		_CurrentPopups.Add(popup);
	}

	return popup;
}

void UWidgetSubsystem::OnPopupClosed(UWidgetBase* _widget, bool _is_removed)
{
	auto popup = Cast<UPopupBase>(_widget);
	if (IsInvalid(popup))
		return;

	auto pc = GetLocalPlayerController();
	if (IsInvalid(pc))
		return;

	if (popup->GetConfig().RemainOnLevelChanged)
	{
		_RemainingPopupClasses.Remove(popup->GetClass());
	}

	if(_CurrentPopups.Contains(popup))
	{
		_CurrentPopups.Remove(popup);
	}

	// update mouse cursor
	auto top_popup = GetTopPopup();
	if (IsValid(top_popup))
	{
		pc->SetShowMouseCursor(top_popup->GetConfig().ShowMouseCursor);
	}
	else
	{
		pc->SetShowMouseCursor(_DefaultShowMouseCursor);
	}
}

UPopupBase* UWidgetSubsystem::GetTopPopup() const
{
	if (_CurrentPopups.IsEmpty() == false)
	{
		return _CurrentPopups.Last();
	}

	return nullptr;
}

bool UWidgetSubsystem::CheckCanOpenPopup(TSubclassOf<UPopupBase> _popup_class) const
{
	if (IsInvalid(_popup_class))
	{
		TRACE_ERROR(TEXT("_popup_class Invalid!!"));
		return false;
	}

	const auto default_class = _popup_class.GetDefaultObject();
	if (IsInvalid(default_class))
		return false;

	if (default_class->GetConfig().AllowDuplicate == false)
	{
		if (IsPopupOpened(_popup_class))
		{
			TRACE_WARNING(TEXT("이미 열려있습니다 : %s"), *_popup_class->GetName());
			return false;
		}
	}

	const auto top_popup = GetTopPopup();
	if (IsValid(top_popup))
	{
		if (top_popup->GetConfig().IgnoreOtherPopup)
		{
			TRACE_WARNING(TEXT("Popop Ignored : %s"), *_popup_class->GetName());
			return false;
		}
	}

	return true;
}

bool UWidgetSubsystem::IsPopupOpened(TSubclassOf<UPopupBase> _popup_class) const
{
	for (const auto popup : _CurrentPopups)
	{
		if (IsValid(popup))
		{
			if (popup->GetClass() == _popup_class.Get())
				return true;
		}
	}

	return false;
}

class UGameInstance* UWidgetSubsystem::GetGameInstance() const
{
	auto world = GetWorld();
	if (IsValid(world))
	{
		return world->GetGameInstance();
	}

	return nullptr;
}
