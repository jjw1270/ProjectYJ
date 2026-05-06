// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "AdvancedDragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMUI_API UAdvancedDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (Tooltip = "위젯 내부에서 피벗으로부터의 마우스 Offset"))
	FVector2D _LocalOffset = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadWrite, meta = (Tooltip = "마우스 시작 position"))
	FVector2D _DragStartScreenPos = FVector2D::ZeroVector;

};
