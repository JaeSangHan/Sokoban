//�ܼ� â�� ���õ� �Լ��� �����ϴ� �κ��̴�.
#include <Windows.h>

void SetColor(int color) //��Ʈ��
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void gotoxy(int x, int y) //���� ��ġ
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void _SetCursor(bool bVisible) //Ŀ�� �����
{
	CONSOLE_CURSOR_INFO ConsoleCursor;
	ConsoleCursor.bVisible = bVisible; //true�� ����, false �� �� ����	
	ConsoleCursor.dwSize = 1; //Ŀ�� ������
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ConsoleCursor);
}