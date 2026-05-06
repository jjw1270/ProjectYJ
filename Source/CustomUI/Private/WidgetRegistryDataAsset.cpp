// Copyright (c) 2026 장윤제. All rights reserved.


#include "WidgetRegistryDataAsset.h"
#include "CommonUtils.h"
#include "Widgets/WidgetBase.h"

#if WITH_EDITOR
void UWidgetRegistryDataAsset::PostEditChangeProperty(FPropertyChangedEvent& _property_changed_event)
{
	Super::PostEditChangeProperty(_property_changed_event);
}
#endif
