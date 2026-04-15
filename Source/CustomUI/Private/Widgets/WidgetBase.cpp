// Copyright (c) 2026 장윤제. All rights reserved.


#include "Widgets/WidgetBase.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetTree.h"
#include "MovieScene.h"
#include "Components/WidgetSwitcher.h"

void UWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	OnNativeVisibilityChanged.RemoveAll(this);
	OnNativeVisibilityChanged.AddUObject(this, &UWidgetBase::OnVisibilityChanged);
}

void UWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (_WidgetHideType != EWidgetHideType::NA)
	{
		HideWidget();
		return;
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
	TRACE_WARNING(TEXT("이 함수 대신 UWidgetBase::Show/Hide 함수를 사용해주세요!"));
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
	case EWidgetState::Showing:
		if (_OnShowEvent.IsBound())
			_OnShowEvent.Broadcast(this);
		OnShow();

		if (_IsTraverseWidgetToShow)
			TraverseWidgetToShow();
		break;

	case EWidgetState::Idle:
		if (_OnIdleEvent.IsBound())
			_OnIdleEvent.Broadcast(this);
		OnIdle();
		break;

	case EWidgetState::Hide:
		if (_OnHideEvent.IsBound())
			_OnHideEvent.Broadcast(this, _WidgetHideType);
		OnHide();
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
		if (_IsPreventInitialFlicker && IsValid(ShowAnim))
		{
			SetRenderOpacity(0.0f);

			auto world = GetWorld();
			if (IsValid(world))
			{
				world->GetTimerManager().SetTimerForNextTick(
					FTimerDelegate::CreateWeakLambda(this,
						[this]()
						{
							SetRenderOpacity(1.0f);
						}
					)
				);
			}
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
	else
	{
		SetWidgetState(EWidgetState::Showing);
	}
}

void UWidgetBase::TraverseWidgetToShow()
{
	if (IsInvalid(WidgetTree))
		return;

	TraverseWidgetToShow_Internal(WidgetTree->RootWidget);
}

bool UWidgetBase::CanTraverseWidget(UWidget* _widget) const
{
	if (IsInvalid(_widget))
		return false;

	if (_widget->IsVisible() == false)
		return false;

	const auto parent = _widget->GetParent();
	if (IsValid(parent))
	{
		const auto widget_switcher = Cast<UWidgetSwitcher>(parent);
		if (IsValid(widget_switcher))
		{
			if (widget_switcher->GetActiveWidget() != _widget)
				return false;
		}
	}

	return true;
}

void UWidgetBase::TraverseWidgetToShow_Internal(UWidget* _widget)
{
	if (CanTraverseWidget(_widget) == false)
		return;

	auto widget_base_widget = Cast<UWidgetBase>(_widget);
	if (IsValid(widget_base_widget))
	{
		widget_base_widget->SetWidgetState(EWidgetState::Showing);
	}

	auto user_widget = Cast<UUserWidget>(_widget);
	if (IsValid(user_widget))
	{
		if (user_widget != this && IsAllValid(user_widget->WidgetTree, user_widget->WidgetTree->RootWidget))
		{
			TraverseWidgetToShow_Internal(user_widget->WidgetTree->RootWidget);
			return;
		}
	}

	auto panel = Cast<UPanelWidget>(_widget);
	if (IsValid(panel))
	{
		const int32 child_count = panel->GetChildrenCount();
		for (int32 i = 0; i < child_count; ++i)
		{
			TraverseWidgetToShow_Internal(panel->GetChildAt(i));
		}
	}
}

void UWidgetBase::Hide(EWidgetHideType _hide_type, bool _is_skip_anim)
{
	if (_WidgetState == EWidgetState::Hiding || _WidgetState == EWidgetState::Hide)
		return;

	if (_hide_type == EWidgetHideType::NA)
	{
		TRACE_ERROR(TEXT("_hide_type이 NA 일 수 없습니다."));
		return;
	}

	_WidgetHideType = _hide_type;

	if (_OnStartHideEvent.IsBound())
		_OnStartHideEvent.Broadcast(this, _WidgetHideType);
	OnStartHide();

	if (_WidgetState == EWidgetState::Hide)
		return;

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
