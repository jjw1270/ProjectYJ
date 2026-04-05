// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WidgetPlayerController.generated.h"

class UPageBase;

UCLASS(Abstract)
class CUSTOMUI_API AWidgetPlayerController : public APlayerController
{
	GENERATED_BODY()
	
/* Deprecated
protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPageBase> _InitialPageClass = nullptr;

public:
	TSubclassOf<UPageBase> GetInitialPageClass() const { return _InitialPageClass; }
*/
};
