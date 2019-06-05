#include <stdio.h>
#include <pthread.h>

int checkArgumentNumbers(int argumentsNumber)
{
	if(argumentsNumber<=2)
	{
		printf("Invalid arguments number. Has to be at least 2.\n");
		return -1;
	}
	else return 0;
}

void addReader(void *orderNumber);

void addWriter(void *orderNumber);

int main(int argc, char* argv[])
{
	if(checkArgumentNumbers(argc)==-1) return 0;
	while (1)
    	{
		static struct option long_options[] =
        	{
          		{"writers",  required_argument, 0, 'w'},
          		{"readers",  required_argument, 0, 'r'},
          		{"debug",  no_argument, 0, 'd'},
          		{0, 0}
        	};
      		int option_index = 0;

		c = getopt_long (argc, argv, "r:w:d",
                long_options, &option_index);

		// Detect the end of the options. 
      	        if (c == -1)
		break;

      		switch (c)
        	{
      			case 0:
        		// If this option set a flag, do nothing else now. 
    			if (long_options[option_index].flag != 0)
		        break;
         		printf ("option %s", long_options[option_index].name);
          		if (optarg)
  		        	printf (" with arg %s", optarg);
          		printf ("\n");
          		break;

			case 'r':
				readerNumber=atoi(optarg);
				break;

			case 'w':
				writerNumber=atoi(optarg);
				break;

			case 'd':
				debug=1;
				break;

			case '?':
				break;

			default:
				break;
        	}
	}
	pthread_t writerThread[writerNumber], readerNumber[readerNumber];
	int iretW[writerNumber], iretR[readerNumber];
	for(int i=0; i<writerNumber; i++)
	{
		iretW[i]=pthread_create(&writerThread[i], NULL, addWriter, &i);
	}
	for(int i=0; i<readerNumber; i++)
	{
		iretR[i]=pthread_create(&readerThread[i], NULL, addReader, &i);
	}
	for(int i=0; i<writerNumber; i++)
	{
		pthread_join(writerThread[i], NULL);
	}
	for(int i=0; i<readerNumber; i++)
	{
		pthread_join(readerThread[i], NULL);
	}
	exit(0);
}
