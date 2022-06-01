/*
 ============================================================================
 Name        : CPU.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "CPU.h"

int main(void)
{




	//t_log* logger;
	t_config* config;

	logger = iniciar_logger();
	logger = log_create("CPU.log","CPU says: ",1,LOG_LEVEL_DEBUG);

	if ((config = config_create("src/config"))==NULL){
		printf("Error leyendo configuración. \n");
	};

	entradas_tlb=config_get_int_value(config, "ENTRADAS_TLB");
	REEMPLAZO_TLB=config_get_string_value(config,"REEMPLAZO_TLB");
	RETARDO_NOOP=config_get_double_value(config, "RETARDO_NOOP");
	ipMemoria = config_get_string_value(config,"IP_MEMORIA");
	puertoMemoria = config_get_string_value(config,"PUERTO_MEMORIA");
	puertoCPUDispatch = config_get_string_value(config,"PUERTO_CPU_DISPATCH");
	puertoCPUInterrupt= config_get_string_value(config,"PUERTO_CPU_INTERRUPT");




	//Lo primero que hacemos es conectarnos con la memoria y hacer el Handshake incial
/*
	conexionMemoria = crear_conexion(ipMemoria, puertoMemoria);
	int handshakeMemo = 1; //ver que handshake le envio a la memoria
	int handshakeMemoResult;
	send(conexionMemoria, &handshakeMemo, sizeof(int), NULL);
	recv(conexionMemoria, &handshakeMemoResult, sizeof(int), MSG_WAITALL);
	if (handshakeMemoResult == 0 ){
		log_info(logger,"conexion a la memoria exitosa");
	}
	else {
		log_info(logger,"error en la conexion a la memoria");
	}
*/
	//va int servidor_dispatch = iniciar_servidor(ipMemoria,puertoCPUDispatch);
	char* ip="127.0.0.1";
	char * puerto = "8000";

	prueba_consola = iniciar_servidor(ip,puerto);


	log_info(logger, "cpu espera consola");

	while(1){
			//va int socket_cliente = esperar_cliente(servidor_dispatch);


			pthread_t thread;
		    pthread_create(&thread, NULL, instruccion, NULL);
		    pthread_detach(thread);
		}


	terminar_programa(conexionMemoria, logger, config);

}

void instruccion(){
	int socket_cliente = esperar_cliente(prueba_consola);

	bool validarWhile = true;
	//int puerto= *(int*)socket_cliente;
		while(validarWhile){
			t_paquete* paquete=crear_paquete_pcb();
		process_pcb_t* proceso = malloc(sizeof(process_pcb_t));
		int codigo_operacion = recibir_operacion(socket_cliente);
		switch(codigo_operacion){
			case MENSAJE:
				log_info(logger, "entra en mensaje");
				//proceso -> size = recibir_tamanio_proceso(socket_cliente);
				//log_info(logger, (char*) proceso -> size);
				break;
			/*case PAQUETE:
				log_info(logger, "Entro en paquete");
				paquete = recibir_paquete(socket_cliente);
				//proceso->lista= paquete;
				//proceso->id = generarID();
				log_info(logger, "Recibio paquete en CPU");

				break;
				*/
			case INSTRUCCIONES:

				proceso->id=1;
				proceso->size=2;
				proceso->bufferInstrucciones = recibir_buffer(&(proceso->tamanioBuffer),socket_cliente);
				proceso->program_counter=3;
				proceso->tabla_de_paginas=4;
				proceso->estimacion_rafaga=5;
				t_list* listaInstrucciones = deserializar_instrucciones(proceso->tamanioBuffer,proceso->bufferInstrucciones);
				list_iterate(listaInstrucciones, (void*) iterator);

				agregar_a_paquete_pcb(paquete,proceso);
				enviar_paquete(paquete,socket_cliente);
 				 //process_pcb_t* pcb = deserializar_pcb(socket_cliente);
 				/*log_info(logger, "Me llegaron los siguientes valores:\n");
 				list_iterate(pcb -> list, (void*) iterator);
 				pcb->program_counter++;*/
				//free(proceso->bufferInstrucciones);
 	 	 	 	//free(proceso);
 	 	 	 	validarWhile = false;
 	 	 	 	break;
			case -1:
				log_info(logger,"error");
				validarWhile = false;
				break;
			default:
				log_info(logger,"salio");
				break;
		}
	}

}

void iterator(t_instruccion* instruccion) {
	log_info(logger,"%s", instruccion->id);
	if (instruccion->size_param >0){
		for(int i=0;i<instruccion->size_param;i++){
			log_info(logger,"%d", instruccion->param[i]);
		}


	}
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config;

	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* leido;


	leido = readline("> ");


	while (strcmp(leido,"")!=0){
			log_info(logger,leido);
			free(leido);
			leido = readline("> ");

		}


	free(leido);
}

void paquete(int conexion)
{
/*
	char* leido;
	t_paquete* paquete;
	paquete = crear_paquete();

	leido = readline("> ");
	while (strcmp(leido,"")!=0){
				agregar_a_paquete(paquete,leido,strlen(leido)+1);
				free(leido);
				leido = readline("> ");

			}

	enviar_paquete(paquete,conexion);
	eliminar_paquete(paquete);
	free(leido);
*/
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{

	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
}
