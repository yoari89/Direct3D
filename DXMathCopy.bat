cd %~dp0

Rem : (��ġ������ �ִ� ���丮 ��ġ)�� ��������� _DXMath ������ ���ٸ� ���� �� �����Ѵ�.
xcopy DXMath\Include\*.h	_DXMath\Include\	/s /y /d	/exclude:exclude.txt

Rem : �ٸ� ������Ʈ���� Math Library�� ����� �� �ֵ��� �����Ѵ�. (Debug, Release)
xcopy DXMath\Bin\*.*	DXEngine\Bin\	/s /y /d	/exclude:exclude.txt