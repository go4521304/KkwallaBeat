// Fill out your copyright notice in the Description page of Project Settings.


#include "FailPageWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "EngineUtils.h"
#include "ManagerInterface.h"

void UFailPageWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UMaterialInterface* Mat = Cast<UMaterialInterface>(Img_fail->GetBrush().GetResourceObject());
	if (IsValid(Mat))
	{
		FailMatInst = UMaterialInstanceDynamic::Create(Mat, nullptr, TEXT("MatInst"));
		Img_fail->Brush.SetResourceObject(FailMatInst);
	}

	Btn_Re->OnClicked.AddDynamic(this, &ThisClass::RetryGame);

	for (AActor* Iter : TActorRange<AActor>(GetWorld()))
	{
		if (IsValid(Iter) && Iter->Implements<UManagerInterface>())
		{
			ManagerActor = Iter;
		}
	}
}

void UFailPageWidget::SetFailPlayer(int32 InPlayerIndex)
{
	FailMatInst->SetScalarParameterValue(TEXT("Index"), InPlayerIndex);
}

void UFailPageWidget::RetryGame()
{
	ManagerActor->Restart();
}
