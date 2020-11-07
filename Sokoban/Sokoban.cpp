#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include "Sokoban.h"
//#include <termio.h>
//#include <unistd.h>

void GenerateMap(char * arr[]);						//�ؽ�Ʈ ���� �о����
void RenderBoard(char * arr[]);						//���� ���
void InputManager(char * arr[]);					//�Է� ó��
void MovePlayer(Movement movement, char * arr[]);	//�÷��̾� �̵�
void Swap(char *num1, char *num2);					//����
void Release(char * arr[]);							//�޸� ����

int main()
{
	char **board;

	//���� ����� ���߾� �����Ҵ�	
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

//�ؽ�Ʈ ���� �о����
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

//���� ���
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

//�Է� ó��
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

void MovePlayer(Movement movement, char * arr[]) //�÷��̾� �̵�
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

//������
void Swap(char *num1, char *num2)
{
	char tmp;
	tmp = *num1;
	*num1 = *num2;
	*num2 = tmp;
}

//�޸� ����
void Release(char * arr[])
{
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		free(arr[i]);
	}
	free(arr);
}
