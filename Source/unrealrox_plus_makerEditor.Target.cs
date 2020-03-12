// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class unrealrox_plus_makerEditorTarget : TargetRules
{
	public unrealrox_plus_makerEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "unrealrox_plus_maker", "UnrealROX_Plus" } );
	}
}
