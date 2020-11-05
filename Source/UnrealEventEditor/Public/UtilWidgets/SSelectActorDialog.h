#pragma once 
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "ActorPickerMode.h"

class SSelectActorDialog : public SWindow
{
public:
	SLATE_BEGIN_ARGS(SSelectActorDialog) {}
		SLATE_ATTRIBUTE(TSubclassOf<AActor>, ActorClass)
		SLATE_ATTRIBUTE(AActor*, InitActor)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~SSelectActorDialog();
	
	AActor* ShowModal();

	AActor* GetCurrentActor() const { return CurrentActor; }
	void SetCurrentActor(AActor* InActor) { CurrentActor = InActor; }
private:
	void _HandleUseSelected();
	void _OnEdit();
	void _OnCopy();
	void _OnPaste();
	bool _CanPaste();
	void _OnClear();
	void _OnActorSelected(AActor* InActor);
private:
	AActor* CurrentActor;
	FOnShouldFilterActor ActorFilter;
};
