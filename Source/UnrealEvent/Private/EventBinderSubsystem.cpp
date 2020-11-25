#include "EventBinderSubsystem.h"
#include "EventBinder.h"
#include "Engine/LevelStreaming.h"

bool UEventBinderSubsystemWorld::_ApplyBindInfo(const FDelegateBindInfo& InBindInfo, FName InEventName, AActor* TargetActor,
												FName InCompName)
{
	AActor* BindTarget = InBindInfo.TargetActor.Get();
	if (BindTarget == nullptr || BindTarget->GetWorld() != GetWorld()) return false;
	FMulticastDelegateProperty* DelegateProperty = CastField<FMulticastDelegateProperty>(
		TargetActor->GetClass()->FindPropertyByName(InEventName));

	FScriptDelegate NewDelegate;
	NewDelegate.BindUFunction(BindTarget, InBindInfo.BindFunction);
	DelegateProperty->AddDelegate(NewDelegate, TargetActor);
	
	return true;
}

void UEventBinderSubsystemWorld::_ApplyLevelBindInfo(ULevel* InLevel)
{
	UEventBinderAssetUserData* Data = InLevel->GetAssetUserData<UEventBinderAssetUserData>();
	if (!Data) return;

	// apply bind data  
	for (auto & ItActorBind : Data->AllBindMap)
	{
		AActor* TargetActor = ItActorBind.Key;
		if (!TargetActor) continue;
		for (auto & ItCompBind : ItActorBind.Value.ActorBindMap)
		{
			FName TargetComponent = ItCompBind.Key;
			for (auto& ItEvent : ItCompBind.Value.EventBindMap)
			{
				FName EventName = ItEvent.Key;
				for (auto & Delegate : ItEvent.Value.AllDelegates)
				{
					if (!_ApplyBindInfo(Delegate, EventName, TargetActor, TargetComponent))
					{
						FEventBinderLostReferenceItem NewItem;
						NewItem.BindInfo = Delegate;
						NewItem.ComponentName = TargetComponent;
						NewItem.TargetActor = TargetActor;
						NewItem.EventName = EventName;
						LostReferenceItems.Add(NewItem);
					}
				}
			}
		}
	}
}

void UEventBinderSubsystemWorld::_TryApplyItemsLostReference()
{
	for (int32 i = 0; i < LostReferenceItems.Num();)
	{
		if (LostReferenceItems[i].TargetActor == nullptr)
		{
			// lost reference 
			LostReferenceItems.RemoveAtSwap(i);
		}
		else if (_ApplyBindInfo(
				LostReferenceItems[i].BindInfo,
				LostReferenceItems[i].EventName,
				LostReferenceItems[i].TargetActor,
				LostReferenceItems[i].ComponentName))
		{
			LostReferenceItems.RemoveAtSwap(i);
		}
		else
		{
			++i;
		}
	}
}

void UEventBinderSubsystemWorld::_ResearchItemsLostReference()
{
	UWorld* World = GetWorld();
	// collect levels 
	TArray<ULevel*> AllLevels;
	TArray<ULevelStreaming*> AllStreamingLevels = World->GetStreamingLevels();
	for (ULevelStreaming* Level : AllStreamingLevels)
	{
		if (Level->GetLoadedLevel() && Level->GetLoadedLevel()->OwningWorld == World)
		{
			AllLevels.Add(Level->GetLoadedLevel());
		}
	}
	AllLevels.Add(World->PersistentLevel);

	LostReferenceItems.Reset();
	for (ULevel* Level : AllLevels)
	{
		UEventBinderAssetUserData* Data = Level->GetAssetUserData<UEventBinderAssetUserData>();
		if (!Data) return;

		// apply bind data  
		for (auto & ItActorBind : Data->AllBindMap)
		{
			AActor* TargetActor = ItActorBind.Key;
			if (!TargetActor) continue;
			for (auto & ItCompBind : ItActorBind.Value.ActorBindMap)
			{
				FName TargetComponent = ItCompBind.Key;
				for (auto& ItEvent : ItCompBind.Value.EventBindMap)
				{
					FName EventName = ItEvent.Key;
					for (auto & Delegate : ItEvent.Value.AllDelegates)
					{
						AActor* BindTarget = Delegate.TargetActor.Get();
						if (BindTarget == nullptr || BindTarget->GetLevel()->OwningWorld != World)
						{
							FEventBinderLostReferenceItem NewItem;
							NewItem.BindInfo = Delegate;
							NewItem.ComponentName = TargetComponent;
							NewItem.TargetActor = TargetActor;
							NewItem.EventName = EventName;
							LostReferenceItems.Add(NewItem);
						}
					}
				}
			}
		}
	}
	
}

void UEventBinderSubsystemWorld::Initialize(FSubsystemCollectionBase& Collection)
{
	UWorld* World = GetWorld();
	// filter invalid world 
	if (World->WorldType != EWorldType::Game &&
		World->WorldType != EWorldType::PIE)
	{
		return;
	}

	// listen level add to world for bind actor that in other level 
	OnLevelAddToWorldHandle = FWorldDelegates::LevelAddedToWorld.AddLambda([&](ULevel* InLevel, UWorld* InWorld)
	{
		if (InWorld != GetWorld()) return;
		// try to apply items that lost reference
		_TryApplyItemsLostReference();

		// apply level bind info 
		_ApplyLevelBindInfo(InLevel);
	});

	// listen level removed from world for resume lost reference item
	OnLevelRemoveFromWorldHandle = FWorldDelegates::LevelRemovedFromWorld.AddLambda([&](ULevel* InLevel, UWorld* InWorld)
	{
		if (InWorld != GetWorld()) return;
		_ResearchItemsLostReference();
	});

	// collect levels 
	TArray<ULevel*> AllLevels;
	TArray<ULevelStreaming*> AllStreamingLevels = World->GetStreamingLevels();
	for (ULevelStreaming* Level : AllStreamingLevels)
	{
		AllLevels.Add(Level->GetLoadedLevel());
	}
	AllLevels.Add(World->PersistentLevel);

	// apply 
	for (ULevel* Level : AllLevels)
	{
		if (Level)
		{
			_ApplyLevelBindInfo(Level);
		}
	}
}

void UEventBinderSubsystemWorld::Deinitialize()
{
	UWorld* World = GetWorld();
	// filter invalid world 
	if (World->WorldType != EWorldType::Game &&
        World->WorldType != EWorldType::PIE)
	{
		return;
	}
	
	FWorldDelegates::LevelAddedToWorld.Remove(OnLevelAddToWorldHandle);
	FWorldDelegates::LevelRemovedFromWorld.Remove(OnLevelRemoveFromWorldHandle);
}
