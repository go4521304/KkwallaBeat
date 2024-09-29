// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleHUD.h"
#include "Blueprint/UserWidget.h"
#include "CreditPageWidget.h"

ATitleHUD::ATitleHUD()
{
	CreditPageWidgetAsset = FSoftClassPath(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/GameMake/UMG/UMG_ED.UMG_ED_C'"));
}

void ATitleHUD::InitSet()
{
	UUserWidget* CreatedWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), CreditPageWidgetAsset.LoadSynchronous());
	
	if (IsValid(CreatedWidget))
	{
		CreditPageWidget = Cast<UCreditPageWidget>(CreatedWidget);
		CreditPageWidget->AddToViewport();
		CreditPageWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ATitleHUD::ShowCreditPage(bool bShow)
{
	CreditPageWidget->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (bShow)
	{
		CreditPageWidget->ShowCredit();
	}
}
