#include "SysFrame.h"

//���õ�ǰ����λ��
void SetCurPos(int x, int y)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = { x, y };
	SetConsoleCursorPosition(handle, pos);
}
//���ع��
void HideCursor()
{
	CONSOLE_CURSOR_INFO cursor_info = { 100, 0 };
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(handle, &cursor_info);
}
//����
void DrawCol(int x, int y)
{
	for (int i = 0; i < HEIGHT; ++i)
	{
		SetCurPos(x, y + i);
		printf("||");
	}
}
//����
void DrawRow(int x, int y)
{
	for (int i = 0; i < WIDTH - 4; ++i)
	{
		SetCurPos(x + i, y);
		printf("=");
	}
}

//�����
void DrawFrame(char* title)
{
	char buffer[MAX_TITLE_SIZE + 6 + 1] = "title "; //6:title%20 1:\0
	strcat(buffer, title);
	system(buffer); //����ϵͳ����

	char mode[128] = { 0 };
	sprintf(mode, "mode con cols=%d lines=%d", WIDTH, HEIGHT);
	system(mode); //���ÿ���̨�ĳ��ȺͿ��
	system("color 0F");//������ɫ
	DrawCol(0, 0);
	DrawCol(WIDTH - 2, 0);
	DrawRow(2, 0);
	DrawRow(2, 2);
	DrawRow(2, 4);
	DrawRow(2, HEIGHT - 4);
	DrawRow(2, HEIGHT - 2);
}

void SystemEnd()
{
	SetCurPos(0, HEIGHT - 1);
}

extern const char* title;

//���˵�
void DrawMenu()
{
	//���������
	SetCurPos((WIDTH - 4 - strlen(title)) / 2, 1);
	printf("%s", title);
	//���� ·��
	SetCurPos(2, 3);
	printf("%-30s %-85s", "����", "·��");
	//�˳�����
	SetCurPos((WIDTH - 4 - strlen("exit �˳�ϵͳ .")) / 2, HEIGHT - 3);
	printf("%s", "exit �˳�ϵͳ .");
	DrawRow(2, HEIGHT - 6);
	//SetCurPos((WIDTH-4-strlen("������:>"))/2, 15);
	SetCurPos(2, HEIGHT - 5);
	printf("%s", "������:>");
}