#include "../../libraries/rc_usefulincludes.h"
#include "../../libraries/roboticscape.h"

// choice of 1,2,4,8,16 oversampling. Here we use 16 and sample at 25hz which
// is close to the update rate specified in robotics_cape.h for that oversample.
#define OVERSAMPLE  BMP_OVERSAMPLE_16
// choice of OFF, 2, 4, 8, 16 filter constants. Here we turn off the filter and
// opt to use our own 2nd order filter instead.
#define INTERNAL_FILTER	BMP_FILTER_OFF

// our own low pass filter
#define ORDER			2
#define CUTOFF_FREQ		2.0f	// 2rad/s, about 0.3hz
#define BMP_CHECK_HZ	25
#define	DT				1.0f/BMP_CHECK_HZ
//Paramètres
#define pas 0.1 //pas entre 2 mesures
#define nb_e 5 //nombre échantillon de détection de chute
#define nb_check 50 //nb de boucle apres chute de demande d etat de la personne
#define sh_alr 40 //seuil haut
//Variables et initialisation sprintf pour l'envoie du mail
char comm[256];
char buf[256];
int system(const char *command);
#define TAILLE_MAX 256

int chute=0; // flag de chute détectée
int compteur = 0;
int etat =0;

char user[5][TAILLE_MAX]; //Le nombre de lignes du tableau dépend du nombre d'informations à récupérer
char contact1[5][TAILLE_MAX];
char contact2[5][TAILLE_MAX];
//char contact3[5][TAILLE_MAX];

//bouttons
void on_pause_pressed(){
	rc_set_led(RED, ON);
	chute = -1;
	printf("Appel au secours demandé\n");
	sprintf(comm, "echo \"L'utilisateur a appuyé sur le bouton d'appel au secours\" | mutt -x -s \"Alerte : l'utilisateur requiert de l'aide\" info.mec4@gmail.com -a data.csv");
	system(comm);

	return;
}

void on_mode_pressed(){
	if((compteur >nb_e )&& (compteur<(nb_e+nb_check))){
		printf("La personne ne requiert pas l'appel des secours\n");
		rc_set_led(RED, OFF);
		compteur = 0;
		etat = 0;
	}
	return;
}

int main(int argc, char *argv[]){

//Parsing

    FILE* fichier = NULL;
    fichier = fopen("info.txt", "r");

    if (fichier != NULL){
	//ON CHERCHE LE NOMBRE DE LIGNES
	puts("fichier ouvert");
	int nb_ligne=0;
	char c;
	do{
		c= fgetc(fichier);
		if(c=='\n'){
			nb_ligne++;
		}
	}while(nb_ligne<3);//(c!=EOF);
	printf("nombre de ligne : %d \n\n", nb_ligne);

	//On replace le curseur au debut du fichier
	fseek( fichier, 0, SEEK_SET );
printf("nombre de ligne %d", nb_ligne);

	//Définition des tableaux qui vont stocker les données

	//char contact4[5][TAILLE_MAX];
	//char contact5[5][TAILLE_MAX];

//On va parser chaque ligne indépendamment, et on va incorporer ça dans le tableau correspondant. Le nombre de chaine a lire dépend du nombre de ligne dans le .txt

//Déclaration des variables utilisées pour le parsing :

	int mot=0;
	int debutmot=0;
	int i=0;
	int j=0;
	int k=0;
	int longueurmot=0;
	int u,v;
	int longueurChaine;


	//Parsing chaine1
	if(nb_ligne>0){

		char chaine[TAILLE_MAX] = ""; //Contient user
		fgets(chaine, TAILLE_MAX, fichier); //On récupère la première ligne du fichier
		mot=0;
		debutmot=0;
		i=0;
		j=0;
		k=0;
		longueurmot=0;

		//Initialisation du tableau
		for(u=0;u<TAILLE_MAX;u++){
			for(v=0;v<5;v++){
				user[v][u]='\0';
			}
		}
		//Remplissage du Tableau avec le parsing
		longueurChaine = strlen(chaine);
		for(i=0;i<=longueurChaine;i++){
			if ((chaine[i] == ',')||(chaine[i]=='\n')){
				longueurmot=i-debutmot;
				for(j=debutmot;j<=i-1;j++){
					user[mot][j-debutmot]=chaine[j]; //On concatène notre chaine avec le caractère
				}
				debutmot=debutmot+longueurmot+1;
				mot++; //On passe à la case suivante

			}
		}
		//On vérifie le résulat
		for (k=0;k<5;k++){
			printf("user %d : %s \n", k, user[k]);
		}
	}



	//Parsing Contact1
	if(nb_ligne>1){

		for(u=0;u<TAILLE_MAX;u++){
			for(v=0;v<5;v++){
				contact1[v][u]='\0';
			}
		}

		char chaine1[TAILLE_MAX] = ""; //Contient contact 1
		fgets(chaine1, TAILLE_MAX, fichier);

		//réinitialisation des variables
		mot=0;
		debutmot=0;
		i=0;
		j=0;
		k=0;
		longueurmot=0;

		longueurChaine = strlen(chaine1);
		for(i=0;i<=longueurChaine;i++){
			if ((chaine1[i] == ',')||(chaine1[i]=='\n')){
				longueurmot=i-debutmot;
				for(j=debutmot;j<=i-1;j++){
					contact1[mot][j-debutmot]=chaine1[j]; //On concatène notre chaine
				}
				debutmot=debutmot+longueurmot+1;
				mot++; //On passe à la case suivante
			}
		}
		for (k=0;k<5;k++){
			printf("contact 1 : %d : %s \n", k, contact1[k]);
		}
	}


//Parsing Contact2
	if(nb_ligne>2){

		for(u=0;u<TAILLE_MAX;u++){
			for(v=0;v<5;v++){
				contact2[v][u]='\0';
			}
		}

		char chaine2[TAILLE_MAX] = "";
		fgets(chaine2, TAILLE_MAX, fichier);
		mot=0;
		debutmot=0;
		i=0;
		j=0;
		k=0;
		longueurmot=0;

		longueurChaine = strlen(chaine2);
		for(i=0;i<=longueurChaine;i++){
			if ((chaine2[i] == ',')||(chaine2[i]=='\n')){
				longueurmot=i-debutmot;
				for(j=debutmot;j<=i-1;j++){
					contact2[mot][j-debutmot]=chaine2[j]; //On concatène notre chaine
				}
				debutmot=debutmot+longueurmot+1;
				mot++; //On passe à la case suivante
			}
		}
		for (k=0;k<5;k++){
			printf("contact 2: %d : %s \n", k, contact2[k]);
		}
	}
puts("fin parsing");
/*
//Parsing Contact3
	if(nb_ligne>3){

		for(u=0;u<TAILLE_MAX;u++){
			for(v=0;v<5;v++){
				contact3[v][u]='\0';
			}
		}

		char chaine3[TAILLE_MAX] = "";
		fgets(chaine3, TAILLE_MAX, fichier);
		mot=0;
		debutmot=0;
		i=0;
		j=0;
		k=0;
		longueurmot=0;

		longueurChaine = strlen(chaine3);
		for(i=0;i<=longueurChaine;i++){
			if ((chaine3[i] == ',')||(chaine3[i]=='\n')){
				longueurmot=i-debutmot;
				for(j=debutmot;j<=i-1;j++){
					contact3[mot][j-debutmot]=chaine3[j]; //On concatène notre chaine
				}
				debutmot=debutmot+longueurmot+1;
				mot++; //On passe à la case suivante
			}
		}
		for (k=0;k<5;k++){
			printf("contact 3 : %d : %s \n", k, contact3[k]);
		}
	}
*/

}




//Fin Parsing



	float valeur[nb_e];
	float alr; //résultante de l'accélération
	int alr_max=0; //valeur max de alr lors d'une chute
	float t_alr_max=0; // temps corresponds a alr_max
	//int i=0; //variable d'itération de boucle
	int q=0;
	int ret;

	//Accelerometre

	double altitude, filtered_alt;
	rc_filter_t lowpass = rc_empty_filter(); //Filtre passe bas altimètre
	rc_imu_data_t data; //struct to hold new data
	rc_imu_config_t conf = rc_default_imu_config();

	// initialize hardware first
	if(rc_initialize()){
		fprintf(stderr,"ERROR: failed to run rc_initialize(), are you root?\n");
		return -1;
	}

	//boutons
	rc_set_pause_pressed_func(&on_pause_pressed);
	rc_set_mode_pressed_func(&on_mode_pressed);

	if(rc_initialize_imu(&data, conf)){
		fprintf(stderr,"rc_initialize_imu_failed\n");
		return -1;
	}

	if(rc_initialize_barometer(OVERSAMPLE, INTERNAL_FILTER)<0){
		fprintf(stderr,"ERROR: rc_initialize_barometer failed\n");
		return -1;
	}

	// create the lowpass filter and prefill with current altitude
	if(rc_butterworth_lowpass(&lowpass,ORDER, DT, CUTOFF_FREQ)){
		fprintf(stderr,"ERROR: failed to make butterworth filter\n");
		return -1;
	}

	printf("Démarage\n");
	altitude = rc_bmp_get_altitude_m();
	rc_prefill_filter_inputs(&lowpass, altitude);
	rc_prefill_filter_outputs(&lowpass, altitude);
	float tim=0;

	int fd;
	fd = open("data.csv", O_CREAT|O_WRONLY|O_TRUNC);
	if(fd < 0)
	{
		printf("erreur open\n");
		return -1;
	}

	while((rc_get_state()!=EXITING)&&(chute!=-1)){
		//rc_usleep(1000000/BMP_CHECK_HZ);
		rc_usleep(1000000*pas);
		tim+=pas;
		//get data from accelero

		if(rc_read_barometer()<0){
			fprintf(stderr,"\rERROR: Can't read Barometer");
			//fflush(stdout);
			continue;
		}

		if(rc_read_accel_data(&data)<0){
			printf("read accel data failed\n");
		}

		if(rc_read_gyro_data(&data)<0){
			printf("read gyro data failed\n");
		}

		// if we got here, new data was read and is ready to be accessed.
		// these are very fast function calls and don't actually use i2c

		altitude = rc_bmp_get_altitude_m();
		filtered_alt = rc_march_filter(&lowpass,altitude);
		//rc_read_accel_data(&data);
		//rc_read_gyro_data(&data);


		alr = sqrt(data.accel[0]*data.accel[0]+data.accel[1]*data.accel[1]+data.accel[2]*data.accel[2]);

		//affichage terminal
		//printf("in printf : %6.2f,%6.2f,%6.2f,%6.1f,%6.1f,%6.1f,%8.2f,%6.2f,%6.2f\n", data.accel[0], data.accel[1], data.accel[2], data.gyro[0]*DEG_TO_RAD, data.gyro[1]*DEG_TO_RAD, data.gyro[2]*DEG_TO_RAD, filtered_alt, tim,alr);
		printf("t = %6.2f s -> alr =%6.2f m/s² \n",tim,alr);

		//envoie des données dans le fichier data.csv
		sprintf(buf, "%6.2f,%6.2f,%6.2f,%6.1f,%6.1f,%6.1f,%8.2f,%6.2f\n", data.accel[0], data.accel[1], data.accel[2], data.gyro[0]*DEG_TO_RAD, data.gyro[1]*DEG_TO_RAD, data.gyro[2]*DEG_TO_RAD, filtered_alt, tim);
		ret = write(fd, buf, strlen(buf));
		if(ret <= 0){
				printf("erreur write\n");
			return -1;
		}


		if(((compteur > 0) && (compteur < nb_e))||(alr > sh_alr)){
			etat = 1;
		}
		else if(compteur == nb_e){
			etat = 2;
		}
		else if((compteur > nb_e) && (compteur< (nb_e + nb_check))){
			etat =3;
		}
		else if(compteur == (nb_e + nb_check)){
			etat = 4;
		}
		else if(compteur == 0){
			etat = 0;
		}


		switch(etat){
			case 0 :	printf("Etat 0 \t");
						//Detection de chute active
						break;
			case 1 :	printf("etat 1 \t");
						//Chute détectée
						valeur[compteur]=alr;
						compteur+=1;
						rc_set_led(RED, ON);
						break;

			case 2 :	printf("etat 2 \t");
						alr_max=valeur[0];
						t_alr_max=tim-(nb_e-1)*pas;

						for(q=1;q==nb_e-1;q++){
							if(valeur[q]>alr_max){
								alr_max=valeur[q];
								t_alr_max+=q*pas;
							}
						}
						compteur += 1;
						break;

			case 3 :	printf("etat 3 \t");
						compteur += 1;
						break;

			case 4 :	printf("etat 4 \t");
						//sprintf(comm, "echo \"une chute à été detectée, pas de reponse de l'utilisateur. Valeur pic alr= %d à t simu= %6.2f secondes \" | mutt -x -s \"Alerte : Chute + utilisateur inconcient?\" info.mec4@gmail.com -a data.csv", alr_max, t_alr_max);
sprintf(comm, "echo \"Bonjour %s %s,\n une chute à été detectée au domicile de %s %s (adresse : %s), pas de reponse de l'utilisateur. Valeur pic alr= %d à t simu= %6.2f secondes \" | mutt -x -s \"Alerte : Chute + utilisateur inconcient?\" %s -a data.csv", contact1[2], contact1[0], user[0], user[1], user[2], alr_max, t_alr_max, contact1[3]);
system(comm);
						printf("chute détectée à alr= %d à t simu= %6.2f secondes\n",alr_max,t_alr_max);
						chute = -1;
						break;

			default :	break;
		}
	}
	rc_power_off_imu();
	rc_power_off_barometer();
	rc_cleanup();
	return 0;
}

