// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ManagerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UManagerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TOUCHRHYTHM_API IManagerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void TouchInput(const FVector& InPos) = 0;
	virtual void Restart() = 0;
	virtual void SetPlayerNum(const int32 InPlayerNum);
};
