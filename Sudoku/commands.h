#ifndef UNTITLED5_COMMANDS_H
#define UNTITLED5_COMMANDS_H

void solve(char * fileName);
void edit(char * fileName);
void mark_errors(int x);
void print_board();
int set(int y, int x, int z);
void undo ();
void redo ();
void autofill ();
void reset ();
void generate(int x, int y, int iteration);
int save(char * fileName);
void hint (int x, int y);
int validate();
void num_solutions ();

#endif
