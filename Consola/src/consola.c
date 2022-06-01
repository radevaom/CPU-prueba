/*
 ============================================================================
 Name        : consola.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "consola.h"

int main(int argc, char** argv) {

	logger = log_create("consola.log", "Consola", 1, LOG_LEVEL_INFO);
	t_config* config = config_create("consola.config");
	FILE* file = fopen("pseudocodigo.txt", "r");
	checkFiles(config, file, logger);

	int conexion = crear_conexion(config_get_string_value(config, "IP"), config_get_string_value(config, "PUERTO"), logger);
	t_paquete* paquete = crear_paquete();


	char caracter;
	char* instruccion;
	t_instruccion* instruccionED;

	printf("\n %d \n", atoi(argv[2]));

	//enviar_mensaje(argv[2],conexion);

	fread(&caracter, sizeof(char), 1 , file);

	while(!feof(file))
	{
		instruccion = malloc(sizeof(char));
		strcpy(instruccion, "");

		while (caracter != '\n' && !feof(file))
		{
			size_t len = strlen(instruccion);

			instruccion = realloc(instruccion, len +1 +1);
			instruccion[len] = caracter;
			instruccion[len + 1] = '\0';

			fread(&caracter, sizeof(char), 1 , file);
		}

		instruccionED = crear_instruccion(instruccion);

		if(string_contains(instruccion,"NO_OP"))
		{
			int i = instruccionED->param[0] -1;
			instruccionED->size_param = 0;
			free(instruccionED->param);

			while(i != 0)
			{
				agregar_a_paquete(paquete, instruccionED);
				log_instruccion(logger, instruccionED);
				i--;
			}
		}

		agregar_a_paquete(paquete, instruccionED);
		log_instruccion(logger, instruccionED);
		liberar_instruccion(instruccion, instruccionED);

		fread(&caracter, sizeof(char), 1 , file);
	}

	enviar_paquete(paquete, conexion);
	process_pcb_t* proceso = malloc(sizeof(process_pcb_t));
	int codigo_operacion = recibir_operacion(conexion);
	switch(codigo_operacion){
		case PCB:
		log_info(logger,"soy la consola, recibi pcb, ahora deserializo para corroborar");
		proceso = deserializar_pcb(conexion);
		log_info(logger,"%d",proceso->id);
		log_info(logger,"%d",proceso->size);
		log_info(logger,"%d",proceso->program_counter);
		log_info(logger,"%d",proceso->tabla_de_paginas);
		log_info(logger,"%f",proceso->estimacion_rafaga);

		t_list* listaInstrucciones = deserializar_instrucciones(proceso->tamanioBuffer,proceso->bufferInstrucciones);
		list_iterate(listaInstrucciones, (void*) iterator);
		free(proceso->bufferInstrucciones);
		free(proceso);
	}

	terminar_programa(logger, config, file, paquete, conexion);



   return 0;
}




void iterator(t_instruccion* instruccion) {
	log_info(logger,"%s", instruccion->id);
	if (instruccion->size_param >0){
		for(int i=0;i<instruccion->size_param;i++){
			log_info(logger,"%d", instruccion->param[i]);
		}


	}
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

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
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




    memcpy(&(pcb->program_counter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(pcb->tabla_de_paginas), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(pcb->estimacion_rafaga), buffer + desplazamiento, sizeof(double));
    desplazamiento += sizeof(double);

    memcpy(&(pcb->tamanioBuffer), buffer + desplazamiento, sizeof(uint32_t));
           desplazamiento += sizeof(uint32_t);
           pcb->bufferInstrucciones = malloc(pcb->tamanioBuffer);
           memcpy(pcb->bufferInstrucciones,buffer + desplazamiento,pcb->tamanioBuffer);


    return pcb;
}


void checkFiles(t_config* config, FILE* file, t_log* logger)
{
	if(config == NULL){
		log_error(logger, "El archivo de configuración no pudo ser abierto");
	}
	else {
		log_info(logger, "El archivo de configuración se abrió correctamente");
	}

	if(file == NULL){
		log_error(logger, "El archivo de pseudocódigo no pudo ser abierto");
	}
	else {
		log_info(logger, "El archivo de pseudocódigo se abrió correctamente");
	}
}

t_instruccion* crear_instruccion (char* instruccion)
{
	char** tmp = string_split(instruccion, " ");

	t_instruccion* instruccionED = malloc(sizeof(t_instruccion));

	instruccionED->size_id = (uint8_t) strlen(tmp[0]) + 1;
	instruccionED->id = malloc(instruccionED->size_id);
	strcpy(instruccionED->id, tmp[0]);

	instruccionED->size_param = 0;

	if(tmp[1] != NULL)
	{
		instruccionED->param = malloc(sizeof(uint32_t));
		int i = 0;

		while(tmp[i+1] != NULL)
		{
			instruccionED->param = realloc(instruccionED->param, sizeof(uint32_t) * (i+1));
			instruccionED->param[i] = atoi(tmp[i+1]);
			i++;
		}
		instruccionED->size_param = i;
	}

	string_array_destroy(tmp);

	return instruccionED;
}

void log_instruccion (t_log* logger, t_instruccion* instruccionED)
{
	log_info(logger, "%s ", instruccionED->id);

	for(int i=0; i < instruccionED->size_param; i++)
	{
		log_info(logger, "Parámetro %d: %d ", i, instruccionED->param[i]);
	}

	log_info(logger, "Cantidad de parámetros: %d\n", instruccionED->size_param);
}

void liberar_instruccion (char* instruccion, t_instruccion* instruccionED)
{
	if(instruccionED->size_param != 0){ free(instruccionED->param); }
	free(instruccionED->id);
	free(instruccionED);
	free(instruccion);
}

void terminar_programa(t_log* logger, t_config* config, FILE* file, t_paquete* paquete, int conexion)
{
	log_info(logger, "Finalizando consola");
	log_destroy(logger);
	config_destroy(config);
	fclose(file);
	eliminar_paquete(paquete);
	liberar_conexion(conexion);
}
