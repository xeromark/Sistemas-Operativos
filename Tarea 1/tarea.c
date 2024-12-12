#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//Tarea de sistemas operativos de Omar Marca

//Algoritmo de reglas:

//  Procesos == Jugadores
//  Zona de proteccion == Cola de procesos escuchando para tomar o pasar su turno
//  decir SOOOO en voz alta == el proceso tiene su propio espacio de tiempo para decir SOOOO
//  quedarse sin aliento == El proceso debe disminuir la vitalidad o resistencia de patadas que le quedan
//  Patadas == Los demas procesos necesitan ejecutarse pero no pueden hasta que el proceso vuelva a la "cola" 
//  El jugador toca a alguien == termino de decir SOOOO y sede el espacio a otro para decir SOOOO

int TurnRandPlayer(pid_t player, pid_t PlayerInGame[], int CountPlayerInGame){
    int turno = (rand() % CountPlayerInGame);

    if(player == PlayerInGame[turno]){
        turno = TurnRandPlayer(player, PlayerInGame,CountPlayerInGame);
    }

    return turno;


}

int ChangeTurn(int n, int anterior, pid_t player[], pid_t spectators[], int CountPlayerInGame){ // cambia el turno de los jugadores
                                                                        //Outgame corresponde a los jugadores que aun estan jugando
    pid_t PlayerInGame[CountPlayerInGame];
    int k = 0;
    for(int i = 0 ; i < n; i++){        //Este bloque de codigo lo que hace es filtrar por la NO interseccion entre los jugadores en el juego y los que estan expectando por haber perdido
        int check = 1;
        for(int j = 0 ; j < n; j++){
            if(player[i] == spectators[j]){ // si son distintos, el player i esta vivo
                check = 0;
                break;
            }

        }
        if(check == 1){ // si son distintos, el player i esta vivo
            PlayerInGame[k] = player[i];
            k++;
        }
    }
    
    int turn = TurnRandPlayer(player[anterior], PlayerInGame, CountPlayerInGame);   //retorna un numero random sin repetir el anterior

    for(int i = 0 ; i < n; i++){

        if(PlayerInGame[turn] == player[i] && PlayerInGame[turn] != 0){

            return i;
            break;
        }
    }

}


int SOOOO(){

    int DuracionAlientoSeg = rand() % 5 + 1; //Duracion del tiempo en que dice SOOOO

        sleep(1);
        printf("El jugador %d gritara %d segundos: \n", getpid(), DuracionAlientoSeg);

    for(int i = 0; i < DuracionAlientoSeg ; i++){
        printf("(seg %d) Jugador %d dice: SOOOO! \n", DuracionAlientoSeg - i, getpid());
        sleep(1);
    }

    return DuracionAlientoSeg;
}


int llegoAbase(int DuracionAlientoSeg){

    //Ahora verificamos si el jugador llega a la base. Reglas:
    //DuracionAlientoSeg >= Tiempo_De_llegar_base == Logra tocar a alguien
    //DuracionAlientoSeg < Tiempo_De_llegar_base == Hay que patearlo por no decir SOOOO ni haber llegado a la base a tiempo

    srand(time(NULL));
    int Tiempo_De_llegar_base = rand() % 5 + 1; // representa el tiempo en llegar a base, Se hizo de esta forma porque se concluye que si el jugador se le acaba el aire es porque no llego a tiempo a base y este valor dependera por ronda

    if(DuracionAlientoSeg >= Tiempo_De_llegar_base){
        return 1;
    }
    else{
        return 0;
    }

}


int main() {    system("clear");
    //Asumimos que el codigo principal es el padre, por ende es el unico quien tendra las referencias a los hijos
    pid_t Father = getpid(); // solo el padre puede hacer hijos
    int n = 2; //corresponde al numero de jugadores
    printf("Hola bienvenido al juego del SOOOO! \n");
    printf("Ingrese el numero de jugadores: ");
    scanf("%d", &n);

    int number_players = n; // Numnero de jugadores en juego
    int FatherStatus = 1; //estado del padre que es 1 cuando escribe y 0 cuando el padre escucha a sus hijos
    pid_t player[n];   //Separamos al padre y ponemos al primer jugador que si o si va
    pid_t spectators[n];    //Estos seran los expectadores

    int ftoc[n][2]; // Creamos la Pipe. Esta "tuberia" ira del padre hacia los hijos
    int ctof[n][2]; // Creamos la Pipe. Esta "tuberia" ira del hijo hacia el padre
    int canal;    //cada pipe tiene su propio canal para escuchar al padre.
    int Ronda = 1;

    for (int i = 0; i < n; i++) {
        spectators[i] = 0;
        if (pipe(ftoc[i]) == -1 || pipe(ctof[i]) == -1 ) {
            perror("Error al crear la tubería");
            exit(EXIT_FAILURE);
        }
    }


    for(int i = 0 ; i < n ; i++){
        if(Father == getpid()){
            player[i] = fork();
            canal = i;          //asignamos el numero del canal del hijo asociado al orden en el que fueron creados
            if (player[i] == -1) {
                perror("Error al crear el proceso hijo");
                exit(EXIT_FAILURE);
            }
        }
    }
    srand(time(NULL));

    if(Father == getpid()){        //proceso de comunicacion con pipes, proceso padre
        //presentacion
        for(int i = 0 ; i < n ; i++){ // Este For es para dar presentacion a los jugadores
            close(ctof[i][1]); // Cierra el extremo de escritura de la tubería
            write(ftoc[i][1], "", strlen("") + 1); // Escribe el mensaje en la tuberia por cada hijo

            char mensaje_recibido[100]; // Un buffer para almacenar el mensaje recibido
            read(ctof[i][0], mensaje_recibido, sizeof(mensaje_recibido));

        }
        //presentacion


        int turn = -2;

        while(1){


            if(FatherStatus == 1){


                if(spectators[turn] != player[turn] && turn != -2){   // para saber quien es el proximo que le toca, si es != (distinto) significa que no murio y aun no es expectador por lo tanto llego a tocar a alguien mas. Tambien le ponemos distinto de -2 para que no se produsca este anuncio en el primer turno
                    int turno_anterior = turn;
                    turn = ChangeTurn(n, turn, player, spectators, number_players);

                    printf("\n\n El jugador %d logro tocar a:  %d\n\n", player[turno_anterior], player[turn]);

                }
                else{
                    turn = ChangeTurn(n, turn, player, spectators, number_players);

                    printf("\n\n turno del jugador: %d\n\n", player[turn]);
                }

                printf("\n________________________\n");

                printf    ("----- Ronda: %d ------ \n", Ronda);

                printf("________________________\n\n\n");


                Ronda++;



                //Envia los datos
                char mensaje[10];
                sprintf(mensaje, "%d", player[turn]);   

                //printf(" enviando mensaje a hijo: %s \n", mensaje);

                write(ftoc[turn][1], mensaje, strlen(mensaje) + 1); // Escribe el mensaje en la tuberia por cada hijo

                FatherStatus = 0;
                //ChangeTurn(player);
            }
            else{                               

                char mensaje_recibido[100]; // Un buffer para almacenar el mensaje recibido
                read(ctof[turn][0], mensaje_recibido, sizeof(mensaje_recibido));

                if(strcmp(mensaje_recibido, "CambiarTurno") == 0){
                    FatherStatus = 1;

                }
                else{
                    for(int i = 0 ; i < n ; i++){
                        if(i != turn && spectators[i] != player[i]){ // spectators si tiene el mismo numero que players, quiere decir que ese proceso lloron ya dinalizo asi que no tiene que esperar nada de el

                            write(ftoc[i][1], "", strlen("") + 1); // Escribe el mensaje en la tuberia por cada hijo

                            char mensaje_recibido[100]; // Un buffer para almacenar el mensaje recibido
                            read(ctof[i][0], mensaje_recibido, sizeof(mensaje_recibido));

                            if(strcmp(mensaje_recibido, "Patear") == 0){
                                printf("%d ha recibido una patada de %d \n", player[turn], player[i]);
                                write(ftoc[turn][1], "Patear", strlen("Patear") + 1); // Escribe el mensaje en la tuberia de quien es el turno
                                char mensaje_recibido[100]; 
                                read(ctof[turn][0], mensaje_recibido, sizeof(mensaje_recibido));

                                if(strcmp(mensaje_recibido, "mori") == 0){
                                    spectators[turn] = player[turn];
                                    number_players--;
                                    FatherStatus = 1;


                                    if(number_players == 1){ // si el numero de jugadores es 1 se declara el ganador
                                        printf("\n\nEl ganador del juego del SOOOO es el jugador: %d \n", player[i]);
                                        write(ftoc[i][1], "ganador", strlen("ganador") + 1); // Escribe el mensaje en la tuberia de quien es el turno
                                        number_players--;
                                    }

                                    break;

                                }
                            }
                        }
                    }
                    FatherStatus = 1;
                }
            }

            if(number_players == 0){
                break;
            }
        }

        wait(NULL);
    }

//

    if(Father != getpid()){

        //sleep(rand() % 2);
        int Vitalidad = rand() % 4 + 1; // representa el numero de patadas que puede sorportar cada hijo. Tiene sentido porque normalmente 3 patadas bien dadas deja a cualquiera en el suelo.

        for(int i = 0 ; i < getpid()%10 ; i++){ //Este for se hace por un bug con los hijos que hacia que tuvieran la misma vida 
            Vitalidad = rand() % 3 + 1;
        }

        int DuracionAlientoSeg = 5; //Duracion del tiempo en que dice SOOOO

        printf("Hola! Soy el jugador %d con vitalidad: %d \n", getpid(), Vitalidad);


        while(1){

            char mensaje_recibido[100]; // Un buffer para almacenar el mensaje recibido
            read(ftoc[canal][0], mensaje_recibido, sizeof(mensaje_recibido));

            int id_mensaje = (int) strtol(mensaje_recibido, NULL, 10);

            if(getpid() == id_mensaje){
                //
                DuracionAlientoSeg = SOOOO();
                //

                //Momento de enviar el mensaje de que ya termino de hablar en lenguaje SOOOO               
                //Envia los datos
                if(llegoAbase(DuracionAlientoSeg) == 1){
                    write(ctof[canal][1], "CambiarTurno", strlen("CambiarTurno") + 1); // Escribe el mensaje en la tuberia por cada hijo

                }
                else{
                    write(ctof[canal][1], "OtraVezMiTurno", strlen("OtraVezMiTurno") + 1); // Escribe el mensaje en la tuberia por cada hijo

                }
            }
            else if(strcmp(mensaje_recibido, "Patear") == 0){ // que pasaria si el mensaje que llega es una patada?... pues habra que comprobarlo en la linea de abajo JAJAJAJA
                Vitalidad--;

                printf("El jugador %d tiene vitalidad: %d \n", getpid(), Vitalidad);


                if(Vitalidad <= 0){
                    printf("El jugador %d se pone a llorar... Se termina el proceso", getpid());
                    printf("\U0001F622\n"); // Imprime el emoji de calavera


                    write(ctof[canal][1], "mori", strlen("mori") + 1); // Escribe el mensaje en la tuberia por cada hijo
                    exit(EXIT_SUCCESS);

                }
                else{
                    write(ctof[canal][1], "vivo", strlen("vivo") + 1); // Escribe el mensaje en la tuberia por cada hijo

                }
            }
            else if(strcmp(mensaje_recibido, "ganador") == 0){
                exit(EXIT_SUCCESS);
                
            }
            else{ // Si yo no soy el que grita SOOOO le voy a dar sus pataditas al que NO diga SOOOO... si es que le llego a dar...
                  // Calculamos la probabilidad de dar un golpe


                if(rand() % 2){

                    write(ctof[canal][1], "Patear", strlen("Patear") + 1); // Escribe el mensaje en la tuberia por cada hijo
                }
                else{
                    write(ctof[canal][1], "none", strlen("none") + 1); // Escribe el mensaje en la tuberia por cada hijo

                }


            }


        }
        

    }


    return 0;
}