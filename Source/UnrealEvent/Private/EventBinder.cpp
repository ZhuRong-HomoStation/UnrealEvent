#include "EventBinder.h"

void FEventBinder::BindToActor(AActor* InActor)
{
	for (auto & BindMap : EventBindMap)
	{
		FMulticastDelegateProperty* Property = static_cast<FMulticastDelegateProperty*>(
            InActor->GetClass()->FindPropertyByName(BindMap.Key));

		for (auto& Delegate : BindMap.Value.AllDelegates)
		{
			if (Delegate.BindFunction == NAME_None || !Delegate.TargetActor) continue;
			
			FScriptDelegate BindDelegate;
			BindDelegate.BindUFunction(Delegate.TargetActor, Delegate.BindFunction);
			Property->AddDelegate(BindDelegate, InActor);
		}
	}
}
