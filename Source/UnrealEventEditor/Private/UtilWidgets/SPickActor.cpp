#include "UtilWidgets/SPickActor.h"
#include "Widgets/Images/SImage.h"
#include "GameFramework/Actor.h"
#include "UtilWidgets/SSelectActorDialog.h"

void SPickActor::Construct(const FArguments& InArgs)
{
	OnActorSelected = InArgs._OnActorSelected;
	OnGetAllowedClasses = InArgs._OnGetAllowedClasses;
	OnShouldPickActor = InArgs._OnShouldPickActor;
	PickedActor = InArgs._DefaultActor;
	
	// default bound 
	if (!OnGetAllowedClasses.IsBound())
	{
		OnGetAllowedClasses = FOnGetAllowedClasses::CreateLambda(
			[](TArray<const UClass*>& InClass)
			{
				InClass.Add(AActor::StaticClass());
			});
	}
	if (!OnShouldPickActor.IsBound())
	{
		OnShouldPickActor = FOnShouldFilterActor::CreateLambda(
			[](const AActor*)
			{
				return true;
			});
	}

	static TSharedPtr<FButtonStyle> BtnStyle;
	if (!BtnStyle.IsValid())
	{
		BtnStyle = MakeShared<FButtonStyle>(FEditorStyle::Get().GetWidgetStyle<FButtonStyle>(TEXT("PropertyEditor.AssetComboStyle")));
		BtnStyle->Normal.TintColor = FLinearColor(0.07,0.07,0.07,1);
		BtnStyle->Hovered.TintColor = FLinearColor(0.04,0.04,0.04,1);
		BtnStyle->Pressed.TintColor = FLinearColor(0.03,0.03,0.03,1);
	}
	
	ChildSlot
	[
		SNew(SButton)
		.ButtonStyle( BtnStyle.Get()  )
        .IsEnabled(true)
        .ContentPadding(2.0f)
        .OnClicked_Raw(this, &SPickActor::_OnOpenSelectActorWindow)
        [
	        SNew(SHorizontalBox)
	        + SHorizontalBox::Slot()
	        .Padding(3,0,0,0)
	        .FillWidth(1)
	        .VAlign(VAlign_Center)
	        [
	            // Show the name of the asset or actor
	            SNew(STextBlock)
	            .TextStyle( FEditorStyle::Get(), "PropertyEditor.AssetClass" )
	            .Font( FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
	            .Text_Raw(this, &SPickActor::_OnGetActorName)
	        ]
	        + SHorizontalBox::Slot()
	        .AutoWidth()
	        .VAlign(VAlign_Center)
	        [
	            SNew(SButton)
	            .ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
	            .OnClicked(this, &SPickActor::_OnPickActor)
	            .ContentPadding(4.0f)
	            .ForegroundColor(FSlateColor::UseForeground())
	            .IsFocusable(false)
	            [ 
	                SNew(SImage)
	                .Image(FEditorStyle::GetBrush("PropertyWindow.Button_PickActorInteractive"))
	                .ColorAndOpacity(FSlateColor::UseForeground())
	            ]
	        ]
	        + SHorizontalBox::Slot()
	        .AutoWidth()
	        .VAlign(VAlign_Center)
	        [
		        SNew(SButton)
	            .ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
	            .OnClicked(this, &SPickActor::_OnFindActor)
	            .ContentPadding(4.0f)
	            .ForegroundColor(FSlateColor::UseForeground())
	            .IsFocusable(false)
	        	[
	        		SNew(SImage)
	        		.Image(FEditorStyle::GetBrush("PropertyWindow.Button_Browse"))
	        		.ColorAndOpacity(FSlateColor::UseForeground())
	        	]
	        ]
        ]
	];
}

SPickActor::~SPickActor()
{
}

void SPickActor::SetPickedActor(AActor* InActor)
{
	if (InActor != PickedActor)
	{
		PickedActor = InActor;
		OnActorSelected.ExecuteIfBound(InActor);
	}
}

FText SPickActor::_OnGetActorName() const
{
	return FText::FromString(PickedActor.Get() ? PickedActor.Get()->GetActorLabel() : TEXT("None"));
}

FReply SPickActor::_OnOpenSelectActorWindow()
{
	TSharedPtr<SSelectActorDialog> PickWin = SNew(SSelectActorDialog)
		.ShoudPickActor(OnShouldPickActor)
		.InitActor(PickedActor.Get());
	
	SetPickedActor(PickWin->ShowModal());
	return FReply::Handled();
}

FReply SPickActor::_OnPickActor()
{
	FActorPickerModeModule& ActorPickerMode = FModuleManager::Get().GetModuleChecked<FActorPickerModeModule>("ActorPickerMode");

	if(ActorPickerMode.IsInActorPickingMode())
	{
		ActorPickerMode.EndActorPickingMode();
	}
	else
	{
		ActorPickerMode.BeginActorPickingMode(
            OnGetAllowedClasses,
            OnShouldPickActor,
            FOnActorSelected::CreateRaw(this, &SPickActor::SetPickedActor));
	}

	return FReply::Handled();
}

FReply SPickActor::_OnFindActor()
{
	if (PickedActor.Get())
	{
		GEditor->MoveViewportCamerasToActor(*PickedActor.Get(), false);
		GEditor->SelectNone(true, true);
		GEditor->SelectActor(PickedActor.Get(), true, true);
	}
	
	return FReply::Handled();
}
