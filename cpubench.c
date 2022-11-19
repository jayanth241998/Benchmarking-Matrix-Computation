#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <pthread.h>
#define MSG "* running cpubench with seed %s mode %s using %s with size %s and %s threads with debug %s...\n"

#define USAGE "usage: ./cpubench <seed> <mode> <type> <size> <threads> <debug> \n" \
"     - seed: 0 \n" \
"     - mode: flops / matrix \n" \
"     - type: single / double \n" \
"     - size: 256 / 1024 / 4096 / 16386 -- MegaFlops / array dimension \n" \
"     - threads: 1 / 2 / 4 \n" \
"     - debug: true / false \n" \


#define MEGAFLOPS 1000000
#define MEGABYTES 1024*1024

int DEBUG = false;
int divider = 0;
int  num_threads= 0;
int *arr1,*arr2,*arr3,*transpose;
double *darr1,*darr2,*darr3,*transposedouble;

//MUST MODIFY TO SATISFY ASSIGNMENT BELOW THIS LINE//
//=================================================//

int SIZE = 0;


// This function multiplies mat1[][] and mat2[][],
// and stores the result in res[][]
//may want to print periodic status to the screen to verify the rate of progress
void multiply_double(int size, double mat1[size][size], double mat2[size][size], double res[size][size])
{
    int i, j, k;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            res[i][j] = 0;
            for (k = 0; k < size; k++)
                res[i][j] += mat1[i][k] * mat2[k][j];
        }
    }
}

void multiply_int(int size, int* mat1, int* mat2, int* res)
{
    int i, j, k;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            res[i*size + j] = 0;
            for (k = 0; k < size; k++)
                res[i*size + j] += mat1[i*size + k] * mat2[k*size + j];
        }
    }
}

//transposing the matrix
void transposematrix(){
int i,j;

   for(i=0;i<SIZE;i++){
    for (j = 0; j < SIZE; j++) {
       arr2[j*SIZE + i] = transpose[i*SIZE + j];
    }
    }		
}

void transposematrixdouble(){
int i,j;
   for(i=0;i<SIZE;i++){
    for (j = 0; j < SIZE; j++) {
       darr2[j*SIZE + i] = transposedouble[i*SIZE + j];
    }
    }		
}

//computing portion of matrix with tile size 5
void* computemat_double_chunk(void* threadno){
int i,j,k,i1,j1,k1,i1max,j1max,k1max;
int thread = (int)threadno;
int start = thread * divider;
int end = start + divider;
end = (end >= SIZE)? SIZE : end;
int tile = 5;

   for (i = start; i < end; i+=tile) 
   {
         i1max = (i+ tile) > end ? end : (i + tile);
        for (j = 0; j < SIZE; j+=tile) 
        {   
        j1max = (j+ tile) > SIZE ? SIZE : (j + tile);
            for (k = 0; k < SIZE; k+=tile)
            {
                  k1max = (k+ tile) > SIZE ? SIZE : (k + tile);
                 for(i1=i; i1 < i1max ; i1++)
                 {
                     for(j1=j; j1 < j1max ; j1++)
                     {
                        for(k1=k ; k1 < k1max; k1++)
                        { 
                          darr3[i1*SIZE + j1] += darr1[i1*SIZE + k1] * (darr2[j1*SIZE + k1]);
                        }
                     }
                 }
             }
        }
    }    
   pthread_exit(0);
}

//creating threads for each portion of matrix
void multiply_double_parallel(int size, double* mat1, double* mat2, double* res)
{
    int i;
    pthread_t pthreads[num_threads];    
    
    transposematrixdouble();
    
    for(i=0;i<num_threads;i++)
    {
     pthread_create(&pthreads[i],NULL,computemat_double_chunk,(void *)i);
    }
    
    for(i=0;i<num_threads;i++)
    {
     pthread_join(pthreads[i],NULL);
    }
    free(darr1);
    free(darr2);
    
}


//computing portion of matrix with tile size 5
void* computemat_int_chunk(void* threadno){
int i,j,k,i1,j1,k1,i1max,j1max,k1max;
int thread = (int)threadno;
int start = thread * divider;
int end = start + divider;
end = (end >= SIZE)? SIZE : end;
int tile = 5;

   for (i = start; i < end; i+=tile) 
   {
         i1max = (i+ tile) > end ? end : (i + tile);
        for (j = 0; j < SIZE; j+=tile) 
        {   
        j1max = (j+ tile) > SIZE ? SIZE : (j + tile);
            for (k = 0; k < SIZE; k+=tile)
            {
                  k1max = (k+ tile) > SIZE ? SIZE : (k + tile);
                 for(i1=i; i1 < i1max ; i1++)
                 {
                     for(j1=j; j1 < j1max ; j1++)
                     {
                        for(k1=k ; k1 < k1max; k1++)
                        { 
                          arr3[i1*SIZE + j1] += arr1[i1*SIZE + k1] * (arr2[j1*SIZE + k1]);
                        }
                     }
                 }
             }
        }
    }    
   pthread_exit(0);
}

//creating threads for each portion of matrix
void multiply_int_parallel(int size, int* mat1, int* mat2, int* res)
{
    int i;
    pthread_t pthreads[num_threads];
    
    transposematrix();
    
    for(i=0;i<num_threads;i++)
    {
     pthread_create(&pthreads[i],NULL,computemat_int_chunk,(void *)i);
    }
    
    for(i=0;i<num_threads;i++)
    {
     pthread_join(pthreads[i],NULL);
    }
    free(arr1);
    free(arr2);
    
}



void compute_flops_int(int size)
{
	int index, index2;
	int result = 0;
	int loops = size;
	for (index=0;index<loops;index++)
	{ 
		for (index2=0;index2<MEGAFLOPS;index2++)
			result = index2+1;
	}
	if (DEBUG)
		printf(" result %d\n",result);
	
}

//dividing for loops among threads
void* parallel_flops_int(void* args)
{
   int threadID = (int)(args);
   int start = threadID * divider;
   int end = start + divider;
   end = (end >= SIZE) ? SIZE : end;
   int index, index2;
   int result = 0;
   for (index=start;index<end;index++)
	{
	  for (index2=0;index2<MEGAFLOPS;index2++){
			result = index2+1;}
	}
	
	if (DEBUG)
		printf(" result %d\n",result);
	
  pthread_exit(0);
}

//creating pthreads
void compute_flops_int_parallel(int size)
{
	int index;
	pthread_t threads[num_threads];
	for (index=0;index<num_threads;index++)
	{
	     pthread_create(&threads[index],NULL,parallel_flops_int,(void *)index);
	}
	
	for (index=0;index<num_threads;index++)
	{
	     pthread_join(threads[index],NULL);
	}
	
}


void compute_flops_double(int size)
{
	int index, index2;
	double result;
	int loops = size;
	for (index=0;index<loops;index++)
	{
		for (index2=0;index2<MEGAFLOPS;index2++)
			result = index2+1.0;
	}
	if (DEBUG)
		printf("%lf\n",result);
	
}


//dividing for loops among threads
void* parallel_flops_double(void* args)
{
   int threadID = (int)(args);
   int start = threadID * divider;
   int end = start + divider;
   end = (end >= SIZE) ? SIZE : end;
   int index, index2;
   double result = 0;
   
   for (index=start;index<end;index++)
	{
	  for (index2=0;index2<MEGAFLOPS;index2++)
			result = index2+1.0;
	}
	
	if (DEBUG)
		printf("%lf\n",result);
	
  pthread_exit(0);
}

//creating pthreads
void compute_flops_double_parallel(int size)
{
	int index;
	pthread_t threads[num_threads];
	for (index=0;index<num_threads;index++)
	{
	     pthread_create(&threads[index],NULL,parallel_flops_double,(void *)index);
	}
	
	for (index=0;index<num_threads;index++)
	{
	     pthread_join(threads[index],NULL);
	   
	}
	
}

//=================================================//
//MUST MODIFY TO SATISFY ASSIGNMENT ABOVE THIS LINE//



int compute_checksum_dynamic_double(int size, double* res){
	if (DEBUG)
	printf("compute_checksum...\n");
    int checksum = 0;
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
                checksum += (int)res[i*size + j];
        }
    }
    return checksum;
}

 int compute_checksum_dynamic_int(int size, int* res){
	if (DEBUG)
	printf("compute_checksum...\n");
	 int checksum = 0;
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
                checksum += res[i*size + j];
        }
    }
    return checksum;
}



int main(int argc, char **argv)
{
	int checksum = 0;
	time_t t;
	srand((unsigned) time(&t));
    if (argc != 7) 
    {
        printf(USAGE);
		printf("mode=-1 type=-1 size=-1 threads=-1 time=-1 throughput=-1 checksum=-1\n");
        exit(1);
    } 
    else 
    {
    	
    	int seed = atoi(argv[1]);
		srand(seed);
    	
		int mode = -1;
        if(strcmp(argv[2],"flops") == 0)
        	mode = 0;
        else if(strcmp(argv[2],"matrix") == 0)
        	mode = 1;
        else {
        	mode = -1;}

		int type = -1;
        if(strcmp(argv[3],"single") == 0)
        	type = 0;
        else if(strcmp(argv[3],"double") == 0)
        	type = 1;
        else{
        	type = -1;}


        int size = atoi(argv[4]);
        num_threads = atoi(argv[5]);
        divider = size/num_threads;
        SIZE = size;

        if(strcmp(argv[6],"true") == 0)
        	DEBUG = true;
        else if(strcmp(argv[6],"false") == 0)
        	DEBUG = false;
        else
        {
        	printf("error in DEBUG argument, must be either true or false; exiting...\n");
			printf("seed=%s mode=%s type=%s size=%d threads=%d time=0.0 throughput=0.0 checksum=-2\n",argv[1],argv[2],argv[3],size,num_threads);
			exit(-1);
	}	
		
		if (DEBUG)
    		printf(MSG, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
		

		struct timeval start, end;

		if (mode == 0 && type == 0)
		{	
			gettimeofday(&start, NULL);
			compute_flops_int_parallel(size);
		    gettimeofday(&end, NULL);
		}
		else if (mode == 0 && type == 1)
		{	
			gettimeofday(&start, NULL);
			compute_flops_double_parallel(size);
		    gettimeofday(&end, NULL);
		}		
		else if (mode == 1 && type == 0)
		{	
			
			size_t len = 0;
			int i, j;//, k;
  
			len = sizeof(double *) * SIZE * SIZE * 3;
			if (DEBUG)
				printf("allocating %lf GB memory...\n",len*3.0/(1024*1024*1024)); 

//dynamic allocation//
//=================================================//
	                 
			arr1 = (int *)malloc(size * size * sizeof(int));
			arr2 = (int *)malloc(size * size * sizeof(int));
			arr3 = (int *)malloc(size * size * sizeof(int));
			transpose = (int *)malloc(size * size * sizeof(int));
			
			




			
			for (i = 0; i < size*size; i++) 
				{
					arr1[i] = (int)((rand()/rand())*10.0); 
					transpose[i] = arr2[i] = (int)((rand()/rand())*10.0); 
					arr3[i] = 0;
				}
		
			if (DEBUG)
  			{
    			for (i = 0; i < size; i++) 
    			{
        			for (j = 0; j < size; j++) 
            			printf("%d ", arr1[i*size + j]); 
        			printf("\n");
				}
				printf("\n");

    			for (i = 0; i < size; i++) 
    			{
        			for (j = 0; j < size; j++) 
            			printf("%d ", arr2[i*size + j]); 
        			printf("\n");
				}
				printf("\n");
			}			
		
		
		
			gettimeofday(&start, NULL);
			multiply_int_parallel(size,arr1,arr2,arr3);
		        gettimeofday(&end, NULL);
		        
		        if(DEBUG) 
		        {
		        for (i = 0; i < size; i++) 
    			{
        			for (j = 0; j < size; j++) {
            			printf("%d ", arr3[i*size + j]); 
        			
				}
				printf("\n");
			
		        } 
		        }
		    
			/*if (STATIC == true)
				//checksum = compute_checksum_static_int(size, arr3);
			else*/
				checksum = compute_checksum_dynamic_int(size, arr3);
		    
		    
		}
		else if (mode == 1 && type == 1)
		{	
			size_t len = 0;
			int i, j;
			len = sizeof(double *) * size * size * 3;
			
			if (DEBUG)
				printf("allocating %lf GB memory...\n",len*3.0/(1024*1024*1024)); 
                        
////dynamic allocation////
//=================================================//
	


                         darr1 = (double *)malloc(size * size * sizeof(double));
			 darr2 = (double *)malloc(size * size * sizeof(double));
			 darr3 = (double *)malloc(size * size * sizeof(double));
			transposedouble = (double *)malloc(size * size * sizeof(double));
			
			  
				for (i = 0; i < size*size; i++) 
				{
                                darr1[i] = (double)rand()/rand(); // OR *(*(arr+i)+j) = ++count 
                                transposedouble[i] = darr2[i] = (double)rand()/rand(); // OR *(*(arr+i)+j) = ++count 
			        darr3[i] = 0.0; 
				}
				
				
			
		
			if (DEBUG)
  			{
    			for (i = 0; i < size; i++) 
    			{
        			for (j = 0; j < size; j++) 
            			printf("%lf ", darr1[i*size + j]); 
        			printf("\n");
				}
				printf("\n");

    			for (i = 0; i < size; i++) 
    			{
        			for (j = 0; j < size; j++) 
            			printf("%lf ", darr2[i*size + j]); 
        			printf("\n");
				}
				printf("\n");
			}			
		
		
			gettimeofday(&start, NULL);
			multiply_double_parallel(size,darr1,darr2,darr3);
		        gettimeofday(&end, NULL);

				checksum = compute_checksum_dynamic_double(size, darr3);
	
			if (DEBUG)
			{	    
				for (i = 0; i < size; i++) 
    			{
        			for (j = 0; j < size; j++) 
            			printf("%lf ", darr3[i*size + j]); 
        			printf("\n");
				}
				printf("\n");
			}	    
	        
		    
		    
		}
		else
		{
			if (DEBUG)
        	{
        		printf(USAGE);
				printf("unrecognized option, exiting...\n");
			}
			else
				printf("seed=%s mode=%s type=%s size=%d threads=%d time=0.0 throughput=0.0 checksum=-6\n",argv[1],argv[2],argv[3],size,num_threads);
				
        	exit(1);
		}

		double elapsed_time_sec = (((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)))/1000000.0;
		long double num_giga_ops = 0;
		
		if (size*MEGAFLOPS < 0)
		{
			if (DEBUG)
				printf("error in size, check for overflow; exiting...\n");
			else
				printf("seed=%s mode=%s type=%s size=%d threads=%d time=0.0 throughput=0.0 checksum=-7\n",argv[1],argv[2],argv[3],size,num_threads);
			exit(1);
		}
		if (elapsed_time_sec < 0)
		{
			if (DEBUG)
				printf("error in elapsed time, check for proper timing; exiting...\n");
			else
				printf("seed=%s mode=%s type=%s size=%d threads=%d time=0.0 throughput=0.0 checksum=-8\n",argv[1],argv[2],argv[3],size,num_threads);
			exit(1);
		}
		if (elapsed_time_sec == 0)
		{
			if (DEBUG)
				printf("elapsed time is 0, check for proper timing or make sure to increase amount of work performed; exiting...\n");
			else
				printf("seed=%s mode=%s type=%s size=%d threads=%d time=0.0 throughput=0.0 checksum=-9\n",argv[1],argv[2],argv[3],size,num_threads);
			exit(1);
		}
			
		
		if (mode == 0)
			num_giga_ops = (size*1000.0)/MEGAFLOPS;
		else if (mode == 1)
			num_giga_ops = (size*size*size*1.0)/(MEGABYTES*1024.0);
			
		long double throughput = num_giga_ops/elapsed_time_sec;
		printf("seed=%s mode=%s type=%s size=%d threads=%d time=%lf throughput=%Lf checksum=%d\n",argv[1],argv[2],argv[3],size,num_threads,elapsed_time_sec,throughput,checksum);
 
    }

    return 0;
}
