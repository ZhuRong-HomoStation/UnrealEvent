#include "EventBinderWidgets/SEventBinderAction.h"
#include "EventBinderLibrary.h"
#include "EventBinderWidgets/SEventItem.h"
#include "UtilWidgets/SPickEvent.h"

TMap<TWeakObjectPtr<UObject>, TMap<FName,bool>> SEventBinderAction::WinStateMap;

void SEventBinderAction::Construct(const FArguments& InArgs)
{
	TargetBinder = InArgs._TargetBinder;
	Outer = InArgs._Outer;
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(EventItemList, SVerticalBox)
		]
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.Padding(20,5,20,0)
		[
			SAssignNew(AddEventComboBtn, SComboButton)
			.HasDownArrow(false)
			.OnGetMenuContent_Raw(this, &SEventBinderAction::_GetAddEventMenuContent)
			.ButtonContent()
			[
				SNew(SBox)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Add Event")))
				]
			]
		]
	];
	
	_CollectDelegates();
	RebuildEventList();
}

SEventBinderAction::~SEventBinderAction()
{
	TArray<TWeakObjectPtr<UObject>> KeyArray;
	WinStateMap.GetKeys(KeyArray);
	for (auto& Obj : KeyArray)
	{
		if (Obj.Get() == nullptr)
		{
			WinStateMap.Remove(Obj);
		}
	}
}

void SEventBinderAction::_OnAddEvent(FName EventName)
{
	AddEventComboBtn->SetIsOpen(false);
	auto& BindInfo = TargetBinder->EventBindMap.Add(EventName);
	BindInfo.EventSignature = DelegateMap[EventName];
	_MarkOuterDirty();
	RebuildEventList();
}

void SEventBinderAction::_OnDeleteEvent(TSharedRef<SEventItem> InItem)
{
	TargetBinder->EventBindMap.Remove(InItem->GetEventName());
	_MarkOuterDirty();
	RebuildEventList();
}

void SEventBinderAction::_OnReplaceEvent(TSharedRef<SEventItem> InItem)
{
	TSharedPtr<SPickEvent> PickMenu;
	TSharedPtr<SWindow> Win;

	SAssignNew(PickMenu, SPickEvent)
	.Source(&DelegateMap)
    .GetShouldPickEvent_Lambda([&](FName InName)
    {
        return DelegateMap[InName] == InItem->GetSignature();
    })
    .OnPickEvent_Lambda([&](FName InName)
    {
        FEventBindInfo Info = MoveTemp(TargetBinder->EventBindMap[InItem->GetEventName()]);
        TargetBinder->EventBindMap.Remove(InItem->GetEventName());
        TargetBinder->EventBindMap.Add(InName, MoveTemp(Info));
		_MarkOuterDirty();
    	RebuildEventList();
        FSlateApplication::Get().DestroyWindowImmediately(Win.ToSharedRef());
    });
	
	SAssignNew(Win, SWindow)
    .Title(FText::FromString(TEXT("PickEvent")))
    .SizingRule(ESizingRule::Autosized)
    .SupportsMaximize(false)
    .SupportsMinimize(false)
	[
		PickMenu.ToSharedRef()
	];
	
	FSlateApplication::Get().AddModalWindow(
        Win.ToSharedRef(),
        FGlobalTabmanager::Get()->GetRootWindow());
}

void SEventBinderAction::_MarkOuterDirty()
{
	Outer->MarkPackageDirty();
}

void SEventBinderAction::RebuildEventList()
{
	EventItemList->ClearChildren();
	for (auto & BindItem : TargetBinder->EventBindMap)
	{
		auto Item = DelegateMap.Find(BindItem.Key);
		auto& WinActiveMap = WinStateMap.FindOrAdd(Outer);
		
		EventItemList->AddSlot()
		.AutoHeight()
	    .Padding(0,10,0,0)
		[
			SNew(SEventItem)
			.EventName(BindItem.Key)
			.Source(&BindItem.Value)
			.TargetSignature(BindItem.Value.EventSignature)
			.IsValid(Item != nullptr)
			.CollapsedDefault(WinActiveMap.FindOrAdd(BindItem.Key))
			.OnEventModify_Raw(this, &SEventBinderAction::_MarkOuterDirty)
			.OnDeleteEvent_Raw(this, &SEventBinderAction::_OnDeleteEvent)
			.OnReplaceEvent_Raw(this, &SEventBinderAction::_OnReplaceEvent)
			.OnCollapsedStateChanged_Lambda([&](TSharedRef<SEventItem> Item, bool bIsCollapsed)
			{
				WinStateMap.FindOrAdd(Outer).FindOrAdd(Item->GetEventName()) = bIsCollapsed;
			})
		];
	}
}

void SEventBinderAction::_CollectDelegates()
{
	DelegateMap.Reset();
	// each delegates 
	for(TFieldIterator<FMulticastDelegateProperty> DelegateIt(Outer->GetClass()); DelegateIt; ++DelegateIt)
	{
		FName DelegateName = FName(DelegateIt->GetName());
		DelegateMap.Add(DelegateName, FEventBinderLibrary::GetFunctionSignature(DelegateIt->SignatureFunction));
	}
}

TSharedRef<SWidget> SEventBinderAction::_GetAddEventMenuContent()
{
	return SNew(SPickEvent)
	.Source(&DelegateMap)
	.GetShouldPickEvent_Lambda([&](FName InEventName) { return !TargetBinder->EventBindMap.Find(InEventName); })
	.OnPickEvent_Raw(this, &SEventBinderAction::_OnAddEvent);
}


