// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "CommonLibrary.h"
#include "WidgetDefines.h"

class FCustomUIModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
