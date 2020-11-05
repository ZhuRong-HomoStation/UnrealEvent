#pragma once
#include "CoreMinimal.h"
#include "EventBinder.h"

class UNREALEVENT_API FEventBinderLibrary
{
public:
	static FFunctionSignature GetFunctionSignature(UFunction* InFunction);
	static FString AppendSignature(FName InName, const FFunctionSignature& InSignature);
};
