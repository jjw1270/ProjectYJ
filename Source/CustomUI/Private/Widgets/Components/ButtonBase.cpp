// Copyright (c) 2026 장윤제. All rights reserved.


#include "Components/ButtonBase.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/NamedSlot.h"


TSet<FKey> UButtonBase::ClickKeyList =
{
	EKeys::LeftMouseButton
};

void UButtonBase::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	if (GetIsEnabled() == false)
	{
		TRACE_WARNING(TEXT("EButtonState::Disabled 를 사용해야 합니다!"));
	}

	ResetButtonState();
}

void UButtonBase::SetIsEnabled(bool _is_enabled)
{
	Super::SetIsEnabled(_is_enabled);

	if(_is_enabled)
	{
		TRACE_WARNING(TEXT("EButtonState::Disabled 를 사용해야 합니다!"));
	}
}

void UButtonBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	UpdateButtonSize();
	UpdateButtonStyle();
	UpdateContentSlot();
}

void UButtonBase::SetButtonDisabled(bool _is_disabled)
{
	if (_is_disabled)
	{
		SetButtonState(EButtonState::Disabled);
	}
	else
	{
		if (IsHovered())
			SetButtonState(EButtonState::Hovered);
		else
			SetButtonState(EButtonState::Normal);
	}
}

void UButtonBase::ResetButtonState()
{
	if (_ButtonState == EButtonState::Disabled)
	{
		SetButtonState(EButtonState::Disabled);
	}
	else
	{
		if (IsHovered())
			SetButtonState(EButtonState::Hovered);
		else
			SetButtonState(EButtonState::Normal);
	}
}

void UButtonBase::SetButtonState(EButtonState _state)
{
	if(_ButtonState == _state)
		return;
	_ButtonState = _state;

	UpdateButtonStyle();
	OnButtonStateChanged();
}

void UButtonBase::OnButtonStateChanged_Implementation()
{
	if (_OnButtonStateChanged.IsBound())
	{
		_OnButtonStateChanged.Broadcast(this, _ButtonState);
	}
}

void UButtonBase::UpdateButtonSize()
{
	if (IsValid(SizeBox))
	{
		SizeBox->SetVisibility(ESlateVisibility::Visible);

		auto sizebox_slot = Cast<UCanvasPanelSlot>(SizeBox->Slot);
		if (IsValid(sizebox_slot))
		{
			FAnchors anchors(0.0f, 0.0f, 1.0f, 1.0f);

			if (_UseFixedWidth && _UseFixedHeight)
			{
				anchors = FAnchors(0.5f);
			}
			else if (_UseFixedWidth)
			{
				anchors = FAnchors(0.5f, 0.0f, 0.5f, 1.0f);
			}
			else if (_UseFixedHeight)
			{
				anchors = FAnchors(0.0f, 0.5f, 1.0f, 0.5f);
			}

			sizebox_slot->SetAnchors(anchors);

			sizebox_slot->SetPosition(FVector2D(0.0f));
			sizebox_slot->SetAlignment(FVector2D(0.5f));
			sizebox_slot->SetSize(FVector2D(0.0f));
			sizebox_slot->SetAutoSize(true);
		}

		if (_UseFixedWidth)
		{
			SizeBox->SetWidthOverride(_FixedSize.X);
		}
		else
		{
			SizeBox->ClearWidthOverride();
		}

		if (_UseFixedHeight)
		{
			SizeBox->SetHeightOverride(_FixedSize.Y);
		}
		else
		{
			SizeBox->ClearHeightOverride();
		}
	}
}

void UButtonBase::UpdateButtonStyle()
{
	auto style_ptr = _StateStyles.Find(_ButtonState);
	if (IsValid(style_ptr))
	{
		if (IsValid(Border))
		{
			Border->SetBrush(style_ptr->Brush);
			Border->SetContentColorAndOpacity(style_ptr->ContentColor);
		}
	}
}

void UButtonBase::UpdateContentSlot()
{
	if (IsValid(NS_Content))
	{
		auto slot = Cast<UBorderSlot>(NS_Content->Slot);
		if (IsValid(slot))
		{
			slot->SetHorizontalAlignment(_ContentHorizontalAlignment);
			slot->SetVerticalAlignment(_ContentVerticalAlignment);

			slot->SetPadding(_ContentPadding);
		}
	}
}

void UButtonBase::SetClickKeyList(const TSet<FKey>& _key_list)
{
	ClickKeyList = _key_list;
}
