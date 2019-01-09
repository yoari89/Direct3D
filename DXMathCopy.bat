cd %~dp0

Rem : (배치파일이 있는 디렉토리 위치)에 헤더파일을 _DXMath 폴더가 없다면 생성 후 복사한다.
xcopy DXMath\Include\*.h	_DXMath\Include\	/s /y /d	/exclude:exclude.txt

Rem : 다른 프로젝트에서 Math Library를 사용할 수 있도록 설정한다. (Debug, Release)
xcopy DXMath\Bin\*.*	DXEngine\Bin\	/s /y /d	/exclude:exclude.txt