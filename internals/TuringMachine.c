#include "TuringMachine.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* --------------------------- служебные функции --------------------------- */

void tm_cell_expand_left(TM_Machine *tm) {
    TM_Cell *new_left = (TM_Cell *)malloc(sizeof(TM_Cell));
    new_left -> sym = tm -> blank;
    new_left -> left = NULL;
    new_left -> right = tm -> leftmost;
    tm -> leftmost -> left = new_left;
    tm -> leftmost = new_left;
}

void tm_cell_expand_right(TM_Machine *tm) {
    TM_Cell *new_right = (TM_Cell *)malloc(sizeof(TM_Cell));
    new_right -> sym   = tm -> blank;
    new_right -> right = NULL;
    new_right -> left  = tm -> rightmost;
    tm -> rightmost -> right = new_right;
    tm -> rightmost = new_right;
}

/* ------------------------- чтение системы команд ------------------------ */

bool add_transition(TM_State *st, TM_Transition tr) {
    bool ok = false;
    if (st -> count < TM_MAX_SYMBOL) {
        st -> trs[st -> count] = tr;
        st -> count += 1;
        ok = true;
    }
    return ok;
}

bool tm_init(TM_Machine *tm, char *fname, char blank) {
    int i = 0;
    while (i < TM_MAX_STATE) {
        tm -> states[i].count = 0;
        i++;
    }
    tm -> state_qty = 0;
    tm -> cur_state = 1;
    tm -> blank = blank;
    tm -> head = NULL;
    tm -> leftmost = NULL;
    tm -> rightmost = NULL;
    tm -> step_counter = 0;

    FILE *fp = fopen(fname, "r");
    bool ok = (fp != NULL);
    if (ok) {
        while (1) {
            int st = 0;
            char read = 0, write = 0, dir = 0;
            int next = 0;
            int res = fscanf(fp, "%d %c %c %c %d", &st, &read, &write, &dir, &next);
            if (res == EOF)  { 
                ok = true;  
                break; 
            }
            if (res != 5 || st >= TM_MAX_STATE || next >= TM_MAX_STATE) { 
                ok = false; 
                break; 
            }

            TM_Transition tr;
            tr.read = read; 
            tr.write = write; 
            tr.move = dir; 
            tr.next_state = next;
            if (!add_transition(&tm -> states[st], tr)) { 
                ok = false; 
                break; 
            }

            if (st + 1 > tm -> state_qty) { 
                tm -> state_qty = st + 1; 
            }
            if (next + 1 > tm -> state_qty) { 
                tm -> state_qty = next + 1; 
            }
        }
        fclose(fp);
    }
    return ok;
}

/* --------------------------- работа с лентой ---------------------------- */

void tm_load_tape(TM_Machine *tm, char *input) {
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

    size_t n = strlen(input);
    size_t k = 0;
    while (k < n) {
        TM_Cell *node = (TM_Cell *)malloc(sizeof(TM_Cell));
        node -> sym = input[k];
        node -> left = cur;
        node -> right = NULL;
        cur -> right = node;
        cur = node;
        k++;
    }
    TM_Cell *right = (TM_Cell *)malloc(sizeof(TM_Cell));
    right -> sym = tm -> blank;
    right -> right = NULL;
    right -> left = cur;
    cur -> right = right;

    tm -> rightmost = right;
    tm -> head = left -> right; 
    tm -> cur_state = 1;                
    tm -> step_counter = 0;
}

TM_Transition *find_transition(TM_Machine *tm, char sym) {
    TM_State *st = &tm -> states[tm -> cur_state];
    int idx = 0;
    TM_Transition *found = NULL;
    while (idx < st -> count) {
        if (st -> trs[idx].read == sym) {
            found = &st -> trs[idx];
            idx = st -> count;
        } else
            idx++;
    }
    return found;
}

bool tm_step(TM_Machine *tm) {
    bool halted = false;
    TM_Transition *tr = find_transition(tm, tm -> head -> sym);

    if (tr == NULL) {
        tm -> cur_state = 0;
    } else {
        tm -> head -> sym = tr -> write;
        if (tr -> move == 'L') {
            if (tm -> head -> left == NULL)  
                tm_cell_expand_left(tm);
            tm -> head = tm -> head -> left;
        } else if (tr -> move == 'R') {
            if (tm -> head -> right == NULL) 
                tm_cell_expand_right(tm);
            tm -> head = tm -> head -> right;
        }
        tm -> cur_state = tr -> next_state;
    }

    tm -> step_counter++;
    if (tm -> cur_state == 0) 
        halted = true;
    return halted;
}

void tm_print(TM_Machine *tm) {
    const int window = 30;
    const TM_Cell *p = tm -> head;
    int left_seen = 0;
    while (left_seen < window && p -> left != NULL) {
        p = p -> left;
        left_seen++;
    }

    const TM_Cell *cur = p;
    int pos = 0, head_pos = -1;
    while (cur != NULL && pos < 2 * window) {
        putchar(cur -> sym);
        if (cur == tm -> head) 
            head_pos = pos;
        cur = cur -> right;
        pos++;
    }
    putchar('\n');

    int i = 0;
    while (i < head_pos) { 
        putchar(' '); 
        i++; 
    }
    putchar('^'); 
    putchar('\n');

    printf("q%d, step %d\n\n", tm -> cur_state, tm -> step_counter);
}

void tm_run(TM_Machine *tm, int max_steps) {
    tm_print(tm);
    bool done = false;
    int  steps_done = 0;

    while (!done && steps_done < max_steps) {
        done = tm_step(tm);
        tm_print(tm);
        steps_done++;
    }

    if (done)
        printf("Машина завершила работу (q0).\n");
    else
        printf("Достигнут лимит %d шагов.\n", max_steps);
}