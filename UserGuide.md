# CustomUI User Guide

이 문서는 CustomUI 플러그인을 실제 프로젝트에서 사용하는 사람을 위한 작업 가이드다. 코드 구조 설명보다 **무엇을 만들고, 어디에 설정하고, 어떻게 열고 닫고, 어떤 규칙을 지켜야 하는지**에 초점을 둔다.

---

## 1. 기본 개념

CustomUI는 다음 단위로 UI를 구성한다.

| 이름 | 설명 |
| --- | --- |
| WidgetBase | Show/Hide 상태, 애니메이션, 사운드, 이벤트를 가진 기본 UserWidget |
| PopupBase | `OpenPopup`으로 열고 `Close`로 닫는 팝업 위젯 |
| Widget Registry | 자주 쓰는 공용 위젯을 `FName`으로 찾아 재사용하기 위한 목록 |
| WidgetSubsystem | LocalPlayer 단위로 Registry 위젯과 Popup 스택을 관리하는 Subsystem |
| WidgetHelper | Blueprint/C++에서 Registry, Popup, String Table 기능을 호출하는 헬퍼 |
| Button Components | Click, Toggle, Radio, RadioButtonGroup 기반 버튼 위젯 |
| TypingRichTextBlock | RichText 텍스트를 타이핑 효과로 표시하는 텍스트 블록 |
| MarqueeWidgetBase | 콘텐츠를 흐르게 표시하는 위젯 |

가장 흔한 흐름은 다음과 같다.

```text
Project Settings 설정
-> Widget Blueprint가 UWidgetBase/UPopupBase 상속
-> 필요한 곳에서 UWidgetHelper::OpenPopup 또는 GetRegisteredWidget 호출
-> Show/Hide/Close와 이벤트로 UI 수명 주기 처리
```

---

## 2. 처음 설정하기

### 2.1 PlayerController 준비

Popup 시스템은 LocalPlayer의 PlayerController를 `AWidgetPlayerController`로 캐스팅해서 사용한다.

권장:

1. 프로젝트 PlayerController Blueprint 또는 C++ 클래스가 `AWidgetPlayerController`를 상속하게 한다.
2. GameMode의 PlayerController Class에 이 클래스를 지정한다.
3. PlayerController Details에서 초기 입력 모드를 설정한다.

주요 설정:

| 설정 | 추천 기준 |
| --- | --- |
| InitialInputMode | 게임 위주면 `GameOnly`, UI 조작이 필요하면 `GameAndUI` 또는 `UIOnly` |
| InMouseLockMode | UI 전용 화면이면 `DoNotLock`, 게임 화면이면 프로젝트 정책에 맞게 설정 |
| HideCursorDuringCapture | GameAndUI에서 마우스 캡처 중 커서를 숨길지 결정 |
| ConsumeCaptureMouseDown | GameOnly에서 첫 클릭 소비 여부 |
| ShowMouseCursor | 시작 시 커서 표시 여부 |

### 2.2 Widget Registry Data Asset 만들기

공용으로 재사용할 위젯이 있다면 Registry를 만든다.

1. Content Browser에서 `Data Asset`을 생성한다.
2. 클래스 선택 창에서 `WidgetRegistryDataAsset`을 선택한다.
3. `_WidgetClassMap`에 이름과 Widget Class를 등록한다.

예:

| Key | Widget Class |
| --- | --- |
| `EditableTextBox` | `UI_EditableTextBox_BP` |
| `CommonConfirmPopup` | `UI_ConfirmPopup_BP` |

주의:

- Key가 `None`이면 등록되지 않는다.
- Widget Class가 비어 있으면 등록되지 않는다.
- Registry 위젯은 처음 조회할 때 생성되고 이후 재사용된다.
- Registry 위젯은 자동으로 Viewport에 올라가지 않는다. 필요한 위치에 직접 `AddToViewport` 또는 Panel 추가를 수행한다.

### 2.3 Project Settings 설정

Project Settings에서 CustomUI 설정을 찾고 다음을 지정한다.

| 설정 | 필수 | 설명 |
| --- | --- | --- |
| WidgetRegistryDataAsset | Registry 기능 사용 시 필수 | 방금 만든 Registry Data Asset |
| StringTables | String Table 헬퍼 사용 시 필수 | `GETTEXT`/`GetStringTableText`로 조회할 String Table 목록 |

String Table은 에셋 이름으로 조회한다.

```cpp
FText Text = UWidgetHelper::GetStringTableText(TEXT("ST_Office"), TEXT("Set_Nickname_IsEmpty"));
```

또는 매크로를 사용할 수 있다.

```cpp
FText Text = GETTEXT("ST_Office", "Set_Nickname_IsEmpty")
```

주의: `GETTEXT` 매크로 정의에는 세미콜론이 포함되어 있다. 일반 대입문처럼 쓰는 용도에 맞춰 사용한다.

---

## 3. WidgetBase 위젯 만들기

### 3.1 Blueprint 부모 클래스 선택

일반 UI는 `WidgetBase`를 부모로 만든다.

사용할 수 있는 이벤트:

- `OnShow`
- `OnIdle`
- `OnStartHide`
- `OnHide`
- `OnSynchronizeProperties`

사용할 수 있는 함수:

- `Show`
- `Hide`
- `Close`
- `GetWidgetState`
- `GetWidgetID`

### 3.2 애니메이션 이름 규칙

WidgetBase는 다음 이름의 Widget Animation을 자동으로 바인딩한다.

| 애니메이션 이름 | 용도 |
| --- | --- |
| `ShowAnim` | Show 진입 시 재생 |
| `IdleAnim` | Show 완료 후 Idle 상태에서 재생 |
| `HideAnim` | Hide/Close 시 재생 |

모두 선택 사항이다. 없으면 상태가 즉시 다음 단계로 넘어간다.

### 3.3 Show/Hide 사용 규칙

직접 `SetVisibility`를 호출하지 말고 다음 함수를 사용한다.

```cpp
MyWidget->Show(EWidgetShowType::Visible);
MyWidget->Hide(EWidgetHideType::Collapsed);
MyWidget->Close(); // RemoveFromParent
```

Blueprint에서도 같은 규칙을 따른다.

| 하고 싶은 일 | 함수 |
| --- | --- |
| 보이기 | `Show` |
| 숨기되 Viewport/부모에는 남기기 | `Hide(Collapsed)` 또는 `Hide(Hidden)` |
| 닫고 제거하기 | `Close` |
| 애니메이션 없이 즉시 처리 | `Show(..., true)` / `Hide(..., true)` / `Close(true)` |

### 3.4 이벤트 순서 이해하기

Show 순서:

```text
Show 호출
-> OnShow / OnShowEvent
-> ShowAnim 재생
-> OnIdle / OnIdleEvent
-> IdleAnim 재생
```

Close 순서:

```text
Close 호출
-> OnStartHide / OnStartHideEvent
-> HideAnim 재생
-> OnHide / OnHideEvent
-> RemoveFromParent
```

정리 작업 추천 위치:

| 작업 | 추천 이벤트 |
| --- | --- |
| 표시 직후 데이터 갱신 | `OnShow` |
| ShowAnim이 끝난 뒤 입력 허용 | `OnIdle` |
| 닫기 시작 시 입력 차단 | `OnStartHide` |
| 이벤트 바인딩 해제/참조 정리 | `OnHide` |

---

## 4. Popup 만들고 열기

### 4.1 Popup Blueprint 만들기

1. Widget Blueprint를 만든다.
2. 부모 클래스를 `PopupBase`로 지정한다.
3. Details에서 `_PopupConfig`를 설정한다.

Popup Config:

| 설정 | 설명 |
| --- | --- |
| IgnoreOtherPopup | 이 Popup이 최상단에 있으면 다른 Popup 열기를 막는다. 모달 팝업에 적합하다. |
| AllowDuplicate | 같은 Popup Class를 여러 개 열 수 있게 한다. |
| ShowMouseCursor | Popup이 열려 있는 동안 PlayerController 커서를 보일지 결정한다. |
| RemainOnLevelChanged | PlayerController 변경/레벨 변경 후 같은 Popup Class를 다시 연다. |

### 4.2 Blueprint에서 Popup 열기

Blueprint에서 `Open Popup` 노드를 사용한다.

입력:

- World Context
- Popup Class

반환값이 유효하면 Popup이 이미 Viewport에 추가된 상태다.

닫을 때는 Popup 자기 자신에게 `Close`를 호출하거나 `Close Popup` 헬퍼를 사용한다.

### 4.3 C++에서 Popup 열기

```cpp
#include "WidgetHelper.h"
#include "Widgets/PopupBase.h"

UPopupBase* Popup = UWidgetHelper::OpenPopup(this, PopupClass);
if (IsValid(Popup))
{
	Popup->_OnHideEvent.AddDynamic(this, &UMyObject::OnPopupClosed);
}
```

구체 타입으로 받고 싶으면 템플릿 오버로드를 사용할 수 있다.

```cpp
UMyPopup* Popup = UWidgetHelper::OpenPopup<UMyPopup>(this, UMyPopup::StaticClass());
```

### 4.4 Popup 닫기

```cpp
UWidgetHelper::ClosePopup(Popup, false);
```

또는:

```cpp
Popup->Close(false);
```

팝업 스택에서 제거되려면 `Close`를 사용하는 것이 안전하다. `Hide(Collapsed)`나 `Hide(Hidden)`은 Viewport에서 제거하지 않으므로 Popup 스택에 남을 수 있다.

### 4.5 중복/차단 규칙

Popup 열기가 실패할 수 있는 경우:

| 상황 | 결과 |
| --- | --- |
| Popup Class가 비어 있음 | 열기 실패 |
| 같은 Class가 이미 열려 있고 `AllowDuplicate=false` | 열기 실패 |
| 최상단 Popup의 `IgnoreOtherPopup=true` | 새 Popup 열기 실패 |
| PlayerController가 `AWidgetPlayerController`가 아님 | 열기 실패 |

---

## 5. Registry 위젯 사용하기

Registry는 공용 위젯 인스턴스를 이름으로 찾아 재사용할 때 사용한다.

### 5.1 Blueprint에서 조회

`Get Registered Widget` 노드를 호출한다.

입력:

- World Context
- Widget Name

반환값을 원하는 위젯 타입으로 Cast해서 사용한다.

### 5.2 C++에서 조회

```cpp
#include "WidgetHelper.h"

UWidgetBase* Widget = UWidgetHelper::GetRegisteredWidget(this, TEXT("EditableTextBox"));
if (IsValid(Widget))
{
	Widget->AddToViewport(20);
	Widget->Show(EWidgetShowType::Visible);
}
```

프로젝트 코드 예시 패턴:

```cpp
_EditableTextBoxWidget = Cast<UUI_EditableTextBox>(
	UWidgetHelper::GetRegisteredWidget(this, TEXT("EditableTextBox"))
);
```

### 5.3 이벤트 바인딩 주의

Registry 위젯은 재사용되므로 이벤트를 바인딩했다면 닫힐 때 해제하는 것이 안전하다.

```cpp
void UMyWidget::OnHideRegisteredWidget(UWidgetBase* _widget, EWidgetHideType _hide_type)
{
	RegisteredWidget->_OnHideEvent.RemoveAll(this);
	RegisteredWidget = nullptr;
}
```

---

## 6. 버튼 위젯 만들기

### 6.1 공통 버튼 구조

`ClickButton`, `ToggleButton`, `RadioButton` 계열은 내부에 다음 BindWidget 이름을 가진 위젯이 필요하다.

| 이름 | 타입 | 역할 |
| --- | --- | --- |
| `SizeBox` | SizeBox | 고정 크기 제어 |
| `Border` | Border | Brush와 ContentColor 적용 |
| `NS_Content` | NamedSlot | 버튼 내부 콘텐츠 |

권장 위젯 트리:

```text
Root Canvas/Overlay
└─ SizeBox (이름: SizeBox)
   └─ Border (이름: Border)
      └─ NamedSlot (이름: NS_Content)
```

### 6.2 상태 스타일 설정

`_StateStyles`에 상태별 스타일을 넣는다.

| 상태 | 용도 |
| --- | --- |
| Normal | 기본 상태 |
| Hovered | 마우스 오버 |
| Pressed | 누른 상태 |
| Disabled | 비활성 상태 |

`ToggleButton`과 `RadioButton`은 선택 상태에서 `_SelectedStateStyles`를 우선 사용한다.

### 6.3 ClickButton

일반 클릭 버튼이다.

사용 이벤트:

- `_OnClicked`
- `_OnDoubleClicked`
- `_OnSubClicked`

설정:

| 설정 | 설명 |
| --- | --- |
| _UseSubClick | 우클릭 등 보조 클릭 사용 여부 |
| _SubClickSound | 보조 클릭 사운드 |
| _DontResetButtonStateOnMouseLeaveWhenPressed | 누른 채 마우스가 나갈 때 Pressed 유지 여부 |

### 6.4 ToggleButton

클릭할 때 선택 상태가 바뀐다.

사용 이벤트:

- `_OnClicked`
- `_OnSelectChanged`

Blueprint/C++에서 강제로 선택 상태를 바꿀 수 있다.

```cpp
ToggleButton->SetIsSelected(true);
bool bNowSelected = ToggleButton->ToggleSelected();
```

### 6.5 RadioButtonGroup

여러 RadioButton 중 하나만 선택하려면 `RadioButtonGroup`을 사용한다.

필수 구조:

```text
RadioButtonGroup Widget
└─ StackBox (이름: StackBox)
   ├─ RadioButton
   ├─ RadioButton
   └─ RadioButton
```

각 RadioButton에는 `_WidgetID`를 지정하는 것이 좋다.

선택 API:

```cpp
RadioButtonGroup->SelectRadioButtonByWidgetID(TEXT("GraphicsHigh"));
RadioButtonGroup->SelectRadioButtonByIndex(0);
```

선택 결과는 `_OnRadioButtonSelected`에서 받는다.

주의:

- StackBox 자식은 모두 `URadioButton`이어야 한다.
- WidgetID가 `None`이면 WidgetID 기반 선택을 할 수 없다.

### 6.6 Disabled 처리

버튼을 비활성화할 때 `SetIsEnabled(false)`를 직접 호출하지 않는다.

대신:

```cpp
Button->SetButtonDisabled(true);
```

다시 활성화:

```cpp
Button->SetButtonDisabled(false);
```

---

## 7. TypingRichTextBlock 사용하기

### 7.1 위젯에 추가

1. Widget Blueprint에 `TypingRichTextBlock`을 배치한다.
2. `_FullText`에 전체 텍스트를 입력한다.
3. `_TypingSpeed`를 설정한다.
4. 자동 시작이 필요하면 `_AutoStart`를 켠다.

### 7.2 수동 시작/중지

```cpp
TypingText->StartTyping(false); // 처음부터 시작
TypingText->StopTyping(false);  // 현재 텍스트 유지하고 중지
TypingText->StopTyping(true);   // 비우고 초기화
TypingText->SetTypingSpeed(20.0f);
```

### 7.3 RichText 마크업 사용

스타일 태그는 유지된다.

```text
<Red>경고</> 메시지입니다.
```

이미지 같은 셀프 클로징 태그는 하나의 토큰으로 취급한다.

```text
<img id="Icon"/> 아이콘 설명
```

주의:

- 스타일 종료 태그는 `</>` 형태를 기준으로 처리한다.
- Design Time에서는 타이핑하지 않고 전체 텍스트가 즉시 보인다.

---

## 8. MarqueeWidget 사용하기

### 8.1 Blueprint 구조 만들기

`MarqueeWidgetBase`를 부모로 Widget Blueprint를 만든다.

필수 위젯 트리:

```text
Root
└─ CanvasPanel (이름: CanvasPanel)
   └─ NamedSlot (이름: NS_Content)
      └─ 실제 표시할 텍스트/이미지/위젯
```

`CanvasPanel`은 자동으로 `ClipToBounds`가 적용된다.

### 8.2 설정

| 설정 | 설명 |
| --- | --- |
| MarqueeType | `Scroll` 또는 `AlternateScroll` |
| Orientation | 가로/세로 방향 |
| IsReverse | 시작 방향 반전 |
| MarqueeSpeed | 이동 속도 |
| DelayTime | 끝점에서 대기하는 시간 |

### 8.3 타입 선택

| 타입 | 사용 상황 |
| --- | --- |
| Scroll | 공지 문구처럼 한 방향으로 계속 흐르게 할 때 |
| AlternateScroll | 영역 안에서 왔다 갔다 움직이게 할 때 |

---

## 9. String Table 텍스트 사용하기

### 9.1 설정

Project Settings의 CustomUI `StringTables` 배열에 사용할 String Table 에셋을 추가한다.

### 9.2 Blueprint에서 조회

`Get String Table Text` 노드를 사용한다.

입력:

- Table Name: String Table 에셋 이름
- Key: String Table Key

### 9.3 C++에서 조회

```cpp
FText ErrorText = UWidgetHelper::GetStringTableText(TEXT("ST_Office"), TEXT("Set_Nickname_IsEmpty"));
```

매크로:

```cpp
FText ErrorText = GETTEXT("ST_Office", "Set_Nickname_IsEmpty")
```

---

## 10. Advanced DragDrop Operation 사용하기

드래그 시작 위치와 로컬 오프셋이 필요한 경우 `AdvancedDragDropOperation`을 사용한다.

보관되는 값:

| 값 | 설명 |
| --- | --- |
| LocalOffset | 위젯 내부 Pivot 기준 마우스 오프셋 |
| DragStartScreenPos | 드래그 시작 Screen Position |

Blueprint DragDrop 구현에서 기본 `DragDropOperation` 대신 이 클래스를 쓰면 드래그 중 위치 계산에 필요한 값을 함께 전달할 수 있다.

---

## 11. 추천 작업 순서

새 UI를 만들 때는 이 순서가 가장 안전하다.

1. PlayerController가 `AWidgetPlayerController`를 상속하는지 확인한다.
2. Project Settings의 CustomUI 설정을 채운다.
3. 공용 위젯이면 `WidgetRegistryDataAsset`에 등록한다.
4. 일반 위젯은 `WidgetBase`, 팝업은 `PopupBase`를 부모로 만든다.
5. 필요하면 `ShowAnim`, `IdleAnim`, `HideAnim`을 만든다.
6. 버튼은 요구되는 `SizeBox`, `Border`, `NS_Content` 이름을 맞춘다.
7. RadioButtonGroup은 `StackBox`와 자식 `RadioButton` 구조를 맞춘다.
8. 표시/숨김은 `Show`, `Hide`, `Close`로만 처리한다.
9. Popup은 `OpenPopup`으로 열고 `Close`로 닫는다.
10. Registry 위젯에 바인딩한 이벤트는 `OnHide`에서 해제한다.
11. PIE에서 Popup 중복, 커서 표시, HideAnim 종료 후 제거 여부를 확인한다.

---

## 12. 운영 팁

- UI 수명 주기는 `SetVisibility`보다 `Show/Hide/Close` 기준으로 설계한다.
- Popup은 `Hide(Collapsed)`보다 `Close()`로 닫는 것이 스택 정리에 안전하다.
- Registry 위젯은 재사용되므로 이전 상태/이벤트가 남지 않게 초기화 루틴을 둔다.
- Modal 팝업은 `IgnoreOtherPopup=true`, 일반 알림 팝업은 `AllowDuplicate` 정책을 명확히 정한다.
- 버튼 비활성은 `SetButtonDisabled`로 처리해 Disabled 스타일과 상태가 일관되게 유지되도록 한다.
- RichText 타이핑은 복잡한 중첩 마크업보다 단순한 `<Style>text</>` 패턴을 권장한다.
- Page 시스템은 Deprecated이므로 신규 화면 전환 설계에는 사용하지 않는다.
