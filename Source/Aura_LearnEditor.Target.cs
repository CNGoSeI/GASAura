// 学习使用

using UnrealBuildTool;
using System.Collections.Generic;

public class Aura_LearnEditorTarget : TargetRules
{
	public Aura_LearnEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "Aura_Learn" } );
	}
}
