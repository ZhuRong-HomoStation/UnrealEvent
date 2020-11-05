#include "EventBinderComponent.h"

void UEventBinderComponent::BeginPlay()
{
	Super::BeginPlay();
	Binder.BindToActor(GetOwner());
}

UEventBinderComponent::UEventBinderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
