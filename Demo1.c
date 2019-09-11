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

//Sockets
#define MY_PORT		5000
int lsockfd, csockfd;

int main(int argc, char *argv[])
{

//Sockets
	int ret;
	char buf[256];
	struct sockaddr_in serv_addr;

/*	if(rc_initialize_imu(&data, conf)){
		fprintf(stderr,"rc_initialize_imu_failed\n");
		return -1;
	}
*/

	lsockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(lsockfd < 0)
	{
		printf("erreur socket\n");
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(MY_PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(lsockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
	if(ret < 0)
	{
		printf("erreur bind\n");
		return -1;
	}

	printf("listening\n");
	ret = listen(lsockfd, 10);
	if(ret < 0)
	{
		printf("erreur listen\n");
		return -1;
	}

	printf("accepting\n");
	csockfd = accept(lsockfd, (struct sockaddr *)NULL, NULL);
	if(csockfd < 0)
	{
		printf("erreur accept\n");
		return -1;
	}
	printf("client connecté \n");


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
	printf("hello");
	altitude = rc_bmp_get_altitude_m();
	rc_prefill_filter_inputs(&lowpass, altitude);
	rc_prefill_filter_outputs(&lowpass, altitude);
float tim=0;
	while(rc_get_state()!=EXITING)
	{
	//rc_usleep(1000000/BMP_CHECK_HZ);
	//rc_usleep(100000);
rc_usleep(100000);
tim=tim+0.1;
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

printf("in printf : %6.2f,%6.2f,%6.2f,%6.1f,%6.1f,%6.1f,%8.2f\n", data.accel[0], data.accel[1], data.accel[2], data.gyro[0]*DEG_TO_RAD, data.gyro[1]*DEG_TO_RAD, data.gyro[2]*DEG_TO_RAD, filtered_alt);

	//envoi des données
	sprintf(buf, "%6.2f,%6.2f,%6.2f,%6.1f,%6.1f,%6.1f,%8.2f,%6.2f\n", data.accel[0], data.accel[1], data.accel[2], data.gyro[0]*DEG_TO_RAD, data.gyro[1]*DEG_TO_RAD, data.gyro[2]*DEG_TO_RAD, filtered_alt, tim);
	ret = write(csockfd, &buf, sizeof(buf));
	printf(" by string : %s", buf);
		if(ret <= 0)
		{
			printf("erreur write value\n");
			return -1;
		}
	}
	rc_power_off_imu();
	rc_power_off_barometer();
	rc_cleanup();
return 0;
}

