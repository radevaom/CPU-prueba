/*
 * cpuutils.h
 *
 *  Created on: 7 may. 2022
 *      Author: utnso
 */

#ifndef CPUUTILS_H_
#define CPUUTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/collections/list.h>

t_log* logger;

typedef enum
{
	MENSAJE,
	INSTRUCCIONES,
	INICIO_MEM,
	EXIT_MEM,
	CPU_EJECUCION,
	SUSPENDIDO_MEM,
	EXIT,
	PCB
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct
{
	char* id;
	uint8_t size_id;
	uint32_t* param;
	uint8_t size_param;
} t_instruccion;

typedef struct{
	uint32_t id;
	uint32_t size;
	void* bufferInstrucciones;
	uint32_t tamanioBuffer;
	uint32_t program_counter;
	uint32_t tabla_de_paginas;
	double estimacion_rafaga;
} process_pcb_t;

void* recibir_buffer(uint32_t*, int);

int iniciar_servidor(char*,char*);
int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
uint32_t recibir_tamanio_proceso(int);
int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(void);
t_paquete* crear_super_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
process_pcb_t* deserializar_pcb(int socket_cliente);
void iterator(t_instruccion* instruccion);
#endif /* CPUUTILS_H_ */
