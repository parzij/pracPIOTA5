#ifndef TURING_MACHINE_H
#define TURING_MACHINE_H

#include <stdbool.h>
#include <stdio.h>

/** Максимальное количество состояний и символов во всей машине.
    При необходимости значения можно увеличить. */
enum { TM_MAX_STATE = 64, TM_MAX_SYMBOL = 64 };

/** Переход: read → write, move, next_state */
typedef struct {
    char  read;        /*!< символ, ожидаемый в текущей ячейке            */
    char  write;       /*!< символ, который нужно записать                */
    char  move;        /*!< 'L', 'R' или 'S'                              */
    int   next_state;  /*!< новый номер состояния                          */
} TM_Transition;

/** Набор переходов для одного состояния */
typedef struct {
    int            count;                /*!< сколько переходов описано      */
    TM_Transition  trs[TM_MAX_SYMBOL];   /*!< сами переходы                  */
} TM_State;

/** Узел двусвязной ленты */
typedef struct TM_Cell {
    char              sym;               /*!< символ в ячейке                */
    struct TM_Cell   *left, *right;      /*!< соседи                         */
} TM_Cell;

/** Собственно машина Тьюринга */
typedef struct {
    TM_State states[TM_MAX_STATE]; /*!< система команд                   */
    int      state_qty;            /*!< сколько состояний загружено      */
    int      cur_state;            /*!< текущее состояние                */
    TM_Cell *head;                 /*!< текущая ячейка (головка)          */
    TM_Cell *leftmost;             /*!< крайняя левая непустая            */
    TM_Cell *rightmost;            /*!< крайняя правая непустая           */
    char     blank;                /*!< символ пустой ячейки (‘_’)        */
    int      step_counter;         /*!< число уже выполненных шагов       */
} TM_Machine;

/* -------------------------- публичный интерфейс -------------------------- */

/** Загружает систему команд из текстового файла.
    @return true – успех, false – ошибка чтения/формата                    */
bool tm_init(TM_Machine *tm, const char *fname, char blank);

/** Перезаписывает ленту введённой строкой (доб. один blank слева/справа) */
void tm_load_tape(TM_Machine *tm, const char *input);

/** Выполняет один шаг;  @return true, если достигнуто q0 (завершение)     */
bool tm_step(TM_Machine *tm);

/** Запускает цикл ≤ max_steps; выводит конфигурацию после каждого шага.   */
void tm_run(TM_Machine *tm, int max_steps);

/** Печатает ленту, позицию головки и состояние                           */
void tm_print(const TM_Machine *tm);

#endif /* TURING_MACHINE_H */
