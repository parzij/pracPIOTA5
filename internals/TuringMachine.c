#include "TuringMachine.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>

/*
 Подпрограмма расширяет ленту пустой ячейкой слева
 Вход :
    1) указатель на машину
 Выход: (нет) – процедура
*/
void tmCellExpandL(TM_Machine *tm) {
    TM_Cell *newLeft = (TM_Cell *)malloc(sizeof(TM_Cell));
    newLeft -> sym = tm -> blank;
    newLeft -> left = NULL;
    newLeft -> right = tm -> leftmost;
    tm -> leftmost -> left = newLeft;
    tm -> leftmost = newLeft;
}

/*
 Подпрограмма расширяет ленту пустой ячейкой справа
 Вход :
    1) указатель на машину
 Выход: (нет) – процедура
*/
void tmCellExpandR(TM_Machine *tm) {
    TM_Cell *newRight = (TM_Cell *)malloc(sizeof(TM_Cell));
    newRight -> sym = tm -> blank;
    newRight -> right = NULL;
    newRight -> left = tm -> rightmost;
    tm -> rightmost -> right = newRight;
    tm -> rightmost = newRight;
}

/*
 Подпрограмма добавляет переход в структуру состояния
 Вход :
    1) указатель на структуру состояния
    2) структура перехода
 Выход: true – переход успешно добавлен, иначе false
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

/*
 Подпрограмма для создания и заполнения машины Тьюринга
 Вход :
    1) указатель на структуру
    2) имя файла с системой команд
    3) символ пустой ячейки
 Выход: true – чтение прошло успешно, иначе false
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
    bool isOk  = (fp != NULL);
    bool loop = isOk;

    while (loop) {
        int st = 0, next = 0;
        char read = 0, write = 0, dir = 0;
        int res = fscanf(fp, "%d %c %c %c %d", &st, &read, &write, &dir, &next);

        if (res == EOF)
            loop = false;
        else if (res != 5 || st >= MaxState || next >= MaxState) {
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

/*
 Подпрограмма заменяет текущую ленту новым словом
 Вход :
    1) указатель на машину
    2) строка‑слово, которую нужно разместить
 Выход: (нет) – процедура
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

/*
 Подпрограмма ищет переход по символу
 Вход :
    1) указатель на машину
    2) символ под головкой
 Выход: указатель на найденный переход или NULL
*/
TM_Transition *findTransition(TM_Machine *tm, char sym) {
    TM_State *st = &tm -> states[tm -> curState];
    TM_Transition *res = NULL;
    for (int i = 0; i < st -> count && res == NULL; i++) {
        if (st -> trs[i].read == sym)
            res = &st -> trs[i];
    }
    return res;
}

/*
 Подпрограмма выполняет один переход машины
 Вход :
    1) указатель на машину
 Выход: true  – машина перешла в q0, иначе false
*/
bool tmStep(TM_Machine *tm) {
    bool halted = false;
    TM_Transition *tr = findTransition(tm, tm -> head -> sym);
    if (tr == NULL)
        tm -> curState = 0;
    else {
        tm -> head -> sym = tr -> write;
        if (tr -> move == 'L') {
            if (tm -> head -> left == NULL) 
                tmCellExpandL(tm);
            tm -> head = tm -> head -> left;
        }
        else if (tr -> move == 'R') {
            if (tm -> head -> right == NULL) 
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

/*
 Подпрограмма отображает текущую конфигурацию машины
 Вход :
    1) указатель на машину
 Выход: (нет) – процедура
*/
void tmPrint(TM_Machine *tm) {
    const int window = 100;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    const WORD clrNorm = 7;
    const WORD clrRed = (WORD)(FOREGROUND_RED | FOREGROUND_INTENSITY);

    const TM_Cell *start = tm -> head;
    for (int moved = 0; moved < window && start -> left != NULL; moved++, start = start -> left);

    const TM_Cell *cur = start;
    for (int pos = 0; cur != NULL && pos < 2 * window; pos++, cur = cur -> right) {
        if (cur == tm -> head) 
            SetConsoleTextAttribute(hOut, clrRed);
        putchar(cur -> sym);
        if (cur == tm -> head) 
            SetConsoleTextAttribute(hOut, clrNorm);
    }
    putchar('\n');
    printf("q%d, step %d\n\n", tm -> curState, tm -> stepCount);
}

/*
 Подпрограмма запускает машину в автоматическом режиме
 Вход :
    1) указатель на машину
    2) максимальное число шагов
 Выход: (нет) – процедура
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

/*
 Подпрограмма освобождает все ячейки ленты и обнуляет указатели
 Вход :
    1) указатель на машину
 Выход: (нет) – процедура
*/
void tmFree(TM_Machine *tm) {
    TM_Cell *p = tm -> leftmost;
    while (p != NULL) {
        TM_Cell *next = p -> right;
        free(p);
        p = next;
    }
    tm -> leftmost = NULL;
    tm -> rightmost = NULL;
    tm -> head = NULL;
}
