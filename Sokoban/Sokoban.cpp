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

void Init();													//설정 초기화
void GenerateMap(char * arr[]);									//텍스트 파일 읽어오기
void RenderBoard(char * arr[], char * buffer[]);									//보드 출력
void SaveFixedObjectPos(char * arr[], char * layer[]);			//고정 오브젝트(GOAL) 레이어 설정
void SetFixedObject(char * arr[], char * layer[]);				//고정 오브젝트(GOAL) 배치
Position GetPlayerPos(char * arr[]);
void InputManager(char * arr[]);								//입력 처리
void Move(Position pos, Movement movement, char * arr[]);		//플레이어/물체 이동
BlockType GetBlockType(int yPos, int xPos, char * arr[]);		//이동 위치의 블럭정보 받아오기
void Swap(char *num1, char *num2);								//스왑
void gotoxy(int x, int y);										//글자 위치
void Release(char * arr[]);										//메모리 해제

int currentLevel;												//현재 난이도
int numOfGold;													//맵에 존재하는 골드($) 개수

int recursiveCount;												//재귀함수 카운트


int main()
{
	char **board;				//보드 배열
	char **backBufferLayer;		//더블버퍼링을 위한 레이어
	char **FixedObjectLayer;	//고정 오브젝트 레이어

	//보드 사이즈에 맞추어 동적할당	
	board = (char**)malloc(sizeof(char*) * MAP_HEIGHT);
	FixedObjectLayer = (char**)malloc(sizeof(char*) * MAP_HEIGHT);
	backBufferLayer = (char**)malloc(sizeof(char*) * MAP_HEIGHT);
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		board[i] = (char*)malloc(sizeof(char) * MAP_WIDTH);
		FixedObjectLayer[i] = (char*)malloc(sizeof(char) * MAP_WIDTH);
		backBufferLayer[i] = (char*)malloc(sizeof(char) * MAP_WIDTH);
	}

	/*=====여기부터 게임이 시작=====*/
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

//설정 초기화
void Init()
{
	system("mode con cols=60 lines=35"); //윈도우 콘솔창 크기
	_SetCursor(false); //커서 숨기기
	currentLevel = 1;
	numOfGold = 0;
	recursiveCount = 0;
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
	if (fp == NULL) return;

	char tmp[255][255]; //임시 버퍼
	for (int i = 0; i < 255; i++)
	{
		fscanf(fp, "%s", tmp[i]);
	}
	
	int count = 0;							//배열 인덱스 0부터 저장하기 위해
	bool isReadyToRead = false;
	char level[2];
	char Nextlevel[2];
	sprintf(level, "%d", currentLevel);		//현재 레벨을 문자열로 저장
	sprintf(Nextlevel, "%d", currentLevel + 1);

	for (int i = 0; i < 255; i++)
	{
		if (strcmp(tmp[i], level) == 0)	//읽기 시작
		{
			isReadyToRead = true;
			continue;
		}

		if (strcmp(tmp[i], Nextlevel) == 0 || strcmp(tmp[i], "e") == 0) //다음 레벨 마크나 e에 도달하면 읽기 종료
		{
			isReadyToRead = false;
			break;
		}

		if (isReadyToRead == true)
		{
			strcpy(arr[count], tmp[i]);
			for (int j = 0; j < MAP_WIDTH; j++)
			{
				if (arr[count][j] == '$')	//골드 개수 카운트
					numOfGold++;
			}
			count++;
		}
	}
}

//보드 출력
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

//항상 자리에 고정되어야 있어야하는 옵젝(저장장소) 좌표 초기화
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

//항상 자리에 고정되어야 있어야하는 옵젝(저장장소) 세팅
//스왑으로 플레이어 이동을 하기 때문에, GOAL의 위치가 바뀌는걸 막기 위함
void SetFixedObject(char * arr[], char * layer[])
{
	int goldOnGoals = 0;	//창고 위에 올려진 골드 개수
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_WIDTH; j++)
		{
			if (arr[i][j] == 'O') //현재 'O' 위치가 있어야 할 위치가 아니면
			{
				if (arr[i][j] != layer[i][j])
					arr[i][j] = '.';
			}
			if (layer[i][j] == 'O') //현재 'O' 위치가 있어야 할 위치에 없으면
			{
				if (arr[i][j] == '@') continue;

				if (arr[i][j] == '$')
				{
					goldOnGoals++;
					if (goldOnGoals == numOfGold) //클리어
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

//입력 처리
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



//플레이어 또는 오브젝트 이동
void Move(Position pos, Movement movement, char * arr[])
{
	if (recursiveCount >= 2) return;

	int player_Xpos = pos.pos_X;
	int player_Ypos = pos.pos_Y;

	int desired_Xpos = player_Xpos;
	int desired_Ypos = player_Ypos;
	//플레이어가 가고자 하는 위치
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

//해당위치 블럭정보 반환
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
