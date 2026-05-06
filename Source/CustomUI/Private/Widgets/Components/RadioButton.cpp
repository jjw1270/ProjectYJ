// Copyright (c) 2026 장윤제. All rights reserved.


#include "Components/RadioButton.h"
#include "CommonUtils.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"


void URadioButton::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsSelected(_IsSelected, true);
}

void URadioButton::NativeOnMouseEnter(const FGeometry& _geo, const FPointerEvent& _mouse_event)
{
	Super::NativeOnMouseEnter(_geo, _mouse_event);

	if (_ButtonState == EButtonState::Disabled)
	{
		SetButtonState(EButtonState::Disabled);
	}
	else if (_IsSelected == false && _ButtonState != EButtonState::Hovered)
	{
		SetButtonState(EButtonState::Hovered);
		PlaySound(_HoverSound);
	}
}

void URadioButton::NativeOnMouseLeave(const FPointerEvent& _mouse_event)
{
	Super::NativeOnMouseLeave(_mouse_event);

	ResetButtonState();
}

FReply URadioButton::NativeOnMouseButtonDown(const FGeometry& _geo, const FPointerEvent& _mouse_event)
{
	Super::NativeOnMouseButtonDown(_geo, _mouse_event);

	if (_ButtonState == EButtonState::Disabled)
	{
		SetButtonState(EButtonState::Disabled);
	}
	else if (_IsSelected == false && _ButtonState == EButtonState::Hovered)
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

FReply URadioButton::NativeOnMouseButtonUp(const FGeometry& _geo, const FPointerEvent& _mouse_event)
{
	Super::NativeOnMouseButtonUp(_geo, _mouse_event);

	if (_ButtonState == EButtonState::Disabled)
	{
		SetButtonState(EButtonState::Disabled);
	}
	else if (_IsSelected == false && _ButtonState == EButtonState::Pressed)
	{
		if (ClickKeyList.Contains(_mouse_event.GetEffectingButton()))
		{
			if (_OnClicked.IsBound())
				_OnClicked.Broadcast(this);

			SetIsSelected(true);

			SetButtonState(EButtonState::Normal);
		}
	}

	return GetReply();
}

void URadioButton::SetIsSelected(bool _is_selected, bool _force_update)
{
	if (!_force_update && _IsSelected == _is_selected)
		return;
	_IsSelected = _is_selected;

	OnSelectChanged();
	UpdateButtonStyle();

	if (_OnSelectChanged.IsBound())
		_OnSelectChanged.Broadcast(this, _IsSelected);
}

void URadioButton::UpdateButtonStyle()
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
