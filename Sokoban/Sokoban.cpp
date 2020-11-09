#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include "Sokoban.h"
#include "Util.h"
//#include <termio.h>
//#include <unistd.h>

void Init();													//���� �ʱ�ȭ
void GenerateMap(char * arr[]);									//�ؽ�Ʈ ���� �о����
void RenderBoard(char * arr[], char * buffer[]);									//���� ���
void SaveFixedObjectPos(char * arr[], char * layer[]);			//���� ������Ʈ(GOAL) ���̾� ����
void SetFixedObject(char * arr[], char * layer[]);				//���� ������Ʈ(GOAL) ��ġ
Position GetPlayerPos(char * arr[]);
void InputManager(char * arr[]);								//�Է� ó��
void Move(Position pos, Movement movement, char * arr[]);		//�÷��̾�/��ü �̵�
BlockType GetBlockType(int yPos, int xPos, char * arr[]);		//�̵� ��ġ�� ������ �޾ƿ���
void Swap(char *num1, char *num2);								//����
void gotoxy(int x, int y);										//���� ��ġ
void Release(char * arr[]);										//�޸� ����

int currentLevel;												//���� ���̵�
int numOfGold;													//�ʿ� �����ϴ� ���($) ����

int recursiveCount;												//����Լ� ī��Ʈ


int main()
{
	char **board;				//���� �迭
	char **backBufferLayer;		//������۸��� ���� ���̾�
	char **FixedObjectLayer;	//���� ������Ʈ ���̾�

	//���� ����� ���߾� �����Ҵ�	
	board = (char**)malloc(sizeof(char*) * MAP_HEIGHT);
	FixedObjectLayer = (char**)malloc(sizeof(char*) * MAP_HEIGHT);
	backBufferLayer = (char**)malloc(sizeof(char*) * MAP_HEIGHT);
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		board[i] = (char*)malloc(sizeof(char) * MAP_WIDTH);
		FixedObjectLayer[i] = (char*)malloc(sizeof(char) * MAP_WIDTH);
		backBufferLayer[i] = (char*)malloc(sizeof(char) * MAP_WIDTH);
	}

	/*=====������� ������ ����=====*/
	Init();
	GenerateMap(board);
	SaveFixedObjectPos(board, FixedObjectLayer);
	while (true)
	{
		RenderBoard(board, backBufferLayer);
		InputManager(board);
		SetFixedObject(board, FixedObjectLayer);
	}
	Release(board);
	/*================================*/
}

//���� �ʱ�ȭ
void Init()
{
	system("mode con cols=60 lines=35"); //������ �ܼ�â ũ��
	_SetCursor(false); //Ŀ�� �����
	currentLevel = 1;
	numOfGold = 0;
	recursiveCount = 0;
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
	if (fp == NULL) return;

	char tmp[255][255]; //�ӽ� ����
	for (int i = 0; i < 255; i++)
	{
		fscanf(fp, "%s", tmp[i]);
	}
	
	int count = 0;							//�迭 �ε��� 0���� �����ϱ� ����
	bool isReadyToRead = false;
	char level[2];
	char Nextlevel[2];
	sprintf(level, "%d", currentLevel);		//���� ������ ���ڿ��� ����
	sprintf(Nextlevel, "%d", currentLevel + 1);

	for (int i = 0; i < 255; i++)
	{
		if (strcmp(tmp[i], level) == 0)	//�б� ����
		{
			isReadyToRead = true;
			continue;
		}

		if (strcmp(tmp[i], Nextlevel) == 0 || strcmp(tmp[i], "e") == 0) //���� ���� ��ũ�� e�� �����ϸ� �б� ����
		{
			isReadyToRead = false;
			break;
		}

		if (isReadyToRead == true)
		{
			strcpy(arr[count], tmp[i]);
			for (int j = 0; j < MAP_WIDTH; j++)
			{
				if (arr[count][j] == '$')	//��� ���� ī��Ʈ
					numOfGold++;
			}
			count++;
		}
	}
}

//���� ���
void RenderBoard(char * arr[], char * buffer[])
{
	//system("cls");
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_WIDTH; j++)
		{
			gotoxy(j * 2, i);
			if (buffer[i][j] != arr[i][j])
				printf("%c", arr[i][j]);
		}
		printf("\n");
	}
	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			buffer[y][x] = arr[y][x];
		}
	}
}

//�׻� �ڸ��� �����Ǿ�� �־���ϴ� ����(�������) ��ǥ �ʱ�ȭ
void SaveFixedObjectPos(char * arr[], char * layer[])
{
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_WIDTH; j++)
		{
			layer[i][j] = ' ';
			if (arr[i][j] == 'O')
			{
				layer[i][j] = arr[i][j];
			}
		}
	}
}

//�׻� �ڸ��� �����Ǿ�� �־���ϴ� ����(�������) ����
//�������� �÷��̾� �̵��� �ϱ� ������, GOAL�� ��ġ�� �ٲ�°� ���� ����
void SetFixedObject(char * arr[], char * layer[])
{
	int goldOnGoals = 0;	//â�� ���� �÷��� ��� ����
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_WIDTH; j++)
		{
			if (arr[i][j] == 'O') //���� 'O' ��ġ�� �־�� �� ��ġ�� �ƴϸ�
			{
				if (arr[i][j] != layer[i][j])
					arr[i][j] = '.';
			}
			if (layer[i][j] == 'O') //���� 'O' ��ġ�� �־�� �� ��ġ�� ������
			{
				if (arr[i][j] == '@') continue;

				if (arr[i][j] == '$')
				{
					goldOnGoals++;
					if (goldOnGoals == numOfGold) //Ŭ����
					{
						system("cls");

						break;
					}
					continue;
				}
				if (arr[i][j] != layer[i][j])
					arr[i][j] = 'O';
			}
		}
	}
}

Position GetPlayerPos(char * arr[])
{
	Position position;
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_WIDTH; j++)
		{
			if (arr[i][j] == '@')
			{
				position.pos_Y = i;
				position.pos_X = j;
			}
		}
	}
	return position;
}

//�Է� ó��
void InputManager(char * arr[])
{
	int key;
	key = _getch();
	//printf("%d %c\n", key, key);
	recursiveCount = 0;
	switch (key)
	{
		//=Movement=
		case Key_h:
			Move(GetPlayerPos(arr), LEFT, arr);
			break;
		case Key_j:
			Move(GetPlayerPos(arr), DOWN, arr);
			break;
		case Key_k:
			Move(GetPlayerPos(arr), UP, arr);
			break;
		case Key_l:
			Move(GetPlayerPos(arr), RIGHT, arr);
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



//�÷��̾� �Ǵ� ������Ʈ �̵�
void Move(Position pos, Movement movement, char * arr[])
{
	if (recursiveCount >= 2) return;

	int player_Xpos = pos.pos_X;
	int player_Ypos = pos.pos_Y;

	int desired_Xpos = player_Xpos;
	int desired_Ypos = player_Ypos;
	//�÷��̾ ������ �ϴ� ��ġ
	switch (movement)
	{
	case UP:
		desired_Ypos--;
		break;
	case DOWN:
		desired_Ypos++;
		break;
	case LEFT:
		desired_Xpos--;
		break;
	case RIGHT:
		desired_Xpos++;
		break;
	}

	if (GetBlockType(desired_Ypos, desired_Xpos, arr) == BLANK)
	{
		Swap(&arr[player_Ypos][player_Xpos], &arr[desired_Ypos][desired_Xpos]);
	}

	else if (GetBlockType(desired_Ypos, desired_Xpos, arr) == GOLD)
	{
		Position GoldPos;
		GoldPos.pos_X = desired_Xpos;
		GoldPos.pos_Y = desired_Ypos;
		recursiveCount++;
		Move(GoldPos, movement, arr);
		if (recursiveCount >= 2)
		{
			return;
		}
		Move(GetPlayerPos(arr), movement, arr);
	}
	else if (GetBlockType(desired_Ypos, desired_Xpos, arr) == GOAL)
	{
		Swap(&arr[player_Ypos][player_Xpos], &arr[desired_Ypos][desired_Xpos]);
	}
	else if (GetBlockType(desired_Ypos, desired_Xpos, arr) == WALL)
		return;
}

//�ش���ġ ������ ��ȯ
BlockType GetBlockType(int yPos, int xPos, char * arr[])
{
	switch (arr[yPos][xPos])
	{
		case '.':
			return BLANK;
		case '#':
			return WALL;
		case '$':
			return GOLD;
		case 'O':
			return GOAL;
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
