// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class hwanyoung2Target : TargetRules
{
	public hwanyoung2Target(TargetInfo Target) : base(Target)
	{
        WindowsPlatform.Compiler = WindowsCompiler.VisualStudio2022;
        WindowsPlatform.CompilerVersion = "14.38.33130"; // 설치된 정확한 버전 번호로 바꿔주세요

        Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("hwanyoung2");
	}
}
