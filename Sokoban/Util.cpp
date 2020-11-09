//콘솔 창에 관련된 함수를 제어하는 부분이다.
#include <Windows.h>

void SetColor(int color) //폰트색
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void gotoxy(int x, int y) //글자 위치
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void _SetCursor(bool bVisible) //커서 숨기기
{
	CONSOLE_CURSOR_INFO ConsoleCursor;
	ConsoleCursor.bVisible = bVisible; //true는 보임, false 는 안 보임	
	ConsoleCursor.dwSize = 1; //커서 사이즈
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ConsoleCursor);
}