cd %~dp0

Rem : (배치파일이 있는 디렉토리 위치)에 파일을 _DXMath 폴더가 없다면 생성 후 복사한다.
xcopy DXEngine\Inc\*.h   _DXEngine\Inc\	/s /y /d /exclude:exclude.txt
xcopy DXEngine\Inc\*.inl _DXEngine\Inc\	/s /y /d /exclude:exclude.txt
xcopy DXEngine\Inc\*.hpp _DXEngine\Inc\	/s /y /d /exclude:exclude.txt

Rem : 다른 프로젝트에서 Engine Library를 사용할 수 있도록 설정한다. (Debug, Release)
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