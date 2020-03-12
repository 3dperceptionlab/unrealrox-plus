// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class unrealrox_plus_makerTarget : TargetRules
{
	public unrealrox_plus_makerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "unrealrox_plus_maker", "UnrealROX_Plus" } );
	}
}
