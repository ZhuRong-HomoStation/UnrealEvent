#pragma once
#include "CoreMinimal.h"
#include "EventBinder.generated.h"

// param signature 
USTRUCT(BlueprintType)
struct UNREALEVENT_API FParamSignature
{
	GENERATED_BODY()
public:
	// param type 
	UPROPERTY(VisibleAnywhere)
	FName		ParamType;
	// extended type(if template, e.g ParamType is TArray and ExtendedType is <int>)
	UPROPERTY(VisibleAnywhere)
	FName		ExtendedType;
	// is param passed by reference 
	UPROPERTY(VisibleAnywhere)
	bool		bIsReference;
	// is out value, value is copied out after function call, like cpp return and pass by reference value 
	UPROPERTY(VisibleAnywhere)
	bool		bIsOut;
	// is return value, only cpp function, blueprint will instead by OutParam 
	UPROPERTY(VisibleAnywhere)
	bool		bIsReturn;
	// param offset in function stack 
	UPROPERTY(VisibleAnywhere)
	int32		Offset;
	// param size in function stack 
	UPROPERTY(VisibleAnywhere)
	int32		Size;

	bool operator==(const FParamSignature& Rhs) const 
	{
		return 	ParamType == Rhs.ParamType &&
				ExtendedType == Rhs.ExtendedType &&
				bIsReference == Rhs.bIsReference &&
				bIsOut == Rhs.bIsOut;
	}
	bool operator!=(const FParamSignature& Rhs) const { return !(*this == Rhs); }
};

// function signature
USTRUCT(BlueprintType)
struct UNREALEVENT_API FFunctionSignature
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	TArray<FParamSignature>		ParamTypes;

	bool operator==(const FFunctionSignature& Rhs) const
	{
		if (ParamTypes.Num() != Rhs.ParamTypes.Num()) return false;
		for (int32 i = 0; i < ParamTypes.Num(); ++i)
		{
			if (ParamTypes[i] != Rhs.ParamTypes[i]) return false;
		}
		return true;
	}
	bool operator!=(const FFunctionSignature& Rhs) const { return !(*this == Rhs); }
};

// delegate bind info(single) 
USTRUCT(BlueprintType)
struct UNREALEVENT_API FDelegateBindInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere)	
	FName		BindFunction;
	UPROPERTY(VisibleAnywhere)
	AActor*		TargetActor = nullptr;

	bool operator==(const FDelegateBindInfo& Rhs) const
	{
		return  BindFunction == Rhs.BindFunction && TargetActor == Rhs.TargetActor;
	}
	bool operator!=(const FDelegateBindInfo& Rhs) const { return !(*this == Rhs); }
};

// delegate bind info(multicast) 
USTRUCT(BlueprintType)
struct UNREALEVENT_API FEventBindInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	TArray<FDelegateBindInfo>	AllDelegates;
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere)
	FFunctionSignature			EventSignature;	
#endif 
};

USTRUCT(BlueprintType)
struct UNREALEVENT_API FEventBinder
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	TMap<FName, FEventBindInfo>	EventBindMap;

	void BindToActor(AActor* InActor);
};
