// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ClickButton.h"


TSet<FKey> UClickButton::SubClickKeyList =
{
	EKeys::RightMouseButton
};

double UClickButton::_DoubleClickThreshold = 0.2f;

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
	else if (_ButtonState == EButtonState::Pressed)
	{
		if (ClickKeyList.Contains(_mouse_event.GetEffectingButton()))
		{
			if (_OnClicked.IsBound())
				_OnClicked.Broadcast(this);

			// 위젯 컴포넌트에서 위젯 인터렉션으로 입력을 받았을 경우에만 이 함수가 호출된다.
			if (_OnDoubleClicked.IsBound())
			{
				CheckDoubleClick();
			}

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

	// 더블클릭 시 NativeOnMouseButtonDown 이 안 들어오는 경우가 있으므로
	// 눌림 상태만 보정한다.
	if (_ButtonState == EButtonState::Disabled)
	{
		SetButtonState(EButtonState::Disabled);
	}
	else if (ClickKeyList.Contains(_mouse_event.GetEffectingButton()))
	{
		SetButtonState(EButtonState::Pressed);
		PlaySound(_ClickSound);
	}

	return GetReply();
}

void UClickButton::CheckDoubleClick()
{
	const double current_time = FPlatformTime::Seconds();

	const bool is_double_click = (current_time - _LastClickTime) <= _DoubleClickThreshold;

	if (is_double_click)
	{
		_OnDoubleClicked.Broadcast(this);
	}

	_LastClickTime = current_time;
}

void UClickButton::SetSubClickKeyList(const TSet<FKey>& _key_list)
{
	SubClickKeyList = _key_list;
}

void UClickButton::SetDoubleClickThreshold(double _second)
{
	_DoubleClickThreshold = _second;
}
