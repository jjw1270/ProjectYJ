// Copyright (c) 2026 장윤제. All rights reserved.


#include "WidgetHelper.h"
#include "CommonUtils.h"
#include "WidgetSubsystem.h"
#include "Widgets/PageBase.h"
#include "Widgets/PopupBase.h"
#include "StringTableSubsystem.h"


UWidgetBase* UWidgetHelper::GetRegisteredWidget(const UObject* _world_ctx, FName _widget_name)
{
	auto widget_subsys = UCommonUtils::GetLocalPlayerSubsystem<UWidgetSubsystem>(_world_ctx);
	if (IsValid(widget_subsys))
	{
		return widget_subsys->GetRegisteredWidget(_widget_name);
	}

	return nullptr;
}


UPageBase* UWidgetHelper::OpenPage_Internal(const UObject* _world_ctx, TSubclassOf<UPageBase> _page_class)
{
#if FEATURE_PAGE
	auto widget_subsys = UCommonUtils::GetLocalPlayerSubsystem<UWidgetSubsystem>(_world_ctx);
	if (IsValid(widget_subsys))
	{
		return widget_subsys->OpenPage(_page_class);
	}
#endif

	return nullptr;
}

UPopupBase* UWidgetHelper::OpenPopup_Internal(const UObject* _world_ctx, TSubclassOf<UPopupBase> _popup_class)
{
	auto widget_subsys = UCommonUtils::GetLocalPlayerSubsystem<UWidgetSubsystem>(_world_ctx);
	if (IsValid(widget_subsys))
	{
		return widget_subsys->OpenPopup(_popup_class);
	}

	return nullptr;
}

void UWidgetHelper::ClosePopup(UPopupBase* _popup, bool _is_skip_anim)
{
	if (IsValid(_popup))
	{
		_popup->Close(_is_skip_anim);
	}
}

UPopupBase* UWidgetHelper::GetTopPopup(const UObject* _world_ctx)
{
	auto widget_subsys = UCommonUtils::GetLocalPlayerSubsystem<UWidgetSubsystem>(_world_ctx);
	if (IsValid(widget_subsys))
	{
		return widget_subsys->GetTopPopup();
	}

	return nullptr;
}

bool UWidgetHelper::IsDesignTime(const UUserWidget* _widget)
{
	if (IsValid(_widget))
	{
		return _widget->IsDesignTime();
	}

	return false;
}

FText UWidgetHelper::GetStringTableText(const FString& _table_name, const FString& _key)
{
	return UStringTableSubsystem::FindTextFromTable(_table_name, _key);
}
