#include "EventBinder.h"

void FEventBinder::BindToObject(UObject* InObject)
{
	for (auto & BindMap : EventBindMap)
	{
		FMulticastDelegateProperty* Property = static_cast<FMulticastDelegateProperty*>(
            InObject->GetClass()->FindPropertyByName(BindMap.Key));

		for (auto& Delegate : BindMap.Value.AllDelegates)
		{
			if (Delegate.BindFunction == NAME_None || !Delegate.TargetActor) continue;
			
			FScriptDelegate BindDelegate;
			BindDelegate.BindUFunction(Delegate.TargetActor.Get(), Delegate.BindFunction);
			Property->AddDelegate(BindDelegate, InObject);
		}
	}
}

void UEventBinderAssetUserData::RemoveInvalidItem()
{
	TArray<AActor*> ItemToRemove;
	for (auto& BindMap : AllBindMap)
	{
		if (BindMap.Key == nullptr)
		{
			ItemToRemove.Add(BindMap.Key);
			continue;
		}
		if (BindMap.Value.IsEmpty())
		{
			ItemToRemove.Add(BindMap.Key);
		}
	}

	for (auto & Key : ItemToRemove)
	{
		AllBindMap.Remove(Key);
	}
}
