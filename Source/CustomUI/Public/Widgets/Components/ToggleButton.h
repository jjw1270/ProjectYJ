// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ButtonBase.h"
#include "ToggleButton.generated.h"

/**
 *
 */
UCLASS(Abstract, Blueprintable)
class CUSTOMUI_API UToggleButton : public UButtonBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ToggleButton")
	bool _IsSelected = false;

	UPROPERTY(EditAnywhere, Category = "ToggleButton")
	TMap<EButtonState, FButtonStyleConfig> _SelectedStateStyles;

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDM_OnSelectChanged, UToggleButton*, _btn, bool, _is_selected);

	UPROPERTY(BlueprintAssignable)
	FDM_OnSelectChanged _OnSelectChanged;
	
protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& _geo, const FPointerEvent& _mouse_event) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& _mouse_event) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& _geo, const FPointerEvent& _mouse_event) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& _geo, const FPointerEvent& _mouse_event) override;

public:
	UFUNCTION(BlueprintCallable)
	void SetIsSelected(bool _is_selected, bool _force_update = false);

	UFUNCTION(BlueprintCallable)
	bool ToggleSelected();

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnSelectChanged();
	void OnSelectChanged_Implementation() {};

	virtual void UpdateButtonStyle() override;

public:
	UFUNCTION(BlueprintPure)
	bool GetIsSelected() const { return _IsSelected; }

};
