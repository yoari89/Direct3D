cd %~dp0

Rem : (��ġ������ �ִ� ���丮 ��ġ)�� ������ _DXMath ������ ���ٸ� ���� �� �����Ѵ�.
xcopy DXEngine\Inc\*.h   _DXEngine\Inc\	/s /y /d /exclude:exclude.txt
xcopy DXEngine\Inc\*.inl _DXEngine\Inc\	/s /y /d /exclude:exclude.txt
xcopy DXEngine\Inc\*.hpp _DXEngine\Inc\	/s /y /d /exclude:exclude.txt

Rem : �ٸ� ������Ʈ���� Engine Library�� ����� �� �ֵ��� �����Ѵ�. (Debug, Release)
xcopy DXEngine\Bin\*.*	  DXClient3D\Bin\	/s /y /d /exclude:exclude.txt

Rem : FMOD (Debug, Release)
xcopy ThirdParty\fmod.dll		DXClient3D\Bin\Debug\		/s /y /d /exclude:exclude.txt
xcopy ThirdParty\fmod_vc.lib	DXClient3D\Bin\Debug\		/s /y /d /exclude:exclude.txt
xcopy ThirdParty\fmod64.dll		DXClient3D\Bin\Debug\		/s /y /d /exclude:exclude.txt
xcopy ThirdParty\fmod64_vc.lib	DXClient3D\Bin\Debug\		/s /y /d /exclude:exclude.txt
xcopy ThirdParty\fmod.dll		DXClient3D\Bin\Release\		/s /y /d /exclude:exclude.txt
xcopy ThirdParty\fmod_vc.lib	DXClient3D\Bin\Release\		/s /y /d /exclude:exclude.txt
xcopy ThirdParty\fmod64.dll		DXClient3D\Bin\Release\		/s /y /d /exclude:exclude.txt
xcopy ThirdParty\fmod64_vc.lib	DXClient3D\Bin\Release\		/s /y /d /exclude:exclude.txt