#ifndef TURING_MACHINE_H
#define TURING_MACHINE_H

#include <stdbool.h>
#include <stdio.h>

#define cnMax 64

// Максимальное количество состояний и символов во всей машине
enum { 
    TM_MAX_STATE = cnMax, 
    TM_MAX_SYMBOL = cnMax 
};

// Переход: read → write, move, next_state
typedef struct {
    char read;      // символ, ожидаемый в текущей ячейке
    char write;     // символ, который нужно записать
    char move;      // 'L', 'R' или 'S'
    int next_state; // новый номер состояния 
} TM_Transition;

// Набор переходов для одного состояния
typedef struct {
    int count;                         // сколько переходов описано
    TM_Transition  trs[TM_MAX_SYMBOL]; // сами переходы
} TM_State;

// Узел двусвязной ленты
typedef struct TM_Cell {
    char sym;              // символ в ячейке
    struct TM_Cell *left;  // сосед слева
    struct TM_Cell *right; // сосед справа
} TM_Cell;

// Собственно машина Тьюринга
typedef struct {
    TM_State states[TM_MAX_STATE]; // система команд
    int state_qty;                 // сколько состояний загружено
    int cur_state;                 // текущее состояние
    TM_Cell *head;                 // текущая ячейка (головка)
    TM_Cell *leftmost;             // крайняя левая непустая
    TM_Cell *rightmost;            // крайняя правая непустая
    char blank;                    // cимвол пустой ячейки (‘_’)
    int step_counter;              // число уже выполненных шагов
} TM_Machine;

/*
 Подпрограмма для создания и заполнения машины Тьюринга
 Вход: 
    1) указатель на структуру, которую нужно инициализировать 
    2) путь к текстовому файлу, содержащему систему команд,
    3) символ, для считывания ячейкой ленты
 Выход: true - файл прочитан без ошибок, иначе - false
*/
bool tm_init(TM_Machine *tm, char *fname, char blank);

/*
 Подпрограмма для заммены текущей ленты машины на новое слово
 Вход: 
    1) указатель на уже инициализированную структуру 
    2) строка, которую нужно поместить на ленту
 Выход: (нет) – процедура
*/ 
void tm_load_tape(TM_Machine *tm, char *input);

/*
 Подпрограмма выполняет один переход машины
 Вход: 
    1) указатель на машину, над которой выполняется шаг
 Выход: true  – машина перешла в состояние q0, если выполнен обычный шаг - false
*/   
bool tm_step(TM_Machine *tm);

/*
 Подпрограмма запускает машину в автоматическом режиме
 Вход: 
    1) указатель на машину
    2) максимальное число шагов
 Выход: (нет) – процедура
*/  
void tm_run(TM_Machine *tm, int max_steps);

/*
 Подпрограмма отображает текущую конфигурацию машины
 Вход: 
    1) указатель на машину, конфигурацию которой нужно вывести
 Выход: (нет) – процедура
*/   
void tm_print(TM_Machine *tm);

#endif 