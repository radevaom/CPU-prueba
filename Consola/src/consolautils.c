/*
 * utils.c
 *
 *  Created on: 21 abr. 2022
 *      Author: utnso
 */

#include "consolautils.h"

#include<commons/log.h>

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

int crear_conexion(char *ip, char* puerto, t_log* logger)
 {
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		log_error(logger, "No se pudo conectar con el servidor");
	}
	else{
		log_info(logger, "Se realizó la conexión con el servidor");
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = INSTRUCCIONES;
	crear_buffer(paquete);
	return paquete;
}

void enviar_mensaje(char* mensaje, int socket_server)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_server, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}


void agregar_a_paquete(t_paquete* paquete, t_instruccion* instruccion)
{
	int tamanio = strlen(instruccion->id) + 1
				+ sizeof(uint8_t)*2
				+ sizeof(uint32_t) * instruccion->size_param;

	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio);

	memcpy(paquete->buffer->stream  + paquete->buffer->size, &(instruccion->size_param), sizeof(uint8_t));
	paquete->buffer->size += sizeof(uint8_t);

	if(instruccion->size_param != 0)
	{
		memcpy(paquete->buffer->stream + paquete->buffer->size, instruccion->param, sizeof(uint32_t) * instruccion->size_param);
		paquete->buffer->size += sizeof(uint32_t) * instruccion->size_param;
	}

	memcpy(paquete->buffer->stream + paquete->buffer->size, &(instruccion->size_id), sizeof(uint8_t));
	paquete->buffer->size += sizeof(uint8_t);

	memcpy(paquete->buffer->stream + paquete->buffer->size, instruccion->id, instruccion->size_id);
	paquete->buffer->size += instruccion->size_id;
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
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
