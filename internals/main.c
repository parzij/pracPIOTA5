#include "TuringMachine.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define cnMAX   256
#define maxStep 10000

/*
 Подпрограмма проверяет, является ли строка двоичным числом
 Вход : указатель на строку
 Выход: true, если непуста и состоит только из '0' и '1'
*/
bool isBinary(char *str);

int main(void) {
    TM_Machine tm;
    bool initOk = tmInit(&tm, "tm_table.txt", '_');
    bool isWork = initOk;

    if (!initOk) 
        printf("Ошибка чтения файла системы команд.\n");

    while (isWork) {
        char arrBuf[cnMAX];
        bool isCorrect = false;
        while (!isCorrect) {
            printf("Введите неотрицательное число в двоичной форме (или \"exit\"): ");
            if (scanf("%255s", arrBuf) != 1) {
                isWork = false;
                isCorrect = true;
            } else if (strcmp(arrBuf, "exit") == 0) {
                isWork = false;
                isCorrect = true;
            } else if (isBinary(arrBuf))
                isCorrect = true;
            else
                printf("Ошибка: допустимы только символы 0 и 1.\n");
        }
        if (isWork) {
            tmLoadTape(&tm, arrBuf);
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