#include "EventBinderWidgets/SEventBinderAction.h"
#include "EventBinderLibrary.h"
#include "EventBinderWidgets/SEventItem.h"
#include "Logging.h"
#include "UtilWidgets/SPickEvent.h"

TMap<TWeakObjectPtr<UObject>, TMap<FName,bool>> SEventBinderAction::WinStateMap;

void SEventBinderAction::Construct(const FArguments& InArgs)
{
	if (InArgs._TargetActor == nullptr)
	{
		UE_LOG(LogUnrealEventEditor, Error, TEXT("%s: Pass null targetActor"), TEXT(__FUNCTION__));
		return;
	}
	if (InArgs._TargetActor->GetLevel() == nullptr)
	{
		UE_LOG(LogUnrealEventEditor, Error, TEXT("%s: targetActor %s lost level"), TEXT(__FUNCTION__), InArgs._TargetActor->GetName());
		return;
	}
	TargetActor = InArgs._TargetActor;
	TargetBinder = TargetActor->GetLevel()->GetAssetUserData<UEventBinderAssetUserData>();
	if (!TargetBinder)
	{
		TargetBinder = NewObject<UEventBinderAssetUserData>(TargetActor->GetLevel());
		TargetActor->GetLevel()->AddAssetUserData(TargetBinder);
	}
	
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
	auto& BindInfo = _GetTargetBinder().ActorBindMap.FindOrAdd(NAME_None).EventBindMap.Add(EventName);
	BindInfo.EventSignature = DelegateMap[EventName];
	_MarkOuterDirty();
	RebuildEventList();
}

void SEventBinderAction::_OnDeleteEvent(TSharedRef<SEventItem> InItem)
{
	_GetTargetBinder().ActorBindMap.FindOrAdd(NAME_None).EventBindMap.Remove(InItem->GetEventName());
	_TryRemoveTargetBinder();
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
        FEventBindInfo Info = MoveTemp(_GetTargetBinder().ActorBindMap.FindOrAdd(NAME_None).EventBindMap[InItem->GetEventName()]);
        _GetTargetBinder().ActorBindMap.FindOrAdd(NAME_None).EventBindMap.Remove(InItem->GetEventName());
        _GetTargetBinder().ActorBindMap.FindOrAdd(NAME_None).EventBindMap.Add(InName, MoveTemp(Info));
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
	TargetActor->MarkPackageDirty();
}

void SEventBinderAction::RebuildEventList()
{
	if (!TargetBinder->AllBindMap.Find(TargetActor)) return;
	EventItemList->ClearChildren();
	for (auto & BindItem : _GetTargetBinder().ActorBindMap.FindOrAdd(NAME_None).EventBindMap)
	{
		auto Item = DelegateMap.Find(BindItem.Key);
		auto& WinActiveMap = WinStateMap.FindOrAdd(TargetActor);
		
		EventItemList->AddSlot()
		.AutoHeight()
	    .Padding(0,10,0,0)
		[
			SNew(SEventItem)
			.EventName(BindItem.Key)
			.Source(&BindItem.Value)
			.TargetSignature(BindItem.Value.EventSignature)
			.IsValid(Item != nullptr)
			.Owner(Cast<AActor>(TargetActor))
			.CollapsedDefault(WinActiveMap.FindOrAdd(BindItem.Key))
			.OnEventModify_Raw(this, &SEventBinderAction::_MarkOuterDirty)
			.OnDeleteEvent_Raw(this, &SEventBinderAction::_OnDeleteEvent)
			.OnReplaceEvent_Raw(this, &SEventBinderAction::_OnReplaceEvent)
			.OnCollapsedStateChanged_Lambda([&](TSharedRef<SEventItem> Item, bool bIsCollapsed)
			{
				WinStateMap.FindOrAdd(TargetActor).FindOrAdd(Item->GetEventName()) = bIsCollapsed;
			})
		];
	}
}

FEventBinderActor& SEventBinderAction::_GetTargetBinder()
{
	return TargetBinder->AllBindMap.FindOrAdd(TargetActor);
}

void SEventBinderAction::_TryRemoveTargetBinder()
{
	TargetBinder->RemoveInvalidItem();
}

void SEventBinderAction::_CollectDelegates()
{
	DelegateMap.Reset();
	// each delegates 
	for(TFieldIterator<FMulticastDelegateProperty> DelegateIt(TargetActor->GetClass()); DelegateIt; ++DelegateIt)
	{
		FName DelegateName = FName(DelegateIt->GetName());
		DelegateMap.Add(DelegateName, FEventBinderLibrary::GetFunctionSignature(DelegateIt->SignatureFunction));
	}
}

TSharedRef<SWidget> SEventBinderAction::_GetAddEventMenuContent()
{
	return SNew(SPickEvent)
	.Source(&DelegateMap)
	.GetShouldPickEvent_Lambda([&](FName InEventName) { return !_GetTargetBinder().ActorBindMap.FindOrAdd(NAME_None).EventBindMap.Find(InEventName); })
	.OnPickEvent_Raw(this, &SEventBinderAction::_OnAddEvent);
}


