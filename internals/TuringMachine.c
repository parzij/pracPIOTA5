/**
 * @file  TuringMachine.c
 * @brief Реализация одноленточной детерминированной машины Тьюринга.
 */

#include "TuringMachine.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Добавляет пустую ячейку слева от текущего левого края.
 *
 * @param tm Указатель на машину.
 */
void tmCellExpandL(TM_Machine *tm) {
    TM_Cell *newLeft = (TM_Cell *)malloc(sizeof(TM_Cell));
    newLeft -> sym = tm -> blank;
    newLeft -> left  = NULL;
    newLeft -> right = tm -> leftmost;
    tm -> leftmost -> left = newLeft;
    tm -> leftmost = newLeft;
}

/**
 * @brief Добавляет пустую ячейку справа от текущего правого края.
 *
 * @param tm Указатель на машину.
 */
void tmCellExpandR(TM_Machine *tm) {
    TM_Cell *newRight = (TM_Cell *)malloc(sizeof(TM_Cell));
    newRight -> sym = tm -> blank;
    newRight -> right = NULL;
    newRight -> left = tm -> rightmost;
    tm -> rightmost -> right = newRight;
    tm -> rightmost = newRight;
}

/**
 * @brief Добавляет переход в таблицу состояния.
 *
 * @param st Указатель на состояние, в которое нужно добавить правило.
 * @param tr Структура перехода.
 * @return `true`, если переход успешно добавлен, иначе `false`.
 */
bool addTransition(TM_State *st, TM_Transition tr) {
    bool isOk = false;
    if (st -> count < MaxSymbol) {
        st -> trs[st -> count] = tr;
        st -> count += 1;
        isOk = true;
    }
    return isOk;
}

/**
 * @brief Создаёт и заполняет структуру машины Тьюринга.
 *
 * @param tm Структура машины.
 * @param fname Файл с таблицей команд.
 * @param blank Символ пустой ячейки.
 * @return `true`, если чтение файла прошло успешно, иначе `false`.
 */
bool tmInit(TM_Machine *tm, char *fname, char blank) {
    for (int i = 0; i < MaxState; i++)
        tm -> states[i].count = 0;

    tm -> stateQty = 0;
    tm -> curState = 1;
    tm -> blank = blank;
    tm -> head = NULL;
    tm -> leftmost = NULL;
    tm -> rightmost = NULL;
    tm -> stepCount = 0;

    FILE *fp = fopen(fname, "r");
    bool isOk = (fp != NULL);
    bool loop = isOk;

    while (loop) {
        int st = 0, next = 0;
        char read = 0, write = 0, dir = 0;
        int res = fscanf(fp, "%d %c %c %c %d", &st, &read, &write, &dir, &next);

        if (res == EOF) {
            loop = false;
        } else if (res != 5 || st >= MaxState || next >= MaxState) {
            isOk = false;
            loop = false;
        } else {
            TM_Transition tr = {read, write, dir, next};
            if (!addTransition(&tm -> states[st], tr)) {
                isOk = false;
                loop = false;
            }
            if (st + 1 > tm -> stateQty) 
                tm -> stateQty = st + 1;
            if (next + 1 > tm -> stateQty) 
                tm -> stateQty = next + 1;
        }
    }

    if (fp) 
        fclose(fp);
    return isOk;
}

/**
 * @brief Заменяет текущую ленту новым словом.
 *
 * @param tm Указатель на машину.
 * @param input Нуль-терминированная строка для размещения на ленте.
 */
void tmLoadTape(TM_Machine *tm, char *input) {
    TM_Cell *p = tm -> leftmost;
    while (p != NULL) {
        TM_Cell *next = p -> right;
        free(p);
        p = next;
    }

    TM_Cell *left = (TM_Cell *)malloc(sizeof(TM_Cell));
    left -> sym = tm -> blank;
    left -> left = NULL;
    left -> right = NULL;
    tm -> leftmost = left;

    TM_Cell *cur = left;
    size_t len = strlen(input);
    for (size_t k = 0; k < len; k++) {
        TM_Cell *node = (TM_Cell *)malloc(sizeof(TM_Cell));
        node -> sym = input[k];
        node -> left = cur;
        node -> right = NULL;
        cur -> right = node;
        cur = node;
    }

    TM_Cell *right = (TM_Cell *)malloc(sizeof(TM_Cell));
    right -> sym = tm -> blank;
    right -> right = NULL;
    right -> left = cur;
    cur -> right = right;
    tm -> rightmost = right;
    tm -> head = left -> right;
    tm -> curState = 1;
    tm -> stepCount = 0;
}

/**
 * @brief Ищет переход по текущему символу.
 *
 * @param tm Машина.
 * @param sym Символ под головкой.
 * @return Указатель на найденный переход или `NULL`.
 */
TM_Transition *findTransition(TM_Machine *tm, char sym) {
    TM_State *st = &tm -> states[tm -> curState];
    TM_Transition *res = NULL;
    for (int i = 0; i < st -> count && !res; i++) {
        if (st -> trs[i].read == sym)    
            res = &st -> trs[i];
    }
    return res;
}

/**
 * @brief Выполняет один переход машины.
 *
 * @param tm Указатель на машину.
 * @return `true`, если машина остановилась (q0), иначе `false`.
 */
bool tmStep(TM_Machine *tm) {
    bool halted = false;
    TM_Transition *tr = findTransition(tm, tm -> head -> sym);
    if (!tr) {
        tm -> curState = 0; 
    } else {
        tm -> head -> sym = tr -> write;
        if (tr -> move == 'L') {
            if (!tm -> head -> left) 
                tmCellExpandL(tm);
            tm -> head = tm -> head -> left;
        } else if (tr -> move == 'R') {
            if (!tm -> head -> right) 
                tmCellExpandR(tm);
            tm -> head = tm -> head -> right;
        }
        tm -> curState = tr -> nextState;
    }

    tm -> stepCount++;
    if (tm -> curState == 0) 
        halted = true;
    return halted;
}

/**
 * @brief Печатает окно ленты, состояние и счётчик шагов.
 *
 * @param tm Машина, чью конфигурацию нужно вывести.
 */
void tmPrint(TM_Machine *tm) {
    const int window = 100;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    const WORD clrNorm = 7;
    const WORD clrRed = (WORD)(FOREGROUND_RED | FOREGROUND_INTENSITY);

    const TM_Cell *start = tm -> head;
    for (int moved = 0; moved < window && start -> left; moved++, start = start -> left);

    const TM_Cell *cur = start;
    for (int pos = 0; cur && pos < 2 * window; pos++, cur = cur -> right) {
        if (cur == tm -> head) 
            SetConsoleTextAttribute(hOut, clrRed);
        putchar(cur -> sym);
        if (cur == tm -> head) 
            SetConsoleTextAttribute(hOut, clrNorm);
    }

    putchar('\n');
    printf("q%d, step %d\n\n", tm -> curState, tm -> stepCount);
}

/**
 * @brief Непрерывный запуск машины с ограничением по количеству шагов.
 *
 * @param tm Машина.
 * @param maxSteps Максимум шагов.
 */
void tmRun(TM_Machine *tm, int maxSteps) {
    tmPrint(tm);
    bool isDone = false;
    int made = 0;

    while (!isDone && made < maxSteps) {
        isDone = tmStep(tm);
        tmPrint(tm);
        made++;
    }

    (isDone) ? printf("Машина завершила работу (q0).\n") : printf("Достигнут лимит %d шагов.\n", maxSteps);
}

/**
 * @brief Освобождает память, занимаемую текущей лентой.
 *
 * @param tm Машина, чью ленту нужно очистить.
 */
void tmFree(TM_Machine *tm) {
    TM_Cell *p = tm -> leftmost;
    while (p) {
        TM_Cell *next = p -> right;
        free(p);
        p = next;
    }
    tm -> leftmost = NULL;
    tm -> rightmost = NULL;
    tm -> head = NULL;
}

/**
 * @brief Проверяет, что строка содержит непустое двоичное число.
 *
 * @param str Нуль-терминированная строка.
 * @return `true`, если строка непуста и состоит только из '0' и '1'.
 */
bool isBinary(char *str) {
    bool ok = (str != NULL) && (str[0] != '\0');
    for (int i = 0; ok && str[i] != '\0'; i++) {
        if (str[i] != '0' && str[i] != '1') 
            ok = false;
    }
    return ok;
}

/**
 * @brief Диалог с пользователем: ввод слова → запуск → вывод.
 *
 * @param tm Экземпляр машины.
 * @param maxSteps Ограничение на количество шагов в одной сессии.
 */
void tm_run(TM_Machine *tm, int maxSteps) {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    if (GetConsoleMode(hOut, &mode))
        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    bool isWork = true;
    while (isWork) {
        char arrBuff[cnMax];
        bool isCorrect = false;
        while (!isCorrect) {
            printf("Введите неотрицательное число в двоичной форме (или \"exit\"): ");
            if (scanf("%255s", arrBuff) != 1) {
                isWork = false;
                isCorrect = true;
            } else if (strcmp(arrBuff, "exit") == 0) {
                isWork = false;
                isCorrect = true;
            } else if (isBinary(arrBuff)) {
                isCorrect = true;
            } else {
                printf("Ошибка: допустимы только символы 0 и 1.\n");
            }
        }

        if (isWork) {
            tmLoadTape(tm, arrBuff);
            tmRun(tm, maxSteps);
        }
    }
}
