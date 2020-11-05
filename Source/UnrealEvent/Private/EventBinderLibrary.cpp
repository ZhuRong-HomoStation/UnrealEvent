#include "EventBinderLibrary.h"

FFunctionSignature FEventBinderLibrary::GetFunctionSignature(UFunction* InFunction)
{
	check(InFunction);
	FFunctionSignature Ret;

	// collect params 
	for (TFieldIterator<FProperty> It(InFunction); It; ++It)
	{
		FParamSignature Param;

		FString Extended;
		Param.ParamType = FName(It->GetCPPType(&Extended));
		Param.ExtendedType = FName(MoveTemp(Extended));
		Param.bIsReference = (bool)(It->PropertyFlags & CPF_ReferenceParm);
		Param.bIsOut = (bool)(It->PropertyFlags & CPF_OutParm);
		Param.bIsReturn = (bool)(It->PropertyFlags & CPF_ReturnParm);
		Param.Offset = It->GetOffset_ForUFunction();
		Param.Size = It->GetSize();

		Ret.ParamTypes.Emplace(MoveTemp(Param));
	}

	return Ret;
}

FString FEventBinderLibrary::AppendSignature(FName InName, const FFunctionSignature& InSignature)
{
	FString RetName = TEXT("void");
	FString FuncNameWitSignature = InName == NAME_None ? TEXT("") : InName.ToString();
	FuncNameWitSignature.Append(TEXT("("));
	for (int32 i = 0; i < InSignature.ParamTypes.Num(); ++i)
	{
		const auto& ParamSignature = InSignature.ParamTypes[i];

		if (i != InSignature.ParamTypes.Num() - 1)
		{
			FuncNameWitSignature.Append(ParamSignature.ParamType.ToString());

			if (ParamSignature.bIsReference)
			{
				FuncNameWitSignature.Append(TEXT("&"));
			}
			
			if (ParamSignature.bIsReturn)
			{
				// return type always last one 
				checkNoEntry();
			}

			if (!InSignature.ParamTypes[i + 1].bIsReturn)
			{
				FuncNameWitSignature.Append(TEXT(", "));
			}
		}
		else
		{
			if (ParamSignature.bIsReturn)
			{
				RetName = ParamSignature.ParamType.ToString();
			}
			else
			{
				FuncNameWitSignature.Append(ParamSignature.ParamType.ToString());

				if (ParamSignature.bIsReference)
				{
					FuncNameWitSignature.Append(TEXT("&"));
				}
			}
		}
	}
	FuncNameWitSignature.Append(TEXT(")"));
	RetName.Append(TEXT(" "));
	RetName.Append(FuncNameWitSignature);
	return RetName;
}
