#include "TuringMachine.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/* ---------- проверка входной двоичной строки ---------- */
static int is_binary(const char *s)
{
    int ok = 1, i = 0;
    if (s[0] == '\0') { ok = 0; }
    while (ok && s[i] != '\0') {
        if (s[i] != '0' && s[i] != '1') { ok = 0; }
        i++;
    }
    return ok;
}

int main(void)
{
    TM_Machine tm;
    if (!tm_init(&tm, "tm_table.txt", '_')) {
        printf("Ошибка чтения файла системы команд.\n");
        return 1;
    }

    int work = 1;
    while (work) {
        char buf[256];
        int valid = 0;
        while (!valid) {
            printf("Введите неотрицательное число в двоичной форме (или \"exit\"): ");
            if (scanf("%255s", buf) != 1) { work = 0; valid = 1; }
            else if (strcmp(buf, "exit") == 0) { work = 0; valid = 1; }
            else if (is_binary(buf)) { valid = 1; }
            else { printf("Ошибка: допустимы только символы 0 и 1.\n"); }
        }
        if (!work) { break; }

        tm_load_tape(&tm, buf);
        tm_run(&tm, 10000);
    }
    return 0;
}
