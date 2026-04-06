// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ClickButton.h"


TSet<FKey> UClickButton::SubClickKeyList =
{
	EKeys::RightMouseButton
};

void UClickButton::NativeOnMouseEnter(const FGeometry& _geo, const FPointerEvent& _mouse_event)
{
	Super::NativeOnMouseEnter(_geo, _mouse_event);

	if (_ButtonState == EButtonState::Disabled)
	{
		SetButtonState(EButtonState::Disabled);
	}
	else if (_ButtonState == EButtonState::Normal)
	{
		SetButtonState(EButtonState::Hovered);
		PlaySound(_HoverSound);
	}
}

void UClickButton::NativeOnMouseLeave(const FPointerEvent& _mouse_event)
{
	Super::NativeOnMouseLeave(_mouse_event);

	if (_ButtonState != EButtonState::Pressed)
		ResetButtonState();
}

FReply UClickButton::NativeOnMouseButtonDown(const FGeometry& _geo, const FPointerEvent& _mouse_event)
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
		else if (_UseSubClick && SubClickKeyList.Contains(_mouse_event.GetEffectingButton()))
		{
			if (_ButtonState != EButtonState::Pressed)
			{
				SetButtonState(EButtonState::Pressed);
				PlaySound(_SubClickSound);
			}
		}
	}

	return GetReply();
}

FReply UClickButton::NativeOnMouseButtonUp(const FGeometry& _geo, const FPointerEvent& _mouse_event)
{
	Super::NativeOnMouseButtonUp(_geo, _mouse_event);
	
	if (_ButtonState == EButtonState::Disabled)
	{
		SetButtonState(EButtonState::Disabled);
	} 
	else if(_ButtonState == EButtonState::Pressed)
	{
		if (ClickKeyList.Contains(_mouse_event.GetEffectingButton()))
		{
			if (_OnClicked.IsBound())
				_OnClicked.Broadcast(this);

			ResetButtonState();
		}
		else if (_UseSubClick && SubClickKeyList.Contains(_mouse_event.GetEffectingButton()))
		{
			if (_OnSubClicked.IsBound())
				_OnSubClicked.Broadcast(this);

			ResetButtonState();
		}
	}

	return GetReply();
}

FReply UClickButton::NativeOnMouseButtonDoubleClick(const FGeometry& _geo, const FPointerEvent& _mouse_event)
{
	Super::NativeOnMouseButtonDoubleClick(_geo, _mouse_event);

	if (_ButtonState == EButtonState::Disabled)
	{
		SetButtonState(EButtonState::Disabled);
	} 
	else
	{
		// Button State와 별개로 동작
		if (ClickKeyList.Contains(_mouse_event.GetEffectingButton()))
		{
			PlaySound(_ClickSound);

			if (_OnDoubleClicked.IsBound())
				_OnDoubleClicked.Broadcast(this);
		}
	}

	return GetReply();
}

void UClickButton::SetSubClickKeyList(const TSet<FKey>& _key_list)
{
	SubClickKeyList = _key_list;
}
