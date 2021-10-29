// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"
#include "Misc/FileHelper.h"
#include "DateTime.h"

/**
 * 
 */
class UNREALROX_PLUS_API ROXTaskUtils
{
public:
	ROXTaskUtils();
	~ROXTaskUtils();
};


/*****************************************************************************
* FROXWriteStringTask: 
* This class allows to write a string to a file in a different execution
* thread, not blocking the game execution. It should be used like this:
* (new FAutoDeleteAsyncTask<FWriteStringTask>(string, absolute_file_path))->StartBackgroundTask();
******************************************************************************/
class FROXWriteStringTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FROXWriteStringTask>;

public:
	FROXWriteStringTask(FString str, FString absoluteFilePath) :
		m_str(str),
		m_absolute_file_path(absoluteFilePath)
	{}

protected:
	FString m_str;
	FString m_absolute_file_path;

	void DoWork()
	{
		// Place the Async Code here.  This function runs automatically.
		// Text File
		FFileHelper::SaveStringToFile(m_str, *m_absolute_file_path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
		/*IFileManager* FileManager = &IFileManager::Get();
		auto Ar = TUniquePtr<FArchive>(FileManager->CreateFileWriter(*m_absolute_file_path, EFileWrite::FILEWRITE_Append));
		if (Ar && !m_str.IsEmpty())
		{
			const TCHAR* StrPtr = *m_str;
			FTCHARToUTF8 UTF8String(StrPtr);
			Ar->Serialize((UTF8CHAR*)UTF8String.Get(), UTF8String.Length() * sizeof(UTF8CHAR));
		}*/
	}

	// This next section of code needs to be here.  Not important as to why.
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FWriteStringTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};


/*****************************************************************************
* FROXScreenshotTask:
* This class allows to write an image to a file in a different execution
* thread, not blocking the game execution. It should be used like this:
* (new FAutoDeleteAsyncTask<FROXScreenshotTask>(image_data, absolute_file_path))->StartBackgroundTask();
******************************************************************************/
class FROXScreenshotTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FROXScreenshotTask>;

public:
	FROXScreenshotTask(TArray<uint8> bitarray, FString absoluteFilePath) :
		m_bitarray(bitarray),
		m_absolute_file_path(absoluteFilePath)
	{}

protected:
	TArray<uint8> m_bitarray;
	FString m_absolute_file_path;

	void DoWork()
	{
		// Text File
		FFileHelper::SaveArrayToFile(m_bitarray, *m_absolute_file_path);
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FScreenshotTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};