#pragma once
#include "CoreMinimal.h"
#include "EventBinder.h"
#include "EventBinderComponent.generated.h"

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class UNREALEVENT_API UEventBinderComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	// ~Begin UActorComponent API
	virtual void BeginPlay() override;
	// ~End UActorComponent API
public:
	UEventBinderComponent();

public:
	UPROPERTY(EditAnywhere)
	FEventBinder	Binder;
};

