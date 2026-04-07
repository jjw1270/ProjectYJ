// Copyright (c) 2026 장윤제. All rights reserved.


#include "Components/RadioButtonGroup.h"
#include "Components/StackBox.h"
#include "Components/StackBoxSlot.h"
#include "Components/CanvasPanelSlot.h"

void URadioButtonGroup::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (IsInvalid(StackBox))
		return;

	StackBox->SetOrientation(_Orientation);
	
	auto cp_slot = Cast<UCanvasPanelSlot>(StackBox->Slot);
	if (IsValid(cp_slot))
	{
		cp_slot->SetAutoSize(true);
	}

	UpdateRadioButtons();
}

void URadioButtonGroup::UpdateRadioButtons()
{
	if (IsInvalid(StackBox))
		return;

	 FSlateChildSize slate_child_size;

	if (_Orientation == EOrientation::Orient_Horizontal)
	{
		slate_child_size.SizeRule = (_HorizontalAlignment == EHorizontalAlignment::HAlign_Fill) ? ESlateSizeRule::Fill : ESlateSizeRule::Automatic;
	}
	else
	{
		slate_child_size.SizeRule = (_VerticalAlignment == EVerticalAlignment::VAlign_Fill) ? ESlateSizeRule::Fill : ESlateSizeRule::Automatic;
	}

	for (auto child : StackBox->GetAllChildren())
	{
		auto radio_button = Cast<URadioButton>(child);
		if (IsValid(radio_button))
		{
			if (radio_button->GetWidgetID().IsNone())
			{
				TRACE_WARNING(TEXT("Radio Button(%s)의 WidgetID가 설정되지 않았습니다!!"), *radio_button->GetName());
			}

			radio_button->_OnClicked.RemoveAll(this);
			radio_button->_OnClicked.AddDynamic(this, &URadioButtonGroup::OnClickRadioButton);

			auto slot = Cast<UStackBoxSlot>(child->Slot);
			if (IsValid(slot))
			{
				slot->SetPadding(_ButtonPadding);

				slot->SetSize(slate_child_size);

				slot->SetHorizontalAlignment(_HorizontalAlignment);
				slot->SetVerticalAlignment(_VerticalAlignment);
			}
		}
		else
		{
			TRACE_ERROR(TEXT("GP_ButtonGroup에는 Radio Button 만 가능합니다!!"));
			return;
		}
	}
}

void URadioButtonGroup::OnClickRadioButton(UButtonBase* _btn)
{
	if (IsInvalid(_btn))
		return;

	SelectRadioButtonByWidgetID(_btn->GetWidgetID());
}

void URadioButtonGroup::SelectRadioButtonByWidgetID(FName _widget_id)
{
	for (auto child : StackBox->GetAllChildren())
	{
		auto radio_btn = Cast<URadioButton>(child);
		if (IsValid(radio_btn))
		{
			if (radio_btn->GetWidgetID() == _widget_id)
			{
				radio_btn->SetIsSelected(true);

				if (_OnRadioButtonSelected.IsBound())
					_OnRadioButtonSelected.Broadcast(radio_btn);
			}
			else
			{
				radio_btn->SetIsSelected(false);
			}
		}
	}
}

void URadioButtonGroup::SelectRadioButtonByIndex(int32 _index)
{
	if (_index < 0 || _index >= StackBox->GetChildrenCount())
	{
		TRACE_WARNING(TEXT("out of index : %d"), _index);
		_index = FMath::Clamp(_index, 0, StackBox->GetChildrenCount() - 1);
	}

	int32 idx = 0;
	for (auto child : StackBox->GetAllChildren())
	{
		auto radio_btn = Cast<URadioButton>(child);
		if (IsValid(radio_btn))
		{
			if (idx == _index)
			{
				radio_btn->SetIsSelected(true);
				
				if (_OnRadioButtonSelected.IsBound())
					_OnRadioButtonSelected.Broadcast(radio_btn);
			}
			else
			{
				radio_btn->SetIsSelected(false);
			}

			idx++;
		}
	}
}
