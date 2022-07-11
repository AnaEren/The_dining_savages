/*
Nome: Ana Ellen Deodato Pereira da Silva
Data: 10/07/2022
Objetivo: Solução para o problema "The dining savages"

Foram usados um mutex:
* panela: pra so um selvagem poder ter acesso à panela de cada vez (variavel global);
e dois semáforos:
* panela_vazia: um selvagem avisa ao cozinheiro que a panela esta vazia, que estava esperando por este sinal para cozinhar.
* panela_cheia: o cozinheiro avisa ao selvagem que o estava esperando para poder comer que a panela está cheia.
obs.: nao ocorre de outro selvagem ter acesso à panela enquanto o missionário cozinha, pois o unico que tem acesso à panela é o selvagem quem foi falar com o cozinheiro;
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define QNT_SELVAGENS 5
#define PORCOES_PANELA 7

// variaveis globais
pthread_mutex_t _panela = PTHREAD_MUTEX_INITIALIZER; // mutex
sem_t panela_cheia, panela_vazia; // semáforos
int _porcoes;


void * selvagem(void *id){ 

  while(1){

    // espera dar um pouco de fome (5s maximo)
    sleep(random()%5);

    // vai até a panela (um de cada vez)
    pthread_mutex_lock(&_panela);

    // verifica se tem comida
    if(_porcoes > 0){
      // se sim, se serve
      _porcoes --;
      printf(" O selvagem %ld se serviu. Agora temos %d porções.\n", (long int)id, _porcoes); fflush(stdout);
    }
    else{
      // senao, acorda o cozinheiro
      printf("Não tem mais comida! O selvagem %ld vai acordar o cozinheiro... \n", (long int)id); fflush(stdout);
      sem_post(&panela_vazia);

      // aguarda o missionario cozinhar
      sem_wait(&panela_cheia);

      sleep(1);
      // agora pode comer
      _porcoes --;
      printf(" O selvagem %ld se serviu. Agora temos %d porções.\n", (long int)id, _porcoes); fflush(stdout);

    }

    // libera a panela para os outros
    pthread_mutex_unlock(&_panela);

    // tira um tempo para comer (10s maximo)
    sleep(random()%10);
  }
}


void * cozinheiro(){

  while(1){
    // dorme ate avisarem que a panela esta vazia
    sem_wait(&panela_vazia);

    // enche a panela
    _porcoes = PORCOES_PANELA;
    printf("A Panela está cheia!\n");

    sem_post(&panela_cheia);
  }
}


int main(){

  pthread_t t_selvagem[QNT_SELVAGENS], t_cozinheiro;
  _porcoes = 0;

  printf("Iniciando o banquete! \nA panela aguenta %d porções para %d selvagens.\n", PORCOES_PANELA, QNT_SELVAGENS);

  // iniciando os semaforos e verificando se nao hoveram erros
	if (sem_init(&panela_cheia, 0, 0) < 0) {
		exit(1);
	}
	if (sem_init(&panela_vazia, 0, 1) < 0) {
		exit(1);
	}

  // iniciando a tribo e verificando se nao houveram erros 
	if (pthread_create(&t_cozinheiro, NULL, cozinheiro, NULL)){
		exit(1);
	}

  for(int i=0; i<QNT_SELVAGENS; i++){
    if (pthread_create(&t_selvagem[i], NULL, selvagem, (void *)(long int)(i+1))){
      exit(1);
    }
  }


  // esperando o retorno das threads
	pthread_join(t_cozinheiro, NULL);
  for(int i=0; i<QNT_SELVAGENS; i++){
	  pthread_join(t_selvagem[i], NULL);
  }


  // destruindo os semaforos
	sem_destroy(&panela_cheia);
	sem_destroy(&panela_vazia);

  return (0);
}
