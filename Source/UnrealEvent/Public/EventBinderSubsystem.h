#pragma once
#include "CoreMinimal.h"
#include "EventBinder.h"
#include "EventBinderSubsystem.generated.h"

USTRUCT()
struct FEventBinderLostReferenceItem
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FDelegateBindInfo	BindInfo;

	UPROPERTY()
	FName				EventName;

	UPROPERTY()
	AActor*				TargetActor;
	
	UPROPERTY()
	FName				ComponentName;
};

UCLASS()
class UEventBinderSubsystemWorld : public UWorldSubsystem
{
	GENERATED_BODY()
private:
	bool _ApplyBindInfo(const FDelegateBindInfo& InBindInfo, FName InEventName, AActor* TargetActor, FName InCompName);
	void _ApplyLevelBindInfo(ULevel* InLevel);
	void _TryApplyItemsLostReference();
public:
	// ~Begin USubsystem API
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~End USubsystem API

private:
	FDelegateHandle		OnLevelAddToWorldHandle;

	UPROPERTY()
	TArray<FEventBinderLostReferenceItem>	LostReferenceItems;
};


