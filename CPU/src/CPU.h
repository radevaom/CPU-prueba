/*
 * CPU.h
 *
 *  Created on: 7 may. 2022
 *      Author: utnso
 */

#ifndef CPU_H_
#define CPU_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<commons/collections/list.h>
#include<pthread.h>
#include "cpuutils.h"

typedef struct{

}list_instructions;





//CONFIG
int entradas_tlb;
double RETARDO_NOOP;
char* REEMPLAZO_TLB;
int conexionMemoria;
int conexionDispatch;
int conexionInterrupt;
char* puertoMemoria;
char* ipMemoria;
char* puertoCPUDispatch;
char* puertoCPUInterrupt;
int prueba_consola;

t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void leer_consola(t_log*);
void paquete(int);
void terminar_programa(int, t_log*, t_config*);
void iterator(t_instruccion* instruccion);
void instruccion();
#endif /* CPU_H_ */
