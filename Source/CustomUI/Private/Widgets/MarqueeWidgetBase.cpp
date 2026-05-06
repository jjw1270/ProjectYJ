// Copyright (c) 2026 장윤제. All rights reserved.


#include "Widgets/MarqueeWidgetBase.h"
#include "CommonUtils.h"
#include "WidgetHelper.h"
#include "Components/NamedSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UMarqueeWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMarqueeWidgetBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (UWidgetHelper::IsDesignTime(this) && IsValid(NS_Content))
	{
		auto ns_slot = Cast<UCanvasPanelSlot>(NS_Content->Slot);
		if (IsValid(ns_slot))
		{
			ns_slot->SetPosition(FVector2D::Zero());
		}
	}
}

void UMarqueeWidgetBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (IsValid(CanvasPanel))
	{
		if(CanvasPanel->GetClipping() != EWidgetClipping::ClipToBounds)
			CanvasPanel->SetClipping(EWidgetClipping::ClipToBounds);
	}

	if (IsInvalid(NS_Content))
		return;

	auto ns_slot = Cast<UCanvasPanelSlot>(NS_Content->Slot);
	if (IsValid(ns_slot))
	{
		switch (_Orientation)
		{
		case EOrientation::Orient_Horizontal:
			ns_slot->SetAnchors(FAnchors(0.0f, 0.5f, 0.0f, 0.5f));
			ns_slot->SetAlignment(FVector2D(0.0f, 0.5f));
			break;
		case EOrientation::Orient_Vertical:
			ns_slot->SetAnchors(FAnchors(0.5f, 0.0f, 0.5f, 0.0f));
			ns_slot->SetAlignment(FVector2D(0.5f, 0.0f));
			break;
		}

		ns_slot->SetAutoSize(true);
	}
}

void UMarqueeWidgetBase::NativeTick(const FGeometry& _geo, float _delta)
{
	Super::NativeTick(_geo, _delta);

	UpdateTargetPos(_geo);

	if (_CanMarqueeing)
	{
		_IsMarqueeing = true;

		if (_DelayTimer > 0.0f)
		{
			_DelayTimer -= _delta;
		}
		else
		{
			switch (_MarqueeType)
			{
			case EMarqueeType::Scroll:
				DriveScroll(_delta);
				break;
			case EMarqueeType::AlternateScroll:
				DriveAlternateScroll(_delta);
				break;
			default:
				break;
			}
		}
	}
	else
	{
		_IsMarqueeing = false;
	}
}

void UMarqueeWidgetBase::UpdateTargetPos(const FGeometry& _geo)
{
	if (IsInvalid(NS_Content))
	{
		_CanMarqueeing = false;
		return;
	}

	const auto& widget_size = _geo.GetLocalSize();
	if (widget_size.IsNearlyZero(0.001f))
	{
		_CanMarqueeing = false;
		return;
	}

	const auto& content_size = NS_Content->GetDesiredSize();

	if (!widget_size.Equals(_PrevWidgetSize, 0.001f) || !content_size.Equals(_PrevContentSize, 0.001f))
	{
		_PrevWidgetSize = widget_size;
		_PrevContentSize = content_size;

		switch (_Orientation)
		{
		case EOrientation::Orient_Horizontal:
			switch (_MarqueeType)
			{
			case EMarqueeType::NA:
				_StartPos = FVector2D(0.0f, 0.0f);
				_EndPos = FVector2D(0.0f, 0.0f);

				_CanMarqueeing = false;
				break;
			case EMarqueeType::Scroll:
				_StartPos = FVector2D(-content_size.X, 0.0f);
				_EndPos = FVector2D(widget_size.X, 0.0f);

				_CanMarqueeing = true;
				break;
			case EMarqueeType::AlternateScroll:
				_StartPos = FVector2D(0.0f, 0.0f);
				_EndPos = FVector2D(widget_size.X - content_size.X, 0.0f);

				_CanMarqueeing = content_size.X < widget_size.X;
				break;
			}
			break;
		case EOrientation::Orient_Vertical:
			switch (_MarqueeType)
			{
			case EMarqueeType::NA:
				_StartPos = FVector2D(0.0f, 0.0f);
				_EndPos = FVector2D(0.0f, 0.0f);

				_CanMarqueeing = false;
				break;
			case EMarqueeType::Scroll:
				_StartPos = FVector2D(0.0f, -content_size.Y);
				_EndPos = FVector2D(0.0f, widget_size.Y);

				_CanMarqueeing = true;
				break;
			case EMarqueeType::AlternateScroll:
				_StartPos = FVector2D(0.0f, 0.0f);
				_EndPos = FVector2D(0.0f, widget_size.Y - content_size.Y);

				_CanMarqueeing = content_size.Y < widget_size.Y;
				break;
			}
			break;
		}

		if (_IsMarqueeing == false)
		{
			if (_IsReverse)
				_CurrentPos = _EndPos;
			else
				_CurrentPos = _StartPos;

			auto ns_slot = Cast<UCanvasPanelSlot>(NS_Content->Slot);
			if (IsValid(ns_slot))
			{
				ns_slot->SetPosition(_CurrentPos);
			}
		}
	}
}

void UMarqueeWidgetBase::DriveScroll(float _delta)
{
	if (IsInvalid(NS_Content))
		return;

	auto ns_slot = Cast<UCanvasPanelSlot>(NS_Content->Slot);
	if (IsInvalid(ns_slot))
		return;

	switch (_Orientation)
	{
	case EOrientation::Orient_Horizontal:
		if (_IsReverse)
		{
			_CurrentPos.X -= (_delta * _MarqueeSpeed);

			if (_CurrentPos.X < _StartPos.X)
			{
				_CurrentPos.X = _EndPos.X;
				_DelayTimer = _DelayTime;
			}
		}
		else
		{
			_CurrentPos.X += (_delta * _MarqueeSpeed);

			if (_CurrentPos.X > _EndPos.X)
			{
				_CurrentPos.X = _StartPos.X;
				_DelayTimer = _DelayTime;
			}
		}
		break;
	case EOrientation::Orient_Vertical:
		if (_IsReverse)
		{
			_CurrentPos.Y -= (_delta * _MarqueeSpeed);

			if (_CurrentPos.Y < _StartPos.Y)
			{
				_CurrentPos.Y = _EndPos.Y;
				_DelayTimer = _DelayTime;
			}
		}
		else
		{
			_CurrentPos.Y += (_delta * _MarqueeSpeed);

			if (_CurrentPos.Y > _EndPos.Y)
			{
				_CurrentPos.Y = _StartPos.Y;
				_DelayTimer = _DelayTime;
			}
		}
		break;
	}

	ns_slot->SetPosition(_CurrentPos);
}

void UMarqueeWidgetBase::DriveAlternateScroll(float _delta)
{
	if (IsInvalid(NS_Content))
		return;

	auto ns_slot = Cast<UCanvasPanelSlot>(NS_Content->Slot);
	if (IsInvalid(ns_slot))
		return;

	switch (_Orientation)
	{
	case EOrientation::Orient_Horizontal:
		if (_IsReverse)
		{
			_CurrentPos.X -= (_delta * _MarqueeSpeed);

			if (_CurrentPos.X < _StartPos.X)
			{
				//_CurrentPos.X = _StartPos.X;
				_DelayTimer = _DelayTime;
				_IsReverse = !_IsReverse;
			}
		}
		else
		{
			_CurrentPos.X += (_delta * _MarqueeSpeed);

			if (_CurrentPos.X > _EndPos.X)
			{
				//_CurrentPos.X = _EndPos.X;
				_DelayTimer = _DelayTime;
				_IsReverse = !_IsReverse;
			}
		}
		break;
	case EOrientation::Orient_Vertical:
		if (_IsReverse)
		{
			_CurrentPos.Y -= (_delta * _MarqueeSpeed);

			if (_CurrentPos.Y < _StartPos.Y)
			{
				_CurrentPos.Y = _StartPos.Y;
				_DelayTimer = _DelayTime;
				_IsReverse = !_IsReverse;
			}
		}
		else
		{
			_CurrentPos.Y += (_delta * _MarqueeSpeed);

			if (_CurrentPos.Y > _EndPos.Y)
			{
				_CurrentPos.Y = _EndPos.Y;
				_DelayTimer = _DelayTime;
				_IsReverse = !_IsReverse;
			}
		}
		break;
	}

	ns_slot->SetPosition(_CurrentPos);
}
