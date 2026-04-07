// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/WidgetBase.h"
#include "Components/RadioButton.h"
#include "RadioButtonGroup.generated.h"


UCLASS(Abstract)
class CUSTOMUI_API URadioButtonGroup : public UWidgetBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UStackBox> StackBox = nullptr;

protected:
	UPROPERTY(EditAnywhere, Category = "RadioButtonGroup")
	TEnumAsByte<EOrientation> _Orientation = EOrientation::Orient_Horizontal;

	UPROPERTY(EditAnywhere, Category = "RadioButtonGroup")
	TEnumAsByte<EHorizontalAlignment> _HorizontalAlignment = EHorizontalAlignment::HAlign_Fill;

	UPROPERTY(EditAnywhere, Category = "RadioButtonGroup")
	TEnumAsByte<EVerticalAlignment> _VerticalAlignment = EVerticalAlignment::VAlign_Fill;

	UPROPERTY(EditAnywhere, Category = "RadioButtonGroup")
	FMargin _ButtonPadding = FMargin();

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDM_OnRadioButtonSelected, URadioButton*, _btn);

	UPROPERTY(BlueprintAssignable)
	FDM_OnRadioButtonSelected _OnRadioButtonSelected;

protected:
	virtual void SynchronizeProperties() override;

protected:
	void UpdateRadioButtons();

	UFUNCTION()
	void OnClickRadioButton(class UButtonBase* _btn);

public:
	UFUNCTION(BlueprintCallable)
	void SelectRadioButtonByWidgetID(FName _widget_id);

	UFUNCTION(BlueprintCallable)
	void SelectRadioButtonByIndex(int32 _index);

};
