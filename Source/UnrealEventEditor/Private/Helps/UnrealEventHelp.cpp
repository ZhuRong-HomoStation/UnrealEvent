#include "Helps/UnrealEventHelp.h"

bool UnrealEventHelp::CanPassFuzzySearch(const FString& InStr, const FString& InSearchStr)
{
	// split
	TArray<FString> Words;
	InSearchStr.ParseIntoArray(Words, TEXT(" "));

	// search
	bool bPassSearch = true;
	for (FString& Word : Words)
	{
		Word.RemoveSpacesInline();
		if (InStr.Find(Word) == INDEX_NONE)
		{
			bPassSearch = false;
			break;
		}
	}
	return bPassSearch;
}
