#include"header.h"
#include"sem.c"
int main(int argc,char *argv[])
{
if(argc !=2)
{
	printf("Zla liczba argumentow\nWywolaj ./producent we.txt\n\n");
	exit(1);
}
char znak;
char *adres_pamieci_wspolnej;
int id_segmentu;
key_t key_seg = ftok(".",'W');
FILE *we;
//utworzenie segmentu pamieci dzielonej
if((id_segmentu = shmget(key_seg,sizeof(char),IPC_CREAT|IPC_EXCL|0600)) == -1)
{
	if((id_segmentu = shmget(key_seg,sizeof(char),IPC_CREAT | 0600 )) == -1)
	{
		printf("BLAD uzyskania dostępu shmget: %i (%s)\n",errno,strerror(errno));
    	exit(EXIT_FAILURE);
	}
	else
	{
		printf("PRODUCENT: uzyskanie dostepu do segmentu pamieci wspolnej\n");
		uzyskanie_dostepu();
	}
}
else
{
	printf("PRODUCENT:  utworzenie segmantu pamieci wspolnej\n");
	utworz_nowy_semafor();
}

adres_pamieci_wspolnej =(char *) shmat(id_segmentu,0,0); //0->system decyduje o adresie i 0-> do odczytu i zapisu
if(*adres_pamieci_wspolnej == -1)
{
    printf("BLAD shmat(przylaczenie) : %i (%s)\n",errno,strerror(errno));
    exit(EXIT_FAILURE);
}
else
{
    printf("PRODUCENT: uzyskano adres segmantu pamieci wspolnej\n");
}


//ustawienie semaforow
ustaw_semafor(0,1); // semafor zapisu na wart 1
ustaw_semafor(1,0);//semafor odczytu na wart 0
//utworzenie uchwytu pliku do czytania
we = fopen(argv[1],"r");

if(we != NULL && (znak=fgetc(we)) != EOF )
{
		fseek(we,0,SEEK_SET); //przesuniecie na poczatek pliku po ifie
		while((znak = fgetc(we)) != EOF)
		{
		//opuszczenie semafora zapisu
		semafor_p(0);
		//sleep(3);
		*adres_pamieci_wspolnej = znak;
		printf("%c %c",znak,*adres_pamieci_wspolnej);
		//podniesienie semafora odczytu
	    semafor_v(1);
		//sleep(3);
		}
	semafor_p(0);
	*adres_pamieci_wspolnej = EOF;
	//podniesienie semafora odczytu
	semafor_v(1);
	//sleep(3);
}
else
{
    printf("PRODUCENT: plik pusty (EOF) lub sie nie otworzyl\n");
	fclose(we);
	if(shmdt(adres_pamieci_wspolnej) == -1)
	{
		printf("Blad odlaczania pamieci: %i (%s)\n",errno,strerror(errno));
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Odlaczono pamiec wspolna\n");
	}
	if(shmctl(id_segmentu,IPC_RMID,0) == -1)
	{
		printf("Blad usuniecia segmentu pamieci: %i (%s)\n",errno,strerror(errno));
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Usunieto segment pamieci\n");
	}
	usun_semafor();
	return 0;
}
//zamkniecie pliku
fclose(we);
//odlaczenie pamieci dzielonej
if((shmdt(adres_pamieci_wspolnej)) == -1 )
{
	printf("Blad odlaczenia pamieci: %i (%s)\n",errno,strerror(errno));
	exit(EXIT_FAILURE);
}
else
{
	printf("PRODUCENT: pamiec odlaczona\n");
}

return 0;
}
