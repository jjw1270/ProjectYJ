// Copyright (c) 2026 장윤제. All rights reserved.


#include "Widgets/Components/TypingRichTextBlock.h"
#include "CustomUI.h"

void UTypingRichTextBlock::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	RefineTypingText();

	if (IsDesignTime())
	{
		FString full_str;
		for (const auto& str : _TypingTextList)
		{
			full_str.Append(str);
		}

		SetText(FText::FromString(full_str));
	}
	else
	{
		if (_AutoStart)
			StartTyping(false);
	}
}

void UTypingRichTextBlock::ReleaseSlateResources(bool _release_children)
{
	Super::ReleaseSlateResources(_release_children);

	if (_IsTyping)
	{
		StopTyping(true);
	}
}

void UTypingRichTextBlock::RefineTypingText()
{
	FString full_str = _FullText.ToString();

	_TypingTextList.Empty();

	FString style;

	auto WrapIfStyled = [&](const TCHAR ch)
		{
			if (style.IsEmpty())
			{
				_TypingTextList.Add(FString(1, &ch));
			}
			else
			{
				FString wrapped;
				wrapped.Reserve(style.Len() + 5);
				wrapped += TEXT("<");
				wrapped += style;
				wrapped += TEXT(">");
				wrapped.AppendChar(ch);
				wrapped += TEXT("</>");
				_TypingTextList.Add(MoveTemp(wrapped));
			}
		};

	for (int32 i = 0; i < full_str.Len();)
	{
		const TCHAR ch = full_str[i];

		// 줄바꿈: \r\n/\r/\n -> "\n" (스타일로 감싸지 않음)
		if (ch == TEXT('\r'))
		{
			if (i + 1 < full_str.Len() && full_str[i + 1] == TEXT('\n'))
			{
				_TypingTextList.Add(TEXT("\n"));
				i += 2;
			}
			else
			{
				_TypingTextList.Add(TEXT("\n"));
				i += 1;
			}
			continue;
		}
		if (ch == TEXT('\n'))
		{
			_TypingTextList.Add(TEXT("\n"));
			i += 1;
			continue;
		}

		if (ch == TEXT('<'))
		{
			const int32 close_idx = full_str.Find(TEXT(">"), ESearchCase::CaseSensitive, ESearchDir::FromStart, i);
			if (close_idx == INDEX_NONE)
			{
				// 비정상: 그냥 문자로 취급
				WrapIfStyled(ch);
				++i;
				continue;
			}

			const FString tag_inner = full_str.Mid(i + 1, close_idx - i - 1).TrimStartAndEnd();
			const FString full_tag = full_str.Mid(i, close_idx - i + 1); // "<...>"

			// 종료 태그: 오직 "</>"만
			if (tag_inner.Equals(TEXT("/"), ESearchCase::CaseSensitive))
			{
				style.Empty();
				i = close_idx + 1;
				continue;
			}

			// 셀프 클로징: "<img .../>" 등은 태그 자체를 토큰으로 유지
			const FString tag_inner_trim = tag_inner.TrimStartAndEnd();
			if (tag_inner_trim.EndsWith(TEXT("/")))
			{
				_TypingTextList.Add(full_tag);
				i = close_idx + 1;
				continue;
			}

			// 여는 스타일 태그: "<Red>" 형태만 (속성 태그는 스타일로 취급하지 않음)
			FString tag_name = tag_inner_trim;
			{
				int32 space_idx = INDEX_NONE;
				if (tag_name.FindChar(TEXT(' '), space_idx))
				{
					// "<tag attr=...>" 형태면 스타일 태그가 아니라고 보고 무시
					i = close_idx + 1;
					continue;
				}
			}

			// 데코레이터 태그는 스타일로 잡지 않음(필요시 목록 확장)
			if (tag_name.Equals(TEXT("img"), ESearchCase::IgnoreCase))
			{
				i = close_idx + 1;
				continue;
			}

			style = tag_name;
			i = close_idx + 1;
			continue;
		}

		WrapIfStyled(ch);
		++i;
	}
}

void UTypingRichTextBlock::StartTyping(bool _is_resume)
{
	if (_IsTyping)
		return;

	auto world = GetWorld();
	if (IsInvalid(world))
		return;

	if(_is_resume == false)
	{
		StopTyping(true);
	}

	if (_TypingTextList.Num() == 0)
	{
		SetText(FText::GetEmpty());
		return;
	}

	_IsTyping = true;

	FTimerDelegate delegate;
	delegate.BindUObject(this, &UTypingRichTextBlock::TickTyping);

	const float rate = 1.0f / FMath::Max(1.0f, _TypingSpeed);

	world->GetTimerManager().SetTimer(_TimerHandle, delegate, rate, true, 0.0f);
}

void UTypingRichTextBlock::StopTyping(bool _is_clear)
{
	if (_IsTyping)
	{
		auto world = GetWorld();
		if (IsValid(world))
		{
			world->GetTimerManager().ClearTimer(_TimerHandle);
		}
	}

	_IsTyping = false;

	if (_is_clear)
	{
		_TypingIndex = 0;
		_TypingAccumulatedStr.Empty();

		SetText(FText::GetEmpty());
	}
}

void UTypingRichTextBlock::SetTypingSpped(float _speed)
{
	_TypingSpeed = _speed;

	// 타이핑 중이 아니면 값만 바꾸고 종료
	if (!_IsTyping)
		return;

	auto world = GetWorld();
	if (!IsValid(world))
		return;

	// 기존 타이머가 유효한지 확인
	if (world->GetTimerManager().IsTimerActive(_TimerHandle) == false)
		return;

	world->GetTimerManager().ClearTimer(_TimerHandle);

	FTimerDelegate delegate;
	delegate.BindUObject(this, &UTypingRichTextBlock::TickTyping);

	const float rate = 1.0f / FMath::Max(1.0f, _TypingSpeed);
	world->GetTimerManager().SetTimer(_TimerHandle, delegate, rate, true, 0.0f);
}

void UTypingRichTextBlock::TickTyping()
{
	if (_TypingIndex >= _TypingTextList.Num())
	{
		StopTyping(false);
		return;
	}

	_TypingAccumulatedStr += _TypingTextList[_TypingIndex];
	++_TypingIndex;

	SetText(FText::FromString(_TypingAccumulatedStr));
}
