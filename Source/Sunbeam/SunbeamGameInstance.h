// Copyright Project SunBeam. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "SunbeamGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SUNBEAM_API USunbeamGameInstance : public UPlatformGameInstance
{
	GENERATED_BODY()

public:
	void ChangeMap(int index);

	UPROPERTY(BlueprintReadOnly)
	int CurrentMapIndex = 2;

	bool HardwareControlEnabled = true;
};
