// Copyright (c) 2026 장윤제. All rights reserved.


#include "Components/ToggleButton.h"
#include "CommonUtils.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"


void UToggleButton::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsSelected(_IsSelected, true);
}

void UToggleButton::NativeOnMouseEnter(const FGeometry& _geo, const FPointerEvent& _mouse_event)
{
	Super::NativeOnMouseEnter(_geo, _mouse_event);

	if (_ButtonState == EButtonState::Disabled)
	{
		SetButtonState(EButtonState::Disabled);
	}
	else if (_ButtonState != EButtonState::Hovered)
	{
		SetButtonState(EButtonState::Hovered);
		PlaySound(_HoverSound);
	}
}

void UToggleButton::NativeOnMouseLeave(const FPointerEvent& _mouse_event)
{
	Super::NativeOnMouseLeave(_mouse_event);

	ResetButtonState();
}

FReply UToggleButton::NativeOnMouseButtonDown(const FGeometry& _geo, const FPointerEvent& _mouse_event)
{
	Super::NativeOnMouseButtonDown(_geo, _mouse_event);

	if (_ButtonState == EButtonState::Disabled)
	{
		SetButtonState(EButtonState::Disabled);
	}
	else if (_ButtonState == EButtonState::Hovered)
	{
		if (ClickKeyList.Contains(_mouse_event.GetEffectingButton()))
		{
			if (_ButtonState != EButtonState::Pressed)
			{
				SetButtonState(EButtonState::Pressed);
				PlaySound(_ClickSound);
			}
		}
	}

	return GetReply();
}

FReply UToggleButton::NativeOnMouseButtonUp(const FGeometry& _geo, const FPointerEvent& _mouse_event)
{
	Super::NativeOnMouseButtonUp(_geo, _mouse_event);

	if (_ButtonState == EButtonState::Disabled)
	{
		SetButtonState(EButtonState::Disabled);
	}
	else if (_ButtonState == EButtonState::Pressed)
	{
		if (ClickKeyList.Contains(_mouse_event.GetEffectingButton()))
		{
			if (_OnClicked.IsBound())
				_OnClicked.Broadcast(this);

			ToggleSelected();

			ResetButtonState();
		}
	}

	return GetReply();
}

void UToggleButton::SetIsSelected(bool _is_selected, bool _force_update)
{
	if (!_force_update && _IsSelected == _is_selected)
		return;
	_IsSelected = _is_selected;

	OnSelectChanged();
	UpdateButtonStyle();

	if (_OnSelectChanged.IsBound())
		_OnSelectChanged.Broadcast(this, _IsSelected);
}

bool UToggleButton::ToggleSelected()
{
	SetIsSelected(!_IsSelected);
	return _IsSelected;
}

void UToggleButton::UpdateButtonStyle()
{
	if (_IsSelected)
	{
		auto style_ptr = _SelectedStateStyles.Find(_ButtonState);
		if (IsValid(style_ptr))
		{
			if (IsValid(Border))
			{
				Border->SetBrush(style_ptr->Brush);
				Border->SetContentColorAndOpacity(style_ptr->ContentColor);
			}
		}
	}
	else
	{
		Super::UpdateButtonStyle();
	}
}
