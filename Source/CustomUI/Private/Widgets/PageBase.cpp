// Copyright (c) 2026 장윤제. All rights reserved.


#include "Widgets/PageBase.h"

void UPageBase::SynchronizeProperties()
{
#if FEATURE_PAGE
	Super::SynchronizeProperties();

	if(_PageConfig.SetFocus != IsFocusable())
	{
		SetIsFocusable(_PageConfig.SetFocus);
	}
#endif
}