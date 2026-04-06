// Fill out your copyright notice in the Description page of Project Settings.


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