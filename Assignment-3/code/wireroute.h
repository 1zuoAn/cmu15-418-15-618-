/**
 * Parallel VLSI Wire Routing via OpenMP
 * Name 1(andrew_id 1), Name 2(andrew_id 2)
 */

#ifndef __WIREOPT_H__
#define __WIREOPT_H__

#include <omp.h>

typedef struct { int index[5][2] = {0} ;int cnt = 0; } wire_t;

typedef int cost_t;
int init_cost(wire_t* wires,int cost[1000][1000],int num_of_wires, int dim_x, int dim_y);
void create_new(wire_t* wires, int num_of_wires);
int join_way(int x1, int y1, int x2, int y2);
int init(int** cost, int n);


const char *get_option_string(const char *option_name,
                              const char *default_value);
int get_option_int(const char *option_name, int default_value);
float get_option_float(const char *option_name, float default_value);

#endif
