// Fill out your copyright notice in the Description page of Project Settings.


#include "KkwallaHUD.h"
#include "Blueprint/UserWidget.h"
#include "BreakPage.h"

AKkwallaHUD::AKkwallaHUD()
{
	BreakTimeWidgetAsset = FSoftClassPath(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/GameMake/UMG/UMG_LD_B.UMG_LD_B_C'"));
	FailWidgetAsset = FSoftClassPath(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/GameMake/UMG/UMG_LD_A.UMG_LD_A_C'"));
}

void AKkwallaHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AKkwallaHUD::InitSet()
{
	UUserWidget* CreatedWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), BreakTimeWidgetAsset.LoadSynchronous());
	BreakWidget = Cast<UBreakPage>(CreatedWidget);
	if (IsValid(BreakWidget))
	{
		BreakWidget->AddToViewport();
		ChangeBreakWidgetVisibility(false);
	}

	FailWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), FailWidgetAsset.LoadSynchronous());
	if (IsValid(FailWidget))
	{
		FailWidget->AddToViewport();
		ShowFailePage(false);
	}
}

void AKkwallaHUD::ChangeBreakWidgetVisibility(bool bShow, FLinearColor InColorA /*= FLinearColor::Black*/, FLinearColor InColorB /*= FLinearColor::Black*/)
{
	if (IsValid(BreakWidget) == false)
	{
		return;
	}

	if (bShow)
	{
		BreakWidget->SetVisibility(ESlateVisibility::Visible);
		BreakWidget->SetBGColor(InColorA, InColorB);
		BreakWidget->PlayAnimA();
	}
	else
	{
		BreakWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AKkwallaHUD::ShowFailePage(bool bShow)
{
	if (IsValid(FailWidget) == false)
	{
		return;
	}

	if (bShow)
	{
		FailWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		FailWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AKkwallaHUD::PlayAnimCount(int32 InCount)
{
	switch (InCount)
	{
	case 1:
		BreakWidget->PlayAnimCount1();
	break;
	case 2:
		BreakWidget->PlayAnimCount2();
	break;
	case 3:
		BreakWidget->PlayAnimCount3();
	break;
	default:
		break;
	}
}
