/*
 * cpuutils.c
 *
 *  Created on: 7 may. 2022
 *      Author: utnso
 */


#include "cpuutils.h"

t_paquete* crear_paquete_pcb(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PCB;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete_pcb(t_paquete* paquete, process_pcb_t* pcb){

	int tamanio = sizeof(uint32_t)*5
				+ sizeof(double)
				+ pcb->tamanioBuffer;

	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio);

	memcpy(paquete->buffer->stream + paquete->buffer->size, &(pcb->id), sizeof(uint32_t));
	paquete->buffer->size+=sizeof(uint32_t);

	memcpy(paquete->buffer->stream + paquete->buffer->size, &(pcb->size), sizeof(uint32_t));
	paquete->buffer->size+=sizeof(uint32_t);



	memcpy(paquete->buffer->stream + paquete->buffer->size, &(pcb->program_counter), sizeof(uint32_t));
	paquete->buffer->size+=sizeof(uint32_t);

	memcpy(paquete->buffer->stream + paquete->buffer->size, &(pcb->tabla_de_paginas), sizeof(uint32_t));
	paquete->buffer->size+=sizeof(uint32_t);

	memcpy(paquete->buffer->stream + paquete->buffer->size, &(pcb->estimacion_rafaga), sizeof(double));
	paquete->buffer->size+=sizeof(double);

	memcpy(paquete->buffer->stream + paquete->buffer->size, &(pcb->tamanioBuffer), sizeof(uint32_t));
		paquete->buffer->size+=sizeof(uint32_t);
		//log_info(logger,"%d ",&(pcb->tamanioBuffer));
		memcpy(paquete->buffer->stream  + paquete->buffer->size, pcb->bufferInstrucciones, pcb->tamanioBuffer);
		paquete->buffer->size += pcb->tamanioBuffer;
	free(pcb->bufferInstrucciones);
	free(pcb);
}

process_pcb_t* deserializar_pcb(int socket_cliente) {
	int size;
	int desplazamiento = 0;
	void * buffer = recibir_buffer(&size,socket_cliente);
	process_pcb_t* pcb = malloc(sizeof(process_pcb_t));



    memcpy(&(pcb->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(pcb->size), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);


    memcpy(&(pcb->tamanioBuffer), buffer + desplazamiento, sizeof(uint32_t));
       desplazamiento += sizeof(uint32_t);
       pcb->bufferInstrucciones = malloc(pcb->tamanioBuffer);
       memcpy(pcb->bufferInstrucciones,buffer + desplazamiento,pcb->tamanioBuffer);

    memcpy(&(pcb->program_counter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(pcb->tabla_de_paginas), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(pcb->estimacion_rafaga), buffer + desplazamiento, sizeof(double));
    desplazamiento += sizeof(double);



    return pcb;
}

t_list* deserializar_instrucciones(uint32_t tamanioBuffer,void* buffer)
{

	int desplazamiento = 0;
	t_list* instrucciones = list_create();
	int tamanio;

	while(desplazamiento < tamanioBuffer){
		t_instruccion* instruccion = malloc(sizeof(t_instruccion));

		memcpy(&(instruccion->size_param),buffer+desplazamiento,sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		if(instruccion->size_param !=0){
			instruccion->param = malloc(sizeof(uint32_t)* instruccion->size_param);
			memcpy(instruccion->param,buffer+desplazamiento,sizeof(uint32_t)* instruccion->size_param);
			desplazamiento += sizeof(uint32_t)* instruccion->size_param;
		}

		memcpy(&(instruccion->size_id),buffer+desplazamiento,sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->id = malloc(sizeof(instruccion->size_id));
		memcpy(instruccion->id, buffer+desplazamiento,instruccion->size_id);
		desplazamiento += instruccion->size_id;

		list_add(instrucciones,instruccion);

	}

	return instrucciones;
}


void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = 0;
	socket_cliente = socket(server_info->ai_family,
	                    server_info->ai_socktype,
	                    server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}


void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_super_paquete(void)
{
	//me falta un malloc!
	t_paquete* paquete;

	//descomentar despues de arreglar
	//paquete->codigo_operacion = PAQUETE;
	//crear_buffer(paquete);
	return paquete;
}

t_paquete* crear_paquete(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = INSTRUCCIONES;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

int iniciar_servidor(char* ip, char* puerto)
{


	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &servinfo);

	/*
	socket_servidor = socket(servinfo->ai_family,
	                         servinfo->ai_socktype,
	                         servinfo->ai_protocol);
	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	*/
	for (p=servinfo; p != NULL; p = p->ai_next)
		{
			if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
					== -1)
				continue;

			if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
				close(socket_servidor);
				continue;
			}
			break;
		}

	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	// Quitar esta l??nea cuando hayamos terminado de implementar la funcion
	//assert(!"no implementado!");

	// Aceptamos un nuevo cliente
	int socket_cliente;
	socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(uint32_t* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(uint32_t), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}


uint32_t recibir_tamanio_proceso(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	return atoi(buffer);
}
