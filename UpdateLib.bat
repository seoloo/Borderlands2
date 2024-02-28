

//// xcopy	.복사할 파일이 들어있는 경로			.복사받을 경로.



xcopy	/y/s .\Engine\Public\*.*						.\Reference\Headers\
xcopy	/y .\Engine\Bin\*.lib							.\Reference\Librarys\
xcopy	/y .\Engine\Bin\*.dll							.\Client\Bin\
xcopy	/y .\Engine\Bin\*.dll							.\Tool\Bin\
xcopy	/y .\Engine\Bin\*.dll							.\EffectTool\Bin\
xcopy	/y .\Engine\Bin\ShaderFiles\*.*					.\Client\Bin\ShaderFiles\
xcopy	/y .\Engine\Bin\ShaderFiles\*.*					.\Tool\Bin\ShaderFiles\
xcopy	/y .\Engine\Bin\ShaderFiles\*.*					.\EffectTool\Bin\ShaderFiles\