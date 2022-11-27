#include <stdio.h>
#include <sys/mman.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <pthread.h>


#define _BSD_SOURCE

#define RADIO_TUNER_FAKE_ADC_PINC_OFFSET 0
#define RADIO_TUNER_TUNER_PINC_OFFSET 1
#define RADIO_TUNER_CONTROL_REG_OFFSET 2
#define RADIO_TUNER_TIMER_REG_OFFSET 3
#define RADIO_PERIPH_ADDRESS 0x43c00000
#define RADIO_FIFO_ADDRESS 0x43c10000
#define RADIO_FIFO_DATA_OFFSET 0
#define RADIO_FIFO_COUNT_OFFSET 1
#define PORT  25344

//global variables 
char samples[1026];
int16_t packet_count = 0;
int sockfd; 
struct sockaddr_in servaddr, cliaddr; 
volatile unsigned int *my_fifo;
	
 
	

// the below code uses a device called /dev/mem to get a pointer to a physical
// address.  We will use this pointer to read/write the custom peripheral
volatile unsigned int * get_a_pointer(unsigned int phys_addr)
{

	int mem_fd = open("/dev/mem", O_RDWR | O_SYNC); 
	void *map_base = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, phys_addr); 
	volatile unsigned int *radio_base = (volatile unsigned int *)map_base; 
	return (radio_base);
}


void radioTuner_tuneRadio(volatile unsigned int *ptrToRadio, float tune_frequency)
{
	float pinc = (-1.0*tune_frequency)*(float)(1<<27)/125.0e6;
	*(ptrToRadio+RADIO_TUNER_TUNER_PINC_OFFSET)=(int)pinc;
}

void radioTuner_setAdcFreq(volatile unsigned int* ptrToRadio, float freq)
{
	float pinc = freq*(float)(1<<27)/125.0e6;
	*(ptrToRadio+RADIO_TUNER_FAKE_ADC_PINC_OFFSET) = (int)pinc;
}

/* void play_tune(volatile unsigned int *ptrToRadio, float base_frequency)
{
	int i;
	float freqs[16] = {1760.0,1567.98,1396.91, 1318.51, 1174.66, 1318.51, 1396.91, 1567.98, 1760.0, 0, 1760.0, 0, 1760.0, 1975.53, 2093.0,0};
	float durations[16] = {1,1,1,1,1,1,1,1,.5,0.0001,.5,0.0001,1,1,2,0.0001};
	for (i=0;i<16;i++)
	{
		radioTuner_setAdcFreq(ptrToRadio,freqs[i]+base_frequency);
		usleep((int)(durations[i]*500000));
	}
} */


/* void print_benchmark(volatile unsigned int *periph_base)
{
    // the below code does a little benchmark, reading from the peripheral a bunch 
    // of times, and seeing how many clocks it takes.  You can use this information
    // to get an idea of how fast you can generally read from an axi-lite slave device
    unsigned int start_time;
    unsigned int stop_time;
    start_time = *(periph_base+RADIO_TUNER_TIMER_REG_OFFSET);
    for (int i=0;i<2048;i++)
        stop_time = *(periph_base+RADIO_TUNER_TIMER_REG_OFFSET);
    printf("Elapsed time in clocks = %u\n",stop_time-start_time);
    float throughput=0;
    // please insert your code here for calculate the actual throughput in Mbytes/second
	//total num of megabytes transfered = 0.008192
	//clk is 125 mhz
	//total time = stop - start(cycles) /125000000 (cycles/sec)  
	throughput = 0.008192/((float)(stop_time - start_time)/125000000.0);
    // how much data was transferred? How long did it take?
  
    printf("Estimated Transfer throughput = %f Mbytes/sec\n",throughput);
} */

//thread for UDP stream 
void *UDP_stream(void *arg)
{
	unsigned int fifo_data; 
    unsigned int count;
	unsigned int fifo_data; 
	int samp_count;
	char bytes[4]; 
	
	//read and send data continuously
	
	while(1){
		count = *(fifo_base+RADIO_FIFO_COUNT_OFFSET); 
		if(count > 252){ //this value may have to be changes depending on how long this process takes vs filling up fifo 
			samp_count =2; 
			for(int i =2; i < 258; i++;){
				fifo_data = *(my_fifo+RADIO_FIFO_DATA_OFFSET);
				bytes[0] = (fifo_data >> 24) & 0xFF;
				bytes[1] = (fifo_data >> 16) & 0xFF;
				bytes[2] = (fifo_data >> 8) & 0xFF;
				bytes[3] = fifo_data & 0xFF;
				samples[samp_count] = bytes [0];
				samp_count++; 
				samples[samp_count] = bytes [1];
				samp_count++; 
				samples[samp_count] = bytes [2];
				samp_count++;  
				samples[samp_count] = bytes [3];
				samp_count++; 
				samples[0] = (packet_count >> 8) & 0xFF;
				samples[1] = packet_count & 0xFF;
				packet_count++; 
				sendto(sockfd, samples, sizeof(samples), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
			}
		}
	}
}

//prints menu
void printMenu(){
	printf("Options:\n\rf - Manually input a frequency\n\rt - Manually input tuner freq\n\ru/U - Increase Freq by 100/1000Hz\n\rd/D - Decrease Freq by 100/1000Hz\n\rE/X Enable/Disable UDP stream\n\r");
}

int main()
{

// first, get a pointer to the peripheral base address using /dev/mem and the function mmap
    volatile unsigned int *my_periph = get_a_pointer(RADIO_PERIPH_ADDRESS);	
	my_fifo = get_a_pointer(RADIO_FIFO_ADDRESS);	
	char input; 
	int adc_freq; 
	int tune_freq; 
	char ipaddr[15];
	pthread_t tid; 

    print("Welcome to SDR LAB:\n\rCreated by: Jordan Serbin\r\nThis program will play a single tone defined by user inputs on the freq and tuner\n\rand can UDP Stream data\n\rInput a IP address to stream UDP data packets to: ");
	scanf("%s", ipaddr)
	
	    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
        
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
        
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
	
	// Filling client information 
    cliaddr.sin_family    = AF_INET; // IPv4 
    cliaddr.sin_addr.s_addr = inet_addr(ip); 
    cliaddr.sin_port = htons(PORT); 
        
		
	UDP_params.servaddr = servaddr; 
	UDP_params.cliaddr = cliaddr; 
	
	    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
	
    printMenu();
	while(1){
			scanf("Enter option: %c", input);
			switch (input) {
			case 'f':
				printf("Enter ADC Frequency Between 0 and 600000000 Hz: \n\r");
				scanf("%i", adc_freq);
				radioTuner_setAdcFreq(my_periph,adc_freq);
				break;
			case 'u':
				radioTuner_setAdcFreq(my_periph,adc_freq + 100);
				break;
			case 'U':
				radioTuner_setAdcFreq(my_periph,adc_freq + 1000);
				break;
			case 'd':
				radioTuner_setAdcFreq(my_periph,adc_freq - 100);
				break;
			case 'D':
				radioTuner_setAdcFreq(my_periph,adc_freq - 1000);
				break;
			case 't' :
				printf("Enter a Tuner Frequency Between 0 and 600000000 Hz: \n\r");
				scanf(%i, tune_freq);
				radioTuner_tuneRadio(my_periph,tune_freq);
				break;
			case 'E':
				print("UDP Stream Enabled\n\r");
				pthread_create(&tid, NULL, UDP_stream, NULL); 
				break;
			case 'D':
				print("UDP Stream Disabled\n\r");
				pthread_cancel(tid); 
				break;
			default:
				print("Not a valid option\n\r");
				break;
		}
			printMenu();
		}
	}
	return 0;
}