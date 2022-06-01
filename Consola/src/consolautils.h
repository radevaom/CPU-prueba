/*
 * utils.h
 *
 *  Created on: 21 abr. 2022
 *      Author: utnso
 */

#ifndef CONSOLAUTILS_H_
#define CONSOLAUTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/collections/list.h>

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

typedef struct{
	uint32_t id;
	uint32_t size;
	void* bufferInstrucciones;
	uint32_t tamanioBuffer;
	uint32_t program_counter;
	uint32_t tabla_de_paginas;
	double estimacion_rafaga;
} process_pcb_t;

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

t_log* logger;


void* recibir_buffer(int*, int);
process_pcb_t* deserializar_pcb(int socket_cliente);
int recibir_operacion(int);
void iterator(t_instruccion* instruccion);
int crear_conexion(char* ip, char* puerto, t_log* logger);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(void);
void* serializar_paquete(t_paquete* paquete, int bytes);
void agregar_a_paquete(t_paquete* paquete, t_instruccion* instruccion);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void enviar_mensaje(char* mensaje, int socket_server);

#endif /* CONSOLAUTILS_H_ */
