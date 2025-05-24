#ifndef TURING_MACHINE_H
#define TURING_MACHINE_H

#include <stdbool.h>
#include <stdio.h>

/**
 * @def cnMax
 * @brief Максимальное количество состояний *или* символов ленты,
 * поддерживаемое реализацией.
 */
#define cnMax 256

/**
 * @brief Пределы таблиц машины.
 */
enum {
   MaxState  = cnMax,  /**< Макс. число состояний               */
   MaxSymbol = cnMax   /**< Макс. число различных символов ленты*/
};

/**
 * @brief Описание перехода: «read → write, move, nextState».
 */
typedef struct {
   char read;     /**< Символ, ожидаемый под головкой               */
   char write;    /**< Символ, который нужно записать               */
   char move;     /**< Направление движения: `'L'`, `'R'` или `'S'` */
   int nextState; /**< Номер следующего состояния                   */
} TM_Transition;

/**
 * @brief Набор всех переходов одного состояния.
 */
typedef struct {
   int count;                     /**< Сколько переходов описано  */
   TM_Transition trs[MaxSymbol];  /**< Переходы данного состояния */
} TM_State;

/**
 * @brief Узел двусвязной ленты.
 */
typedef struct TM_Cell {
   char sym;              /**< Символ в ячейке        */
   struct TM_Cell *left;  /**< Соседняя ячейка слева  */
   struct TM_Cell *right; /**< Соседняя ячейка справа */
} TM_Cell;

/**
 * @brief Полная структура экземпляра машины Тьюринга.
 */
typedef struct {
   TM_State states[MaxState]; /**< Таблицы переходов                */
   int stateQty;              /**< Фактическое количество состояний */
   int curState;              /**< Текущее состояние                */
   TM_Cell *head;             /**< Текущая ячейка «под головкой»    */
   TM_Cell *leftmost;         /**< Крайняя левая непустая ячейка    */
   TM_Cell *rightmost;        /**< Крайняя правая непустая ячейка   */
   char blank;                /**< Символ «пусто» (обычно '_')      */
   int stepCount;             /**< Счётчик сделанных шагов          */
} TM_Machine;

/**
 * @brief Инициализирует машину и загружает таблицу переходов из текстового файла.
 *
 * @param tm Указатель на структуру машины, которую нужно заполнить.
 * @param fname Путь к текстовому файлу с таблицей переходов.
 * @param blank Символ, обозначающий пустую ячейку ленты.
 * @return `true`, если файл успешно прочитан, иначе `false`.
 */
bool tmInit(TM_Machine *tm, char *fname, char blank);

/**
 * @brief Заменяет текущую ленту новым словом.
 *
 * @param tm Уже инициализированная машина.
 * @param input Нуль-терминированная строка, которая будет помещена на ленту.
 */
void tmLoadTape(TM_Machine *tm, char *input);

/**
 * @brief Ищет переход по текущему символу.
 *
 * @param tm Машина.
 * @param sym Символ под головкой.
 * @return Указатель на найденный переход или `NULL`.
 */
TM_Transition *findTransition(TM_Machine *tm, char sym);

/**
 * @brief Выполняет один шаг машины.
 *
 * @param tm Указатель на машину.
 * @return `true`, если машина перешла в состояние `q0` (останов), иначе `false`.
 */
bool tmStep(TM_Machine *tm);

/**
 * @brief Запускает машину в непрерывном режиме печати конфигураций.
 *
 * @param tm Машина, которую нужно запустить.
 * @param maxSteps Максимальное число шагов до принудительной остановки.
 */
void tmRun(TM_Machine *tm, int maxSteps);

/**
 * @brief Печатает текущую конфигурацию (окно ленты, состояние, счётчик шагов).
 *
 * @param tm Машина, конфигурацию которой нужно вывести.
 */
void tmPrint(TM_Machine *tm);

/**
 * @brief Освобождает всю динамически выделенную память ленты.
 *
 * @param tm Машина, чью ленту нужно очистить.
 */
void tmFree(TM_Machine *tm);

/**
 * @brief Проверяет, что строка содержит непустое двоичное число.
 *
 * @param str Нуль-терминированная строка.
 * @return `true`, если строка непуста и состоит только из '0' и '1'.
 */
bool isBinary(char *str);

/**
 * @brief Расширяет ленту пустой ячейкой слева.
 *
 * @param tm Машина, у которой головка вышла за левый край.
 */
void tmCellExpandL(TM_Machine *tm);

/**
 * @brief Расширяет ленту пустой ячейкой справа.
 *
 * @param tm Машина, у которой головка вышла за правый край.
 */
void tmCellExpandR(TM_Machine *tm);

/**
 * @brief Добавляет переход в таблицу состояния.
 *
 * @param st Указатель на состояние, в которое нужно добавить правило.
 * @param tr Структура перехода.
 * @return `true`, если переход успешно добавлен, иначе `false`.
 */
bool addTransition(TM_State *st, TM_Transition tr);

/**
 * @brief Запускает интерактивный режим: ввод слова → запуск → вывод результата.
 *
 * Завершение по команде **exit** или по `EOF`.
 *
 * @param tm Экземпляр машины.
 * @param maxSteps Ограничение на количество шагов для каждой сессии.
 */
void tm_run(TM_Machine *tm, int maxSteps);

#endif
