#include "TuringMachine.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define cnMAX   256
#define maxStep 10000000

/*
 Подпрограмма проверяет, является ли строка двоичным числом
 Вход :
    1) указатель на строку
 Выход: true, если непуста и состоит только из '0' и '1'
*/
bool isBinary(char *str);

int main(void) {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD  mode;
    if (GetConsoleMode(hOut, &mode))
        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    TM_Machine tm;
    bool initOk = tmInit(&tm, "tmTable.txt", '_');
    bool isWork   = initOk;

    if (!initOk)
        printf("Ошибка чтения файла системы команд.\n");

    while (isWork) {
        char arrBuff[cnMAX];
        bool isCorrect = false;

        while (!isCorrect) {
            printf("Введите неотрицательное число в двоичной форме (или \"exit\"): ");
            if (scanf("%255s", arrBuff) != 1) {
                isWork = false;
                isCorrect = true;
            } else if (strcmp(arrBuff, "exit") == 0) {
                isWork = false;
                isCorrect = true;
            } else if (isBinary(arrBuff))
                isCorrect = true;
            else
                printf("Ошибка: допустимы только символы 0 и 1.\n");
        }

        if (isWork) {
            tmLoadTape(&tm, arrBuff);
            tmRun(&tm, maxStep);
        }
    }

    tmFree(&tm);
    return 0;
}

bool isBinary(char *str) {
    bool ok = str[0] != '\0';
    for (int i = 0; ok && str[i] != '\0'; i++)
        if (str[i] != '0' && str[i] != '1')
            ok = false;
    return ok;
}
