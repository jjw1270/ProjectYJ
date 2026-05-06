# CustomUI

`CustomUI`는 Unreal Engine 5 프로젝트에서 공통 UMG 위젯 기반 클래스, 팝업/등록 위젯 관리, 버튼 계열 컴포넌트, 리치 텍스트 타이핑, 마키(흐르는) 위젯, String Table 조회 헬퍼를 제공하는 런타임 UI 플러그인이다.

이 문서는 `Plugins/CustomUIPlugin`의 **현재 코드 기준**으로 유지되는 기술 README다. 실제 사용 절차 중심 문서는 [UserGuide.md](./UserGuide.md)를 참고한다.

---

## 핵심 요약

- 플러그인은 `CustomUI` 런타임 모듈 하나로 구성된다.
- `CommonLibrary` 플러그인에 의존한다.
- 전역 UI 설정은 `UCustomUIDeveloperSettings`에 저장한다.
- `UWidgetSubsystem`은 LocalPlayer 단위로 등록 위젯과 팝업 스택을 관리한다.
- `UWidgetHelper`는 Blueprint/C++에서 등록 위젯 조회, 팝업 열기/닫기, String Table 텍스트 조회를 제공한다.
- `UWidgetBase`는 `Show/Idle/Hide` 상태와 `ShowAnim`, `IdleAnim`, `HideAnim` 애니메이션 흐름을 표준화한다.
- `UPopupBase`는 `OpenPopup`으로 Viewport에 추가되고, `Close` 시 팝업 스택에서 정리된다.
- `UPageBase`와 Page API는 현재 `FEATURE_PAGE=0`이고 Deprecated 처리되어 사용하지 않는 개념이다.
- 버튼 계열은 `UButtonBase`를 기반으로 Click, Toggle, Radio, RadioButtonGroup을 제공한다.
- `UTypingRichTextBlock`은 RichText 마크업을 유지하면서 글자 단위 타이핑을 수행한다.
- `UMarqueeWidgetBase`는 슬롯 콘텐츠를 가로/세로로 스크롤 또는 왕복 스크롤한다.

---

## 모듈 구성

플러그인은 `CommonLibrary` 플러그인에 의존한다.

| 모듈 | 역할 |
| --- | --- |
| `CustomUI` | 런타임 UI 기반 클래스, LocalPlayer Subsystem, Widget Helper, Widget Registry, Popup 관리, 버튼/텍스트/마키 위젯 컴포넌트 |

`CustomUI.Build.cs`의 주요 의존 모듈:

- `Core`, `CoreUObject`, `Engine`
- `InputCore`
- `UMG`
- `CommonLibrary`
- `DeveloperSettings`
- `Slate`, `SlateCore`(Private)

빌드 정의:

| 정의 | 현재 값 | 의미 |
| --- | --- | --- |
| `FEATURE_PAGE` | `0` | Page 시스템 비활성화. `UPageBase`/`OpenPage`는 Deprecated 경로다. |

---

## 주요 런타임 타입

### `UCustomUIDeveloperSettings`

파일: `Source/CustomUI/Public/CustomUIDeveloperSettings.h`

Project Settings에 저장되는 전역 설정이다.

| 설정 | 의미 |
| --- | --- |
| `_StringTables` | `UStringTableSubsystem`이 시작 시 로드할 String Table 목록 |
| `_WidgetRegistryDataAsset` | `UWidgetSubsystem`이 등록 위젯 목록으로 사용할 Data Asset |

### `UStringTableSubsystem`

파일: `Source/CustomUI/Public/StringTableSubsystem.h`

`UGameInstanceSubsystem`이다. 시작 시 `UCustomUIDeveloperSettings::_StringTables`를 동기 로드하고, String Table 에셋 이름을 `StringTableId`로 매핑한다.

주요 API:

```cpp
FText UStringTableSubsystem::FindTextFromTable(const FString& _table_name, const FString& _key);
```

`UWidgetHelper::GetStringTableText`와 `GETTEXT(_table_name, _key)` 매크로가 이 API를 감싼다.

주의:

- `_table_name`은 현재 코드상 String Table 에셋의 `GetName()` 기준이다.
- 설정에 없는 테이블 이름을 넘기면 빈 `FName` 기반으로 조회되므로, Project Settings의 `_StringTables` 등록을 먼저 확인해야 한다.

### `UWidgetRegistryDataAsset`

파일: `Source/CustomUI/Public/WidgetRegistryDataAsset.h`

공용 위젯 클래스를 이름으로 등록하는 Data Asset이다.

| 데이터 | 의미 |
| --- | --- |
| `_WidgetClassMap` | `FName -> TSubclassOf<UWidgetBase>` 매핑 |

`UWidgetSubsystem`은 초기화 시 이 맵을 읽어 `_RegisteredWidgetDataMap`을 만든다. `GetRegisteredWidget` 호출 시 인스턴스가 없으면 지연 생성하고 이후 같은 인스턴스를 재사용한다.

### `UWidgetSubsystem`

파일: `Source/CustomUI/Public/WidgetSubsystem.h`

`ULocalPlayerSubsystem`이다. LocalPlayer/PlayerController 기준으로 UI 인스턴스를 관리한다.

주요 책임:

- Widget Registry 로드 및 등록 위젯 지연 생성
- Popup 생성, Viewport 추가, 스택 관리
- Popup 중복/차단 규칙 적용
- Level 변경 또는 PlayerController 변경 시 남길 Popup 복구
- `UInputSettings::DoubleClickTime`을 `UClickButton` 더블클릭 임계값으로 반영
- Page 시스템 코드 보유(현재 비활성/Deprecated)

주요 내부 흐름:

| 함수 | 의미 |
| --- | --- |
| `Initialize` | Registry 위젯과 버튼 설정 초기화 |
| `PlayerControllerChanged` | 기존 UI 정리 후 Remaining Page/Popup 복구 |
| `GetRegisteredWidget` | Registry에 등록된 위젯 인스턴스를 지연 생성/반환 |
| `OpenPopup` | Popup 위젯 생성, `EWidgetZOrder::Popup`으로 Viewport 추가 |
| `OnHidePopup` | RemoveFromParent로 닫힌 Popup을 스택과 Remaining 목록에서 제거 |
| `GetTopPopup` | 현재 팝업 스택의 마지막 Popup 반환 |
| `CheckCanOpenPopup` | 유효성, 중복 허용, 상단 Popup의 IgnoreOtherPopup 규칙 검사 |

### `UWidgetHelper`

파일: `Source/CustomUI/Public/WidgetHelper.h`

Blueprint/C++에서 사용하는 정적 헬퍼다.

| 함수 | 의미 |
| --- | --- |
| `IsDesignTime` | 위젯의 Design Time 여부 확인 |
| `GetStringTableText` | 등록된 String Table에서 `FText` 조회 |
| `GetRegisteredWidget` | Registry에 등록된 공용 위젯 인스턴스 조회 |
| `OpenPopup` | Popup Class로 Popup 열기 |
| `ClosePopup` | Popup 닫기 |
| `GetTopPopup` | 현재 최상단 Popup 조회 |
| `OpenPage` | Deprecated. `FEATURE_PAGE=0`에서는 동작하지 않음 |

C++ 템플릿 오버로드는 구체 타입 반환을 돕는다.

```cpp
auto Popup = UWidgetHelper::OpenPopup<UMyPopup>(this, UMyPopup::StaticClass());
```

### `AWidgetPlayerController`

파일: `Source/CustomUI/Public/WidgetPlayerController.h`

BeginPlay에서 초기 Input Mode와 Mouse Cursor 상태를 적용하는 PlayerController 기반 클래스다.

| 설정 | 의미 |
| --- | --- |
| `_InitialInputMode` | `GameAndUI`, `GameOnly`, `UIOnly` 중 초기 입력 모드 |
| `_InMouseLockMode` | UI/GameAndUI 모드에서 마우스 락 방식 |
| `_HideCursorDuringCapture` | GameAndUI 입력 중 캡처 시 커서 숨김 여부 |
| `_ConsumeCaptureMouseDown` | GameOnly 입력 모드에서 캡처 마우스 다운 소비 여부 |
| `_ShowMouseCursor` | BeginPlay 후 커서 표시 여부 |
| `_InitialPageClass` | Deprecated. Page 시스템용 잔여 설정 |

`UWidgetSubsystem::OpenPopup`은 LocalPlayer의 PlayerController를 `AWidgetPlayerController`로 캐스팅한다. 팝업 시스템을 사용하려면 프로젝트 PlayerController가 이 클래스를 상속하는 것이 안전하다.

---

## Widget Base 흐름

### `UWidgetBase`

파일: `Source/CustomUI/Public/Widgets/WidgetBase.h`

모든 CustomUI 위젯의 기본 클래스다. `UUserWidget` 위에 표시/숨김 상태, 애니메이션, 사운드, 이벤트를 얹는다.

주요 데이터:

| 데이터 | 의미 |
| --- | --- |
| `_WidgetID` | Registry/그룹 선택 등에 사용하는 위젯 식별자 |
| `_IsPreventInitialFlicker` | ShowAnim이 있을 때 첫 프레임 Opacity 0으로 깜빡임 방지 |
| `_IsTraverseWidgetToShow` | Show 시 하위 `UWidgetBase`들도 Showing 상태로 순회할지 |
| `ShowAnim` | 선택적 Show 애니메이션. `BindWidgetAnimOptional` |
| `IdleAnim` | 선택적 Idle 반복/상태 애니메이션 |
| `HideAnim` | 선택적 Hide 애니메이션 |
| `_ShowAnimConfig`, `_IdleAnimConfig`, `_HideAnimConfig` | 애니메이션 재생 방향/복원 설정 |
| `_ShowSound`, `_HideSound` | Show/Hide 사운드 |

상태:

| 상태 | 의미 |
| --- | --- |
| `Hide` | 숨김/제거 상태 |
| `Showing` | ShowAnim 재생 중 또는 Show 진입 중 |
| `Idle` | 표시 완료 상태 |
| `Hiding` | HideAnim 재생 중 |

주요 API:

```cpp
void Show(EWidgetShowType _show_type, bool _is_skip_anim = false);
void Hide(EWidgetHideType _hide_type, bool _is_skip_anim = false);
void Close(bool _is_skip_anim = false);
EWidgetState GetWidgetState() const;
```

이벤트:

| 이벤트 | 호출 시점 |
| --- | --- |
| `_OnShowEvent` / `OnShow` | `Showing` 진입 직후 |
| `_OnIdleEvent` / `OnIdle` | 표시 완료 후 `Idle` 진입 |
| `_OnStartHideEvent` / `OnStartHide` | Hide/Close 요청 직후 |
| `_OnHideEvent` / `OnHide` | 실제 숨김/제거 직전 |
| `OnSynchronizeProperties` | `SynchronizeProperties`에서 Blueprint로 전달 |

중요 정책:

- 직접 `SetVisibility`를 호출하면 경고를 출력한다. 표시/숨김은 `Show`, `Hide`, `Close`를 사용한다.
- `Close()`는 `Hide(EWidgetHideType::RemoveFromParent)`와 같다.
- `Hide(Collapsed/Hidden)`는 Viewport에서 제거하지 않고 Visibility만 변경한다.

### `UPopupBase`

파일: `Source/CustomUI/Public/Widgets/PopupBase.h`

Popup 전용 기본 클래스다. `UWidgetBase`를 상속하고 Construct 시 Focusable을 true로 설정한다.

설정:

| 설정 | 의미 |
| --- | --- |
| `_PopupConfig.IgnoreOtherPopup` | 이 Popup이 열려 있는 동안 다른 Popup 열기를 차단 |
| `_PopupConfig.AllowDuplicate` | 같은 Popup Class 중복 열기 허용 |
| `_PopupConfig.ShowMouseCursor` | Popup이 열렸을 때 PlayerController 커서 표시 여부 |
| `_PopupConfig.RemainOnLevelChanged` | PlayerController 변경/레벨 변경 후 다시 열 Popup 목록에 유지 |

### `UPageBase` (Deprecated)

파일: `Source/CustomUI/Public/Widgets/PageBase.h`

과거 Page 시스템용 기본 클래스다. 현재 `FEATURE_PAGE=0`으로 빌드되며 클래스와 Helper API가 Deprecated 처리되어 있다.

현재 정책:

- 신규 UI는 `UWidgetBase` 또는 `UPopupBase`를 사용한다.
- `OpenPage` 경로에 의존하지 않는다.

---

## 버튼/컴포넌트 타입

### `UButtonBase`

파일: `Source/CustomUI/Public/Widgets/Components/ButtonBase.h`

Click/Toggle/Radio 버튼의 공통 기반이다.

필수 BindWidget 구성:

| 이름 | 타입 | 의미 |
| --- | --- | --- |
| `SizeBox` | `USizeBox` | 고정 폭/높이 적용 |
| `Border` | `UBorder` | 상태별 Brush/ContentColor 적용 |
| `NS_Content` | `UNamedSlot` | 버튼 내부 콘텐츠 슬롯 |

상태:

- `Normal`
- `Hovered`
- `Pressed`
- `Disabled`

주요 기능:

- `_StateStyles`로 상태별 Brush/ContentColor 적용
- `_UseFixedWidth`, `_UseFixedHeight`, `_FixedSize`로 크기 제어
- `_ContentHorizontalAlignment`, `_ContentVerticalAlignment`, `_ContentPadding`으로 콘텐츠 정렬/패딩 제어
- `_HoverSound`, `_ClickSound` 재생
- `_OnClicked`, `_OnButtonStateChanged` 이벤트 제공
- `SetButtonDisabled`로 Disabled 상태 전환

정책:

- `SetIsEnabled(false)` 대신 `SetButtonDisabled(true)` 사용을 권장하며, 직접 비활성화 시 경고를 출력한다.
- 기본 클릭 키는 `LeftMouseButton`이다.

### `UClickButton`

파일: `Source/CustomUI/Public/Widgets/Components/ClickButton.h`

일반 클릭 버튼이다.

추가 기능:

- 좌클릭 `_OnClicked`
- 우클릭 등 보조 클릭 `_OnSubClicked`
- 더블클릭 `_OnDoubleClicked`
- `_DontResetButtonStateOnMouseLeaveWhenPressed`
- `_UseSubClick`
- `_SubClickSound`

정책:

- 보조 클릭 기본 키는 `RightMouseButton`이다.
- 더블클릭 임계값은 `UInputSettings::DoubleClickTime`으로 초기화된다.

### `UToggleButton`

파일: `Source/CustomUI/Public/Widgets/Components/ToggleButton.h`

클릭할 때 선택 상태를 토글하는 버튼이다.

주요 API:

```cpp
void SetIsSelected(bool _is_selected, bool _force_update = false);
bool ToggleSelected();
bool GetIsSelected() const;
```

선택 상태일 때는 `_SelectedStateStyles`를 우선 적용한다.

### `URadioButton`

파일: `Source/CustomUI/Public/Widgets/Components/RadioButton.h`

그룹 안에서 하나만 선택되는 버튼이다. 개별 버튼은 `_IsSelected`와 `_SelectedStateStyles`를 가진다.

주요 API:

```cpp
void SetIsSelected(bool _is_selected, bool _force_update = false);
bool GetIsSelected() const;
```

클릭 시 `_OnClicked`를 Broadcast하고 자신을 선택 상태로 만든다. 일반적으로 `URadioButtonGroup` 안에서 사용한다.

### `URadioButtonGroup`

파일: `Source/CustomUI/Public/Widgets/Components/RadioButtonGroup.h`

여러 `URadioButton`을 하나의 선택 그룹으로 관리한다.

필수 BindWidget 구성:

| 이름 | 타입 | 의미 |
| --- | --- | --- |
| `StackBox` | `UStackBox` | RadioButton 자식들을 담는 컨테이너 |

주요 기능:

- `StackBox` Orientation 설정
- 버튼 슬롯 Padding/Alignment/SizeRule 적용
- 자식이 `URadioButton`이 아니면 오류 출력
- `WidgetID` 또는 Index로 선택
- 선택 시 `_OnRadioButtonSelected` Broadcast

주요 API:

```cpp
void SelectRadioButtonByWidgetID(FName _widget_id);
void SelectRadioButtonByIndex(int32 _index);
```

주의:

- `SelectRadioButtonByWidgetID`를 쓰려면 각 `URadioButton`의 `_WidgetID`가 설정되어 있어야 한다.

### `UTypingRichTextBlock`

파일: `Source/CustomUI/Public/Widgets/Components/TypingRichTextBlock.h`

`URichTextBlock`을 상속한 타이핑 텍스트 컴포넌트다.

주요 데이터:

| 데이터 | 의미 |
| --- | --- |
| `_AutoStart` | 런타임 `SynchronizeProperties` 후 자동 타이핑 시작 |
| `_FullText` | 전체 텍스트 |
| `_TypingSpeed` | 초당 출력 토큰 수. 최소 1로 보정 |

주요 API:

```cpp
void StartTyping(bool _is_resume);
void StopTyping(bool _is_clear);
void SetTypingSpeed(float _speed);
bool GetIsTyping() const;
float GetTypingSpeed() const;
```

마크업 처리:

- `<Style>문자</>` 형태의 RichText 스타일은 각 문자 토큰에 다시 감싸서 누적 표시한다.
- `\r\n`, `\r`, `\n`은 줄바꿈 토큰으로 유지한다.
- `<img .../>` 같은 셀프 클로징 태그는 하나의 토큰으로 유지한다.
- Design Time에서는 전체 텍스트를 즉시 표시한다.

### `UMarqueeWidgetBase`

파일: `Source/CustomUI/Public/Widgets/MarqueeWidgetBase.h`

NamedSlot 콘텐츠를 가로/세로로 움직이는 마키 위젯이다.

필수 BindWidget 구성:

| 이름 | 타입 | 의미 |
| --- | --- | --- |
| `CanvasPanel` | `UCanvasPanel` | 콘텐츠가 잘릴 영역. 자동으로 `ClipToBounds` 설정 |
| `NS_Content` | `UNamedSlot` | 이동할 콘텐츠 슬롯 |

설정:

| 설정 | 의미 |
| --- | --- |
| `_MarqueeType` | `NA`, `Scroll`, `AlternateScroll` |
| `_Orientation` | Horizontal 또는 Vertical |
| `_IsReverse` | 이동 방향 반전 |
| `_MarqueeSpeed` | 이동 속도 |
| `_DelayTime` | 끝점 도달 후 대기 시간 |

타입:

- `Scroll`: 콘텐츠가 영역 밖에서 반대쪽 밖까지 흐른 뒤 시작 위치로 돌아간다.
- `AlternateScroll`: 영역 안에서 시작점과 끝점을 왕복한다.

### `UAdvancedDragDropOperation`

파일: `Source/CustomUI/Public/AdvancedDragDropOperation.h`

`UDragDropOperation`에 드래그 시작/로컬 오프셋 정보를 추가한 간단한 데이터 클래스다.

| 데이터 | 의미 |
| --- | --- |
| `_LocalOffset` | 위젯 내부에서 Pivot 기준 마우스 Offset |
| `_DragStartScreenPos` | 드래그 시작 Screen Position |

---

## 런타임 흐름

### 등록 위젯 조회

1. `UWidgetSubsystem`이 초기화된다.
2. `UCustomUIDeveloperSettings::_WidgetRegistryDataAsset`을 동기 로드한다.
3. Registry의 `_WidgetClassMap`을 `FName -> Class`로 저장한다.
4. `UWidgetHelper::GetRegisteredWidget(WorldContext, WidgetName)` 호출 시 인스턴스가 없으면 생성한다.
5. 생성된 인스턴스는 같은 LocalPlayer Subsystem 안에서 재사용된다.

등록 위젯은 반환만 한다. Viewport 추가, 부모 패널 배치, 이벤트 바인딩/해제는 호출자가 처리한다.

### 팝업 열기

1. `UWidgetHelper::OpenPopup` 호출
2. LocalPlayer의 `UWidgetSubsystem` 조회
3. PlayerController를 `AWidgetPlayerController`로 캐스팅
4. Popup Class 유효성 검사
5. `AllowDuplicate == false`면 같은 Class가 이미 열려 있는지 검사
6. 최상단 Popup의 `IgnoreOtherPopup`이 true면 새 Popup 열기 차단
7. `CreateWidget<UPopupBase>` 실행
8. Popup `_OnHideEvent`에 Subsystem 정리 콜백 바인딩
9. `EWidgetZOrder::Popup`으로 Viewport 추가
10. `RemainOnLevelChanged`이면 Remaining 목록에 Class 보관
11. Popup 설정에 맞춰 PlayerController Mouse Cursor 갱신
12. `_CurrentPopups` 스택에 추가

### 팝업 닫기

1. `UWidgetHelper::ClosePopup(Popup, SkipAnim)` 또는 `Popup->Close(SkipAnim)` 호출
2. `UWidgetBase::Hide(RemoveFromParent)` 실행
3. HideAnim이 있으면 재생, 없거나 skip이면 즉시 Hide 상태
4. `_OnHideEvent` Broadcast
5. Subsystem이 RemoveFromParent 닫힘을 감지해 스택/Remaining 목록에서 제거
6. 다음 최상단 Popup 또는 PlayerController 기본값에 따라 Mouse Cursor 갱신

주의:

- Popup을 `Hide(Collapsed/Hidden)`으로 숨기면 RemoveFromParent가 아니므로 `_CurrentPopups`에서 제거되지 않는다.
- 팝업 스택 정리까지 기대한다면 `Close()`를 사용하는 것이 안전하다.

### WidgetBase Show/Hide

Show:

```text
Show() -> Visibility 설정 -> Showing -> OnShow/_OnShowEvent -> ShowAnim -> Idle -> OnIdle/_OnIdleEvent
```

Hide/Close:

```text
Hide()/Close() -> OnStartHide/_OnStartHideEvent -> Hiding -> HideAnim -> Hide -> OnHide/_OnHideEvent -> Remove/Collapsed/Hidden
```

`_IsTraverseWidgetToShow`가 true이면 Show 진입 시 표시 중인 하위 `UWidgetBase`들도 Showing 상태로 순회한다. `WidgetSwitcher`의 비활성 자식은 순회하지 않는다.

---

## Content / Sample

플러그인은 `CanContainContent=true`이며 샘플 콘텐츠를 포함한다.

주요 샘플 경로:

| 경로 | 내용 |
| --- | --- |
| `Content/Sample/DA_SampleWidgetRegistry.uasset` | Widget Registry 샘플 Data Asset |
| `Content/Sample/L_Sample.umap` | 샘플 레벨 |
| `Content/Sample/L_Sample2.umap` | 샘플 레벨 2 |
| `Content/Sample/Widgets/...` | Popup, Button, Toggle, Radio, Marquee 등 샘플 위젯 Blueprint |
| `Content/Sample/RichTextBlock/...` | RichText 스타일/이미지 데코레이터 샘플 |
| `Content/Sample/ST_Sample.uasset` | String Table 샘플 |

---

## 현재 한계 / 주의사항

- `FEATURE_PAGE=0`이므로 Page 시스템은 비활성화되어 있다.
- `UPageBase`와 `UWidgetHelper::OpenPage`는 Deprecated 경로다.
- `OpenPopup`은 `AWidgetPlayerController` 캐스팅에 의존한다.
- `GetRegisteredWidget`은 위젯을 생성/반환만 하며 자동으로 Viewport에 추가하지 않는다.
- Registry와 String Table은 설정된 Soft Object를 초기화 시 동기 로드한다.
- `SetVisibility`와 `SetIsEnabled(false)`는 직접 사용보다 `Show/Hide`와 `SetButtonDisabled` 사용을 권장한다.
- `GETTEXT` 매크로는 끝에 세미콜론이 포함되어 있으므로 표현식 위치에서 사용할 때 주의한다.
- Editor 전용 모듈이나 전용 Asset Factory는 제공하지 않는다. Data Asset/Widget Blueprint는 일반 Unreal 생성 흐름을 사용한다.

---

## 추천 코드 읽기 순서

1. `CustomUI.Build.cs`
2. `WidgetDefines.h`
3. `CustomUIDeveloperSettings.h`
4. `WidgetRegistryDataAsset.h`
5. `WidgetSubsystem.h/.cpp`
6. `WidgetHelper.h/.cpp`
7. `StringTableSubsystem.h/.cpp`
8. `Widgets/WidgetBase.h/.cpp`
9. `Widgets/PopupBase.h/.cpp`
10. `Widgets/Components/ButtonBase.h/.cpp`
11. `Widgets/Components/ClickButton.h/.cpp`
12. `Widgets/Components/ToggleButton.h/.cpp`
13. `Widgets/Components/RadioButton.h/.cpp`
14. `Widgets/Components/RadioButtonGroup.h/.cpp`
15. `Widgets/Components/TypingRichTextBlock.h/.cpp`
16. `Widgets/MarqueeWidgetBase.h/.cpp`
17. `WidgetPlayerController.h/.cpp`
18. `AdvancedDragDropOperation.h/.cpp`

---

## 관련 문서

- [User Guide](./UserGuide.md) — 플러그인을 사용하는 디자이너/개발자용 작업 가이드
