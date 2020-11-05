#pragma once
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "ActorPickerMode.h"

class SPickActor : public SCompoundWidget
{
public:	
    SLATE_BEGIN_ARGS(SPickActor)
	    : _DefaultActor(nullptr)
	{}
		SLATE_EVENT(FOnActorSelected, OnActorSelected)
		SLATE_EVENT(FOnShouldFilterActor, OnShouldPickActor)
		SLATE_ARGUMENT(FOnGetAllowedClasses, OnGetAllowedClasses)
		SLATE_ARGUMENT(AActor*, DefaultActor)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~SPickActor();
	
	void SetPickedActor(AActor* InActor);
	AActor* GetPickedActor() const { return PickedActor.Get(); }
private:
	FText 	_OnGetActorName() const;
	FReply 	_OnOpenSelectActorWindow();
	FReply 	_OnPickActor();
private:
	// config 
	TWeakObjectPtr<AActor>	PickedActor;

	// delegate 
	FOnActorSelected		OnActorSelected;
	FOnShouldFilterActor	OnShouldPickActor;
	FOnGetAllowedClasses	OnGetAllowedClasses;
};
