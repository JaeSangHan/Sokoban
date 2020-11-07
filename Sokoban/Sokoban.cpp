#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include "Sokoban.h"
//#include <termio.h>
//#include <unistd.h>

void GenerateMap(char * arr[]);						//텍스트 파일 읽어오기
void RenderBoard(char * arr[]);						//보드 출력
void InputManager(char * arr[]);					//입력 처리
void MovePlayer(Movement movement, char * arr[]);	//플레이어 이동
void Swap(char *num1, char *num2);					//스왑
void Release(char * arr[]);							//메모리 해제

int main()
{
	char **board;

	//보드 사이즈에 맞추어 동적할당	
	board = (char**)malloc(sizeof(char*) * MAP_HEIGHT);
	for (int i = 0; i < MAP_HEIGHT; i++)
		board[i] = (char*)malloc(sizeof(char) * MAP_WIDTH);


	GenerateMap(board);
	while (true)
	{
		RenderBoard(board);
		InputManager(board);
	}
	Release(board);
}

//텍스트 파일 읽어오기
void GenerateMap(char * arr[]) 
{
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_WIDTH; j++)
		{
			arr[i][j] = ' ';
		}
	}

	FILE* fp = fopen("map.txt", "r");

	for (int i = 0; i < MAP_HEIGHT; i++)
		fscanf(fp, "%s", arr[i]);
}

//보드 출력
void RenderBoard(char * arr[])
{
	system("cls");
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_WIDTH; j++)
		{
			printf("%c", arr[i][j]);
		}
		printf("\n");
	}
}

//입력 처리
void InputManager(char * arr[])
{
	int key;
	key = _getch();
	//printf("%d %c\n", key, key);

	switch (key)
	{
		//=Movement=
		case Key_h:
			MovePlayer(LEFT, arr);
			break;
		case Key_j:
			MovePlayer(DOWN, arr);
			break;
		case Key_k:
			MovePlayer(UP, arr);
			break;
		case Key_l:
			MovePlayer(RIGHT, arr);
			break;

		case Key_u: //undo
			break;
		case Key_r: //replay
			break;
		case Key_n: //new
			break;
		case Key_e: //exit
			break;
		case Key_s: //save
			break;
		case Key_f: //file load
			break;
		case Key_d: //display help
			break;
		case Key_t: //top
			break;
	}
}

void MovePlayer(Movement movement, char * arr[]) //플레이어 이동
{
	int player_Xpos;
	int player_Ypos;

	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_WIDTH; j++)
		{
			if (arr[i][j] == '@')
			{
				player_Ypos = i;
				player_Xpos = j;
			}
		}
	}

	switch (movement)
	{
	case UP:
		Swap(&arr[player_Ypos][player_Xpos], &arr[player_Ypos-1][player_Xpos]);
		break;
	case DOWN:
		Swap(&arr[player_Ypos][player_Xpos], &arr[player_Ypos+1][player_Xpos]);
		break;
	case LEFT:
		Swap(&arr[player_Ypos][player_Xpos], &arr[player_Ypos][player_Xpos-1]);
		break;
	case RIGHT:
		Swap(&arr[player_Ypos][player_Xpos], &arr[player_Ypos][player_Xpos+1]);
		break;
	}
}

//스와핑
void Swap(char *num1, char *num2)
{
	char tmp;
	tmp = *num1;
	*num1 = *num2;
	*num2 = tmp;
}

//메모리 해제
void Release(char * arr[])
{
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		free(arr[i]);
	}
	free(arr);
}
