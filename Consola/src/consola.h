/*
 * consola.h
 *
 *  Created on: 21 abr. 2022
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>

#include "consolautils.h"

void checkFiles(t_config* config, FILE* file, t_log* logger);
t_instruccion* crear_instruccion(char* instruccion);
void log_instruccion(t_log* logger, t_instruccion* instruccionED);
void liberar_instruccion (char* instruccion, t_instruccion* instruccionED);
void terminar_programa(t_log* logger, t_config* config, FILE* file, t_paquete* paquete, int conexion);

#endif /* CONSOLA_H_ */
