// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlock.h"
#include "TypingRichTextBlock.generated.h"


UCLASS()
class CUSTOMUI_API UTypingRichTextBlock : public URichTextBlock
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Content")
	bool _AutoStart = false;

	UPROPERTY(EditAnywhere, Category = "Content")
	FText _FullText = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content")
	float _TypingSpeed = 10.0f;

protected:
	TArray<FString> _TypingTextList;

	bool _IsTyping = false;

	FTimerHandle _TimerHandle;

	int32 _TypingIndex = 0;
	FString _TypingAccumulatedStr;

protected:
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool _release_children) override;

public:
	UFUNCTION(BlueprintCallable)
	void StartTyping(bool _is_resume);

	UFUNCTION(BlueprintCallable)
	void StopTyping(bool _is_clear);

	UFUNCTION(BlueprintCallable)
	void SetTypingSpeed(float _speed);

protected:
	void RefineTypingText();

	void TickTyping();

public:
	UFUNCTION(BlueprintPure)
	bool GetIsTyping() const { return _IsTyping; }

	UFUNCTION(BlueprintPure)
	float GetTypingSpeed() const { return _TypingSpeed; }
};
