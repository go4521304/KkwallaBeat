// Fill out your copyright notice in the Description page of Project Settings.


#include "KkwallaHUD.h"
#include "Blueprint/UserWidget.h"
#include "BreakPage.h"
#include "FailPageWidget.h"

AKkwallaHUD::AKkwallaHUD()
{
	BreakTimeWidgetAsset = FSoftClassPath(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/GameMake/UMG/UMG_LD_B.UMG_LD_B_C'"));
	FailWidgetAsset = FSoftClassPath(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/GameMake/UMG/UMG_Fail.UMG_Fail_C'"));
}

void AKkwallaHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AKkwallaHUD::InitSet()
{
	if (IsValid(BreakWidget) == false)
	{
		UUserWidget* CreatedWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), BreakTimeWidgetAsset.LoadSynchronous());
		BreakWidget = Cast<UBreakPage>(CreatedWidget);
	}
	BreakWidget->AddToViewport();
	ChangeBreakWidgetVisibility(false);

	if (IsValid(FailWidget) == false)
	{
		FailWidget = CreateWidget<UFailPageWidget>(GetOwningPlayerController(), FailWidgetAsset.LoadSynchronous());
	}
	FailWidget->AddToViewport();
	ShowFailePage(false);
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

void AKkwallaHUD::ShowFailePage(bool bShow, int32 InPlayerNum /*= 0*/)
{
	if (IsValid(FailWidget) == false)
	{
		return;
	}

	if (bShow)
	{
		FailWidget->SetFailPlayer(InPlayerNum);
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
	case 4:
		BreakWidget->PlayCount_Go();
	break;
	default:
		break;
	}
}
