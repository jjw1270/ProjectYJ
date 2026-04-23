// Copyright (c) 2026 장윤제. All rights reserved.


#include "WidgetPlayerController.h"

void AWidgetPlayerController::BeginPlay()
{
	Super::BeginPlay();

	switch (_InitialInputMode)
	{
	case EInputMode::GameAndUI:
	{
		FInputModeGameAndUI input_mode;
		input_mode.SetLockMouseToViewportBehavior(_InMouseLockMode);
		input_mode.SetHideCursorDuringCapture(_HideCursorDuringCapture);
		SetInputMode(input_mode);
	}
		break;

	case EInputMode::GameOnly:
	{
		FInputModeGameOnly input_mode;
		input_mode.SetConsumeCaptureMouseDown(_ConsumeCaptureMouseDown);
		SetInputMode(input_mode);
	}
		break;

	case EInputMode::UIOnly:
		{
			FInputModeUIOnly input_mode;
			input_mode.SetLockMouseToViewportBehavior(_InMouseLockMode);
			SetInputMode(input_mode);
		}
		break;

	default:
		break;
	}

	SetShowMouseCursor(_ShowMouseCursor);
}
