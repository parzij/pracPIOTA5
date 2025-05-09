#include "TuringMachine.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define cnMax   256
#define maxSize 10000

// Подпрограмма для проверки входной двоичной строки
// Вход: указатель на C‑строку
// Выход: true, если строка непуста и содержит только '0' или '1', иначе - false
bool isBinary(char *str);

int main(void) {
    TM_Machine tm;
    bool initOk = tm_init(&tm, "tm_table.txt", '_');
    bool isWork = initOk;

    if (!initOk)
        printf("Ошибка чтения файла системы команд.\n");
    while (isWork) {
        char arrBuf[cnMax];
        bool validInp = false;
        while (!validInp) {
            printf("Введите неотрицательное число в двоичной форме (или \"exit\"): ");
            if (scanf("%255s", arrBuf) != 1) {
                isWork = false;          
                validInp = true;   
            } else if (strcmp(arrBuf, "exit") == 0) {
                isWork = false;         
                validInp = true;
            } else if (isBinary(arrBuf))
                validInp = true;  
            else
                printf("Ошибка: допустимы только символы 0 и 1.\n");
        }

        bool needProc = isWork;
        if (needProc) {
            tm_load_tape(&tm, arrBuf);
            tm_run(&tm, maxSize);
        }
    }
    return 0; 
}


bool isBinary(char *str) {
    bool isOk = true;
    if (str[0] == '\0')
        isOk = false;
    for (int i = 0; isOk && str[i] != '\0'; i++) {
        if (str[i] != '0' && str[i] != '1')
            isOk = false;
    }        
    return isOk;
}
