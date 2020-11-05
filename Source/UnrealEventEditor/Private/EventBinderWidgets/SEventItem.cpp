#include "EventBinderWidgets/SEventItem.h"

#include "EventBinderLibrary.h"
#include "EventBinderWidgets/SDelegateItem.h"

void SEventItem::Construct(const FArguments& InArgs)
{
	TargetSignature = InArgs._TargetSignature;
	EventBindInfoSource = InArgs._Source;
	EventName = InArgs._EventName;
	OnEventModify = InArgs._OnEventModify;
	OnDeleteEvent = InArgs._OnDeleteEvent;
	OnReplaceEvent = InArgs._OnReplaceEvent;
	OnCollapsedStateChanged = InArgs._OnCollapsedStateChanged;
	bIsValid = InArgs._IsValid;
	
	// init style 
	static FButtonStyle BtnStyle;
	static FButtonStyle BottomBtnStyle;
	static FSlateBrush BorderBrush;
	static bool bIsSet = false;
	if (!bIsSet)
	{
		FSlateBrush Brush;

		BorderBrush.TintColor = FLinearColor(0.3f,0.3f,0.3f,1);

		Brush.TintColor = FLinearColor::Gray;
		BtnStyle.SetNormal(Brush);
		Brush.TintColor = FLinearColor::Gray;
		BtnStyle.SetHovered(Brush);
		Brush.TintColor = FLinearColor(0.3f,0.3f,0.3f,1);
		BtnStyle.SetPressed(Brush);

		Brush.TintColor = BorderBrush.TintColor;
		BottomBtnStyle.SetNormal(Brush);
		Brush.TintColor = BorderBrush.TintColor;
		BottomBtnStyle.SetHovered(Brush);
		Brush.TintColor = FLinearColor(0.1f,0.1f,0.1f,1);
		BottomBtnStyle.SetPressed(Brush);
		
		bIsSet = true;
	}

	// head btn
	AddSlot()
	.AutoHeight()
	[
		SAssignNew(HeadButton, SButton)
	    .ButtonStyle(&BtnStyle)
	    .OnClicked_Raw(this, &SEventItem::_OnHeadBtnClicked)
	    [
	    	SNew(SHorizontalBox)
	    	+ SHorizontalBox::Slot()
	    	.FillWidth(1)
	    	[
				SAssignNew(HeadBtnText, STextBlock)
		    	.Text(
		    		!bIsValid ?
		    		FText::FromString(TEXT("Invalid Event")) :
		    		FText::FromString(FEventBinderLibrary::AppendSignature(EventName, TargetSignature)))
		    	.ColorAndOpacity(!bIsValid ? FLinearColor::Red : FLinearColor::Black)
	    	]
	    	+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			[
				SNew(SButton)
				.ButtonStyle(&BtnStyle)
				.ToolTipText(FText::FromString(TEXT("Remove")))
				.Text(FText::FromString(TEXT(" X ")))
				.OnClicked_Lambda([&]
				{
					OnDeleteEvent.ExecuteIfBound(StaticCastSharedRef<SEventItem>(this->AsShared()));
					return FReply::Handled();
				})
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			[
				SNew(SButton)
				.ButtonStyle(&BtnStyle)
                .ToolTipText(FText::FromString(TEXT("Replace")))
                .Text(FText::FromString(TEXT(" R ")))
                .OnClicked_Lambda([&]
                {
					OnReplaceEvent.ExecuteIfBound(StaticCastSharedRef<SEventItem>(this->AsShared()));
                	return FReply::Handled();
                })
			]
	    ]
	];

	// delegate item list
	AddSlot()
    .AutoHeight()
	[
		SAssignNew(DelegateItemListArea, SBorder)
		.BorderImage(&BorderBrush)
		[
			SAssignNew(DelegateItemList, SVerticalBox)
		]
	];

	// add and delete
	AddSlot()
	.AutoHeight()
	.HAlign(HAlign_Right)
	[
		SAssignNew(BottomBar, SHorizontalBox)
	    + SHorizontalBox::Slot()
	    .AutoWidth()
	    [
	        SNew(SButton)
	        .Text(FText::FromString(TEXT("  +  ")))
	        .ButtonStyle(&BottomBtnStyle)
            .IsEnabled(bIsValid)
	        .ToolTipText(FText::FromString(TEXT("Add")))
	        .OnClicked_Raw(this, &SEventItem::_OnAddDelegate)
	    ]
	    + SHorizontalBox::Slot()
	    .AutoWidth()
	    [
	        SNew(SButton)
	        .Text(FText::FromString(TEXT("  -  ")))
            .IsEnabled(bIsValid)
	        .ButtonStyle(&BottomBtnStyle)
	        .ToolTipText(FText::FromString(TEXT("Remove")))
	        .OnClicked_Raw(this, &SEventItem::_OnDeleteDelegate)
	    ]
	];

	// close widgets
	SetIsCollapsed(InArgs._CollapsedDefault);
	
	_RefreshEventInfoWidgets();
	_RefreshDelegateItemList();
}

SEventItem::~SEventItem()
{
}

void SEventItem::SetSource(FEventBindInfo* InSource)
{
	EventBindInfoSource = InSource;
	_RefreshDelegateItemList();
}

void SEventItem::SetSignature(FFunctionSignature InSignature)
{
	TargetSignature = MoveTemp(InSignature);
	_RefreshEventInfoWidgets();
}

void SEventItem::SetIsCollapsed(bool bInIsCollapsed)
{
	bIsCollapsed = bInIsCollapsed;
	DelegateItemList->SetVisibility(bIsCollapsed ? EVisibility::Collapsed : EVisibility::Visible);
	BottomBar->SetVisibility(bIsCollapsed ? EVisibility::Collapsed : EVisibility::Visible);
	OnCollapsedStateChanged.ExecuteIfBound(StaticCastSharedRef<SEventItem>(this->AsShared()),bIsCollapsed);
}

FReply SEventItem::_OnHeadBtnClicked()
{
	SetIsCollapsed(!bIsCollapsed);
	return FReply::Handled();
}

FReply SEventItem::_OnAddDelegate()
{
	if (!EventBindInfoSource) return FReply::Handled();

	FDelegateBindInfo BindInfo;
	EventBindInfoSource->AllDelegates.Add(BindInfo);

	OnEventModify.ExecuteIfBound();
	_RefreshDelegateItemList();
	return FReply::Handled();
}

FReply SEventItem::_OnDeleteDelegate()
{
	if (!EventBindInfoSource || !SelectedItem.IsValid()) return FReply::Handled();

	FDelegateBindInfo BindInfo;
	BindInfo.TargetActor = SelectedItem->GetBindActor();
	BindInfo.BindFunction = SelectedItem->GetBindFunction();
	SelectedItem.Reset();
	
	EventBindInfoSource->AllDelegates.Remove(BindInfo);
	OnEventModify.ExecuteIfBound();
	
	_RefreshDelegateItemList();
	return FReply::Handled();
}

void SEventItem::_RefreshEventInfoWidgets()
{
	HeadBtnText->SetText(
		!bIsValid ?
        FText::FromString(TEXT("Invalid Event")) :
        FText::FromString(FEventBinderLibrary::AppendSignature(EventName, TargetSignature)));
}

void SEventItem::_RefreshDelegateItemList()
{
	DelegateItemList->ClearChildren();
	if (!EventBindInfoSource) return;
	if (!bIsValid) return;
	
	for (auto & Delegate : EventBindInfoSource->AllDelegates)
	{
		_AddDelegateItem(Delegate);
	}
}

TSharedRef<SWidget> SEventItem::_AddDelegateItem(FDelegateBindInfo& InBindInfo)
{
	TSharedPtr<SDelegateItem> BindItem;
	DelegateItemList->AddSlot()
    .AutoHeight()
	.Padding(0)
    [
        SAssignNew(BindItem, SDelegateItem)
        .TargetSignature(TargetSignature)
        .DefaultActor(InBindInfo.TargetActor)
        .DefaultFunction(InBindInfo.BindFunction)
        .OnBindActorChanged_Lambda([&](AActor* InActor){ InBindInfo.TargetActor = InActor; OnEventModify.ExecuteIfBound(); })
        .OnBindFunctionChanged_Lambda([&](FName InFuc) { InBindInfo.BindFunction = InFuc; OnEventModify.ExecuteIfBound(); })
        .OnItemSelected_Lambda([&](TSharedRef<SDelegateItem> InItem) { SelectedItem = InItem; })
    ];
	BindItem->IsSelected.Bind(TAttribute<bool>::FGetter::CreateLambda([BindItem, this]{ return BindItem == SelectedItem; }));
	return BindItem->AsShared();
}

