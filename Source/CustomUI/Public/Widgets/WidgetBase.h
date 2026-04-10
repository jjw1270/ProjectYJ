// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomUI.h"
#include "Blueprint/UserWidget.h"
#include "Sound/SoundCue.h"
#include "WidgetBase.generated.h"

USTRUCT(BlueprintType)
struct FWidgetAnimConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EUMGSequencePlayMode::Type> PlayType = EUMGSequencePlayMode::Forward;

	UPROPERTY(EditAnywhere)
	bool IsRestoreState = false;
};

UENUM(BlueprintType)
enum class EWidgetState : uint8
{
	Hide,
	Showing,		// Start Anim 재생중
	Idle,
	Hiding,			// Hide Anim 재생중
};

UENUM(BlueprintType)
enum class EWidgetShowType : uint8
{
	Visible,
	HitTestInvisible,
	SelfHitTestInvisible
};

UENUM(BlueprintType)
enum class EWidgetHideType : uint8
{
	NA										UMETA(Hidden),
	RemoveFromParent			UMETA(Hidden),
	Collapsed,
	Hidden
};

UCLASS(Abstract)
class CUSTOMUI_API UWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName _WidgetID = FName();

public:
	void SetWidgetID(FName _widget_id)
	{
		_WidgetID = _widget_id;
	}

	UFUNCTION(BlueprintPure)
	FName GetWidgetID() const { return _WidgetID; }

protected:
	UPROPERTY(EditAnywhere, meta = (EditCondition = " ShowAnim != nullptr", Tooltip = "첫 프레임 opacity를 0으로 해서 깜빡임 방지\nShowAnim이 있을때만 동작합니다."))
	bool _IsPreventInitialFlicker = true;

	UPROPERTY(EditAnywhere, meta = (Tooltip = "Show일 경우 하위 모든 위젯의 Show를 호출할 것인지?"))
	bool _IsTraverseWidgetToShow = true;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> ShowAnim = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> IdleAnim = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> HideAnim = nullptr;

	UPROPERTY(EditAnywhere, Category = "Anim")
	FWidgetAnimConfig _ShowAnimConfig;

	UPROPERTY(EditAnywhere, Category = "Anim")
	FWidgetAnimConfig _IdleAnimConfig;

	UPROPERTY(EditAnywhere, Category = "Anim")
	FWidgetAnimConfig _HideAnimConfig;

	UPROPERTY()
	TObjectPtr<UWidgetAnimation> _CurrentAnim = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundCue> _ShowSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundCue> _HideSound = nullptr;

private:
	EWidgetState _WidgetState = EWidgetState::Hide;
	EWidgetHideType _WidgetHideType = EWidgetHideType::NA;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* _anim) override;
	virtual void SynchronizeProperties() override;

public:
	virtual void SetVisibility(ESlateVisibility _visibility) override;

protected:
	UFUNCTION()
	virtual void OnVisibilityChanged(ESlateVisibility _visibility);

	// "BP"에서 변수에 변경이 있을 때 호출됩니다.
	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction))
	void OnSynchronizeProperties();

public:
	void SetPreventInitialFlicker(bool _value)
	{
		_IsPreventInitialFlicker = _value;
	}

	UFUNCTION(BlueprintCallable)
	void Show(EWidgetShowType _show_type, bool _is_skip_anim = false);

	void TraverseWidgetToShow();
	bool CanTraverseWidget(UWidget* _widget) const;
	void TraverseWidgetToShow_Internal(UWidget* _widget);

	UFUNCTION(BlueprintCallable)
	void Hide(EWidgetHideType _hide_type, bool _is_skip_anim = false);

	UFUNCTION(BlueprintCallable)
	void Close(bool _is_skip_anim = false);

private:
	void SetWidgetState(EWidgetState _new_state);
	void HideWidget();

public:
	UFUNCTION(BlueprintPure)
	EWidgetState GetWidgetState() const { return _WidgetState; }

#pragma region Event
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDM_OnShowWidget, UWidgetBase*, _widget);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDM_OnIdleWidget, UWidgetBase*, _widget);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDM_OnClosingWidget, UWidgetBase*, _widget);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDM_OnCloseWidget, UWidgetBase*, _widget);

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, meta = (Tooltip = "Show Anim 시작"))
	FDM_OnShowWidget _OnShowEvent;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, meta = (Tooltip = "Idle Anim 시작 (최초 1회)"))
	FDM_OnIdleWidget _OnIdleEvent;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, meta = (Tooltip = "Close 요청 직후, Close Anim 시작"))
	FDM_OnClosingWidget _OnClosingEvent;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, meta = (Tooltip = "Hide 직전, 내부 위젯 접근 가능, Remove/Hide 전"))
	FDM_OnCloseWidget _OnCloseEvent;

	UFUNCTION(BlueprintNativeEvent)
	void OnShow();
	virtual void OnShow_Implementation() {};

	UFUNCTION(BlueprintNativeEvent)
	void OnIdle();
	virtual void OnIdle_Implementation() {};

	UFUNCTION(BlueprintNativeEvent)
	void OnClosing();
	virtual void OnClosing_Implementation() {};

	UFUNCTION(BlueprintNativeEvent)
	void OnClose();
	virtual void OnClose_Implementation() {};

#pragma endregion Event

};
