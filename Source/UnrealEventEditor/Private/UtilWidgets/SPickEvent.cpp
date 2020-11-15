#include "UtilWidgets/SPickEvent.h"

#include "EventBinderLibrary.h"
#include "Helps/UnrealEventHelp.h"

void SPickEvent::Construct(const FArguments& InArgs)
{
	SourceMap = InArgs._Source;
	OnPickEvent = InArgs._OnPickEvent;
	GetShouldPickEvent = InArgs._GetShouldPickEvent;

	if (!GetShouldPickEvent.IsBound())
	{
		GetShouldPickEvent.BindLambda([](FName){ return true; });
	}
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(SearchBox, SEditableTextBox)
			.OnTextChanged_Lambda([&](const FText&) { _RebuildEventSignatureList(); })
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(EventSignatureList, SVerticalBox)
		]
	];

	_RebuildEventSignatureList();
}

SPickEvent::~SPickEvent()
{
}

FReply SPickEvent::_OnSelectEvent(FName InEventName)
{
	OnPickEvent.ExecuteIfBound(InEventName);
	return FReply::Handled();
}

void SPickEvent::_RebuildEventSignatureList()
{
	EventSignatureList->ClearChildren();
	if (!SourceMap) return;

	for (auto & Pair : *SourceMap)
	{
		if (GetShouldPickEvent.Execute(Pair.Key) &&
			UnrealEventHelp::CanPassFuzzySearch(Pair.Key.ToString(), SearchBox->GetText().ToString()))
		{
			EventSignatureList->AddSlot()
			.AutoHeight()
			[
				_BuildEventBtn(Pair.Key)
			];
		}
	}
}

TSharedRef<SWidget> SPickEvent::_BuildEventBtn(FName InEventName)
{
	static TSharedPtr<FButtonStyle> BtnStyle;
	if (!BtnStyle.IsValid())
	{
		BtnStyle = MakeShared<FButtonStyle>(FCoreStyle::Get().GetWidgetStyle<FButtonStyle>(ISlateStyle::Join(TEXT("Menu"), ".Button")));
		FSlateBrush Brush;
		Brush.TintColor = FLinearColor(0.1,0.1,0.1,1);
		BtnStyle->Hovered = Brush;
	}
	
	return SNew(SButton)
        .ButtonStyle(BtnStyle.Get())
        .OnClicked_Raw(this, &SPickEvent::_OnSelectEvent, InEventName)
        [
            SNew(STextBlock)
            .Text(FText::FromString(FEventBinderLibrary::AppendSignature(InEventName, *SourceMap->Find(InEventName))))
            .ColorAndOpacity(FLinearColor::White)
        ];
}
