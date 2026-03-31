// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/WidgetBase.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetTree.h"
#include "MovieScene.h"

void UWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	OnNativeVisibilityChanged.RemoveAll(this);
	OnNativeVisibilityChanged.AddUObject(this, &UWidgetBase::OnVisibilityChanged);
}

void UWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (_IsShowOnNextTick)
	{
		SetRenderOpacity(0.0f);
	}

	if(IsVisible() && _WidgetState == EWidgetState::Hide)
	{
		SetWidgetState(EWidgetState::Showing);
	}
}

void UWidgetBase::NativeDestruct()
{
	_WidgetState = EWidgetState::Hide;

	Super::NativeDestruct();
}

void UWidgetBase::OnAnimationFinished_Implementation(const UWidgetAnimation* _anim)
{
	Super::OnAnimationFinished_Implementation(_anim);

	if (IsValid(_anim) && _CurrentAnim == _anim)
	{
		switch (_WidgetState)
		{
		case EWidgetState::Showing:
			if (_CurrentAnim == ShowAnim)
			{
				SetWidgetState(EWidgetState::Idle);
			}
			break;
		case EWidgetState::Hiding:
			if (_CurrentAnim == HideAnim)
			{
				SetWidgetState(EWidgetState::Hide);
			}
			break;
		default:
			break;
		}
	}
}

void UWidgetBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	OnSynchronizeProperties();
}

void UWidgetBase::SetVisibility(ESlateVisibility _visibility)
{
	Super::SetVisibility(_visibility);
	TRACE_WARNING(TEXT("주의! Construct된 위젯의 경우 Show/Hide를 사용해야 합니다.\n그 이전 시점인 경우 사용해주세요."));
}

void UWidgetBase::OnVisibilityChanged(ESlateVisibility _visibility)
{
	switch (_visibility)
	{
	case ESlateVisibility::Collapsed:
	case ESlateVisibility::Hidden:
		_CurrentAnim = nullptr;
		_WidgetState = EWidgetState::Hide;
		break;
	case ESlateVisibility::Visible:
	case ESlateVisibility::HitTestInvisible:
	case ESlateVisibility::SelfHitTestInvisible:
		if (_IsShowOnNextTick)
		{
			SetRenderOpacity(0.0f);
		}

		if (_WidgetState == EWidgetState::Hide)
		{
			SetWidgetState(EWidgetState::Showing);
		}
		break;
	default:
		break;
	}
}

void UWidgetBase::SetWidgetState(EWidgetState _new_state)
{
	if (_WidgetState == _new_state)
		return;
	_WidgetState = _new_state;

	// broadcast events
	switch (_WidgetState)
	{
	case EWidgetState::Hide:
		if (_OnCloseEvent.IsBound())
			_OnCloseEvent.Broadcast(this, (_WidgetHideType == EWidgetHideType::RemoveFromParent));
		OnClose();
		break;
	case EWidgetState::Showing:
		if (_OnShowEvent.IsBound())
			_OnShowEvent.Broadcast(this);
		OnShow();
		break;
	case EWidgetState::Idle:
		if (_OnIdleEvent.IsBound())
			_OnIdleEvent.Broadcast(this);
		OnIdle();
		break;
	default:
		break;
	}

	// Event를 통해 WidgetState를 건드렸을 경우 예외처리
	if (_WidgetState != _new_state)
		return;

	if (_WidgetState == EWidgetState::Hide)
	{
		HideWidget();
		return;
	}

	UWidgetAnimation* anim_to_play = nullptr;
	FWidgetAnimConfig* anim_config = nullptr;

	bool is_idle = false;

	switch (_WidgetState)
	{
	case EWidgetState::Showing:
		if (_IsShowOnNextTick)
		{
			SetRenderOpacity(1.0f);
		}
		anim_to_play = ShowAnim;
		anim_config = &_ShowAnimConfig;
		PlaySound(_ShowSound);
		break;
	case EWidgetState::Idle:
		anim_to_play = IdleAnim;
		anim_config = &_IdleAnimConfig;
		is_idle = true;
		break;
	case EWidgetState::Hiding:
		anim_to_play = HideAnim;
		anim_config = &_HideAnimConfig;
		PlaySound(_HideSound);
		break;
	default:
		break;
	}

	StopAnimation(_CurrentAnim); // Event는 호출되지 않는다.

	if (IsAllValid(anim_to_play, anim_config))
	{
		_CurrentAnim = anim_to_play;
		PlayAnimation(_CurrentAnim, 0.0f, is_idle ? 0 : 1, anim_config->PlayType, 1.0f, anim_config->IsRestoreState);
	}
	else
	{
		if (_WidgetState == EWidgetState::Showing)
		{
			SetWidgetState(EWidgetState::Idle);
		}
		else if (_WidgetState == EWidgetState::Hiding)
		{
			SetWidgetState(EWidgetState::Hide);
		}
	}
}

void UWidgetBase::Show(EWidgetShowType _show_type, bool _is_skip_anim)
{
	switch (_show_type)
	{
	case EWidgetShowType::Visible:
		Super::SetVisibility(ESlateVisibility::Visible);
		break;
	case EWidgetShowType::HitTestInvisible:
		Super::SetVisibility(ESlateVisibility::HitTestInvisible);
		break;
	case EWidgetShowType::SelfHitTestInvisible:
		Super::SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	default:
		break;
	}

	if (_is_skip_anim)
	{
		SetWidgetState(EWidgetState::Idle);
	}
}

void UWidgetBase::Hide(EWidgetHideType _hide_type, bool _is_skip_anim)
{
	if (_hide_type == EWidgetHideType::NA)
	{
		TRACE_ERROR(TEXT("_hide_type이 NA 일 수 없습니다."));
		return;
	}

	if (_WidgetHideType == _hide_type)
		return;
	_WidgetHideType = _hide_type;

	if (_is_skip_anim)
	{
		SetWidgetState(EWidgetState::Hide);
	}
	else
	{
		SetWidgetState(EWidgetState::Hiding);
	}
}

void UWidgetBase::Close(bool _is_skip_anim)
{
	Hide(EWidgetHideType::RemoveFromParent, _is_skip_anim);
}

void UWidgetBase::HideWidget()
{
	switch (_WidgetHideType)
	{
	case EWidgetHideType::NA:
		TRACE_ERROR(TEXT("_HideType이 NA 일 수 없습니다."));
		break;
	case EWidgetHideType::RemoveFromParent:
		RemoveFromParent();
		break;
	case EWidgetHideType::Collapsed:
		Super::SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EWidgetHideType::Hidden:
		Super::SetVisibility(ESlateVisibility::Hidden);
		break;
	}

	_WidgetHideType = EWidgetHideType::NA;
}
