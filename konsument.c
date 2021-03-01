
#include"header.h"
#include"sem.c"
int main(int argc,char *argv[])
{
if(argc !=2)
{
	printf("Zla liczba argumentow\nWywolaj ./konsument wy.txt");
	exit(1);
}
char znak;
char *adres_pamieci_wspolnej;
int id_segmentu;
key_t key_seg = ftok(".",'W');
FILE *wy;
//uzyskanie dostepu do segmantu pamieci dzielonej
if((id_segmentu = shmget(key_seg,sizeof(char),0600 | IPC_CREAT | IPC_EXCL)) == -1)
{
	if((id_segmentu = shmget(key_seg,sizeof(char), 0600 | IPC_CREAT)) == -1)
	{
    	printf("BLAD uzyskania dostepu shmget: %i (%s)\n",errno,strerror(errno));
    	exit(EXIT_FAILURE);
	}
	else
	{
		printf("Uzyskanie dostępu do segmentu pamieci wspolnej\n");
		uzyskanie_dostepu();
	}
}
else
{
    printf("KONSUMENT: utworzono segmant pamieci wspolnej\n");
	utworz_nowy_semafor();
}

adres_pamieci_wspolnej = (char *)shmat(id_segmentu,0,0);
if(*adres_pamieci_wspolnej == -1)
{
    printf("BLAD shmat z przydzieleniem adresu : %i (%s)\n",errno,strerror(errno));
    exit(EXIT_FAILURE);
}
else
{
    printf("KONSUMENT: uzyskano adres segmantu pamieci wspolnej\n");
}
//uzyskanie dostepu do zbioru semaforow

//utworzenie uchwytu pliku do pisanie
wy = fopen(argv[1],"w");

//petla do momentu kiedy nie zostanie wczytany EOF (do ... while)
if(wy != NULL)
{
	do
	{
	//opuszczenie semafora odczytu
	semafor_p(1);
	//sleep(3);
	znak=*adres_pamieci_wspolnej;
	//printf("%c %c",znak,*adres_pamieci_wspolnej);
	if(znak != EOF)
	{
		fputc(znak,wy);
	}
	//podniesienie semafora zapisu-sygnal dla p
    semafor_v(0);
	//sleep(3);
    }while(znak != EOF);	
//fputc('\n',wy);  
}
else
{
    printf("KONSUMENT: plik sie nie otworzyl\n");
	fclose(wy);
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
fclose(wy);
//odlaczenie pamieci dzielonej
if(shmdt(adres_pamieci_wspolnej) == -1)
{
	printf("Blad odlaczania pamieci: %i (%s)\n",errno,strerror(errno));
	return -1;
} 
else
{
	printf("Odlaczono pamiec wspolna\n");
}
//usuniecie segmentu pamieci dzielonej
if(shmctl(id_segmentu,IPC_RMID,0) == -1)
{
	printf("Blad usuniecia segmentu pamieci: %i (%s)\n",errno,strerror(errno));
	exit(EXIT_FAILURE);
}
else
{
	printf("Usunieto segment pamieci\n");
}
//usuniecie zbioru semaforow
usun_semafor();
return 0;

}
