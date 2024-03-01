#define _CRT_SECURE_NO_WARNINGS

#pragma hdrstop
#include <locale.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#define maxCount 100
#define STUDENT struct STUDENT
STUDENT
{
	char surname[30];
	int yearNumber;
	int grant;
}
*p[maxCount], * temp;
int numberStudent[maxCount];

void InputStudent(STUDENT* p[maxCount])
{
	int i;
	printf("\nВведите номер студента: ");
	scanf("%d", &i);
	numberStudent[i] = i;
	printf("\nВведите фамилию студента: ");
	scanf("%s", &p[i]->surname);
	printf("\nВведите номер курса: ");
	scanf("%d", &p[i]->yearNumber);
	printf("\nВведите размер стипендии: ");
	scanf("%d", &p[i]->grant);
	printf("Студент зарегистрирован\n");
}

void OutputStudent(STUDENT* p[maxCount])
{
	int i;
	printf("Введите номер студента: ");
	scanf("%d", &i);
	if ((p[i]->surname[0] != '\0') & (p[i]->yearNumber != 0))
	{
		printf("Студент №%d\n", numberStudent[i]);
		printf("Фамилия: %s\n", p[i]->surname);
		printf("Номер курса: %d\n", p[i]->yearNumber);
		printf("Стипендия: %d\n", p[i]->grant);
	}
	else printf("Студент под номером %d не зарегистрирован\n", i);
}

void SortDatabase(STUDENT* p[maxCount])
{
	int n;
	for (int i = 0; i < maxCount - 1; i++)
	{
		for (int j = i + 1; j < maxCount; j++)
		{
			if (p[i]->surname[0] > p[j]->surname[0])
			{
				temp = p[i];
				n = numberStudent[i];
				p[i] = p[j];
				numberStudent[i] = numberStudent[j];
				p[j] = temp;
				numberStudent[j] = n;
			}
		}
	}
	printf("Сортировка прошла успешно\n");
}

void OutputPrinter(STUDENT* p[maxCount])
{
	printf("Производится вывод записей на принтер...\n");
	FILE* stream = fopen("base.prn", "w");
	if (stream != NULL)
	{
		for (int i = 0; i < maxCount; i++)
		{
			if ((p[i]->surname[0] != '\0') & (p[i]->yearNumber != 0))
			{
				fprintf(stream, "Студент №%d\n", numberStudent[i]);
				fprintf(stream, "%s\n", p[i]->surname);
				fprintf(stream, "%d\n", p[i]->yearNumber);
				fprintf(stream, "%d\n", p[i]->grant);
			}
		}
		fclose(stream);
		printf("Вывод записей на принтер завершен успешно\n");
	}
	else printf("Ошибка вывода\n");
}

void SaveFile(STUDENT* p[maxCount])
{
	printf("Производится сохранение записей в текстовый файл...\n");
	FILE* stream = fopen("database.txt", "w");
	if (stream != NULL)
	{
		for (int i = 0; i < maxCount; i++)
		{
			if ((p[i]->surname[0] != '\0') & (p[i]->yearNumber != 0))
			{
				fprintf(stream, "%s\n", p[i]->surname);
				fprintf(stream, "%d\n", p[i]->yearNumber);
				fprintf(stream, "%d\n", p[i]->grant);
			}
		}
		fclose(stream);
		printf("Сохранение завершено успешно\n");
	}
	else printf("Ошибка сохранения\n");
}

void InputFile(STUDENT* p[maxCount])
{
	printf("Производится чтение записей из текстового файла...\n");
	FILE* stream = fopen("database.txt", "r");
	if (stream != NULL)
	{
		for (int i = 1; (i < maxCount + 1) & !(feof(stream)); i++)
		{
			fscanf(stream, "%s\n", &p[i]->surname);
			fscanf(stream, "%d\n", &p[i]->yearNumber);
			fscanf(stream, "%d\n", &p[i]->grant);
			numberStudent[i] = i;
		}
		fclose(stream);
		printf("Чтение завершено успешно\n");
	}
	else printf("Ошибка чтения\n");
}

void main()
{
	setlocale(LC_ALL, "Russian");
	char ch;
	for (int i = 0; i < maxCount; i++)
	{
		p[i] = (STUDENT*)malloc(sizeof(STUDENT));
		strcpy(p[i]->surname, " ");
		p[i]->yearNumber = 0;
		p[i]->grant = 0;
	}
	printf("Нажмите 1, чтобы зарегистрировать студента\n");
	printf("Нажмите 2, чтобы посмотреть данные о студенте\n");
	printf("Нажмите 3, чтобы отсортировать базу данных по возрастанию фамилии\n");
	printf("Нажмите 4, чтобы вывести отсортированную базу данных на печать\n");
	printf("Нажмите 5, чтобы сохранить базу данных в текстовый файл\n");
	printf("Нажмите 6, чтобы прочитать базу данных из тектового файла\n");
	printf("Нажмите 7, чтобы завершить работу\n");
	while (ch = getchar())
	{
		switch (ch)
		{
		case '1':
			InputStudent(p);
			getchar();
			break;
		case '2':
			OutputStudent(p);
			getchar();
			break;
		case '3':
			SortDatabase(p);
			getchar();
			break;
		case '4':
			OutputPrinter(p);
			getchar();
			break;
		case '5':
			SaveFile(p);
			getchar();
			break;
		case '6':
			InputFile(p);
			getchar();
			break;
		case '7':
			free(p);
		default:
			printf("Некорректный ввод, выберите одну из предложенных опций\n");
			getchar();
		}
	}
}




