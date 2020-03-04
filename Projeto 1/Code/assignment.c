////////////////////////////////////////////////////////////////////////////////////////////////////
//
// AED, 2019/2020
//
// 92952  Rui Fernandes
// 93015  Daniel Gomes
//
//
// Brute-force solution of the assignment problem (https://en.wikipedia.org/wiki/Assignment_problem)
//
// Compile with "cc -Wall -O2 assignment.c -lm" or equivalent
//
// In the assignment problem we will solve here we have n agents and n tasks; assigning agent
//   a
// to task
//   t
// costs
//   cost[a][t]
// The goal of the problem is to assign one agent to each task such that the total cost is minimized
// The total cost is the sum of the costs
//
// Things to do:
//   0. (mandatory)																							DONE
//      Place the student numbers and names at the top of this file
//   1. (highly recommended)																				DONE+-
//      Read and understand this code
//   2. (mandatory)																							DONE
//      Modify the function generate_all_permutations to solve the assignment problem
//      Compute the best and worst solutions for all problems with sizes n=2,...,14 and for each
//      student number of the group
//   3. (mandatory)																							DONE
//      Calculate and display an histogram of the number of occurrences of each cost
//      Does it follow approximately a normal distribution?
//      Note that the maximum possible cost is n * t_range
//   4. (optional)																							DONE
//      For each problem size, and each student number of the group, generate one million (or more!)
//      random permutations and compute the best and worst solutions found in this way; compare
//      these solutions with the ones found in item 2
//      Compare the histogram computed in item 3 with the histogram computed using the random
//      permutations
//   5. (optional)																							DONE
//      Try to improve the execution time of the program (use the branch-and-bound technique)
//   6. (optional)
//      Surprise us, by doing something more!
//   7. (mandatory)
//      Write a report explaining what you did and presenting your results
//  //  max-plus algebra ,tropical algebra 
// e se o custo mninimo nao for 3?..ver custo minimo de cada matriz(No big deal O(n)=N²)
// but partialcost+1ºmin_cost + 2º min cost... (n-m)º min cost seria despendioso ..
// kolmogorov-smirnov cumulative distribution colocar no relatorio so para demonstrar conhecimento
// octave : plot x->(1;:);(logn,2:);z


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define NDEBUG  // uncomment to skip disable asserts (makes the code slightly faster)
#include <assert.h>


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// problem data
//
// max_n ........ maximum problem size
// cost[a][t] ... cost of assigning agent a to task t
//

//
// if your compiler complains about srandom() and random(), replace #if 0 by #if 1
//
#if 0
# define srandom srand
# define random  rand
#endif

#define max_n    32           // do not change this (maximum number of agents, and tasks)
#define range    20           // do not change this (for the pseudo-random generation of costs)
#define t_range  (3 * range)  // do not change this (maximum cost of an assignment)

static int cost[max_n][max_n];
static int seed; // place a student number here!
static FILE *gnuplot;

static void init_costs(int n)
{
  if(n == -3)
  { // special case (example for n=3)
    cost[0][0] = 3; cost[0][1] = 8; cost[0][2] = 6;
    cost[1][0] = 4; cost[1][1] = 7; cost[1][2] = 5;
    cost[2][0] = 5; cost[2][1] = 7; cost[2][2] = 5;
    return;
  }
  if(n == -5)
  { // special case (example for n=5)
    cost[0][0] = 27; cost[0][1] = 27; cost[0][2] = 25; cost[0][3] = 41; cost[0][4] = 24;
    cost[1][0] = 28; cost[1][1] = 26; cost[1][2] = 47; cost[1][3] = 38; cost[1][4] = 21;
    cost[2][0] = 22; cost[2][1] = 48; cost[2][2] = 26; cost[2][3] = 14; cost[2][4] = 24;
    cost[3][0] = 32; cost[3][1] = 31; cost[3][2] =  9; cost[3][3] = 41; cost[3][4] = 36;
    cost[4][0] = 24; cost[4][1] = 34; cost[4][2] = 30; cost[4][3] = 35; cost[4][4] = 45;
    return;
  }
  assert(n >= 1 && n <= max_n);
  srandom((unsigned int)seed * (unsigned int)max_n + (unsigned int)n);
  for(int a = 0;a < n;a++)
    for(int t = 0;t < n;t++)
      cost[a][t] = 3 + (random() % range) + (random() % range) + (random() % range); // [3,3*range]
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// code to measure the elapsed time used by a program fragment (an almost copy of elapsed_time.h)
//
// use as follows:
//
//   (void)elapsed_time();
//   // put your code to be time measured here
//   dt = elapsed_time();
//   // put morecode to be time measured here
//   dt = elapsed_time();
//
// elapsed_time() measures the CPU time between consecutive calls
//

#if defined(__linux__) || defined(__APPLE__)

//
// GNU/Linux and MacOS code to measure elapsed time
//

#include <time.h>

static double elapsed_time(void)
{
  static struct timespec last_time,current_time;

  last_time = current_time;
  if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&current_time) != 0)
    return -1.0; // clock_gettime() failed!!!
  return            ((double)current_time.tv_sec - (double)last_time.tv_sec)
         + 1.0e-9 * ((double)current_time.tv_nsec - (double)last_time.tv_nsec);
}

#endif

#if defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)

//
// Microsoft Windows code to measure elapsed time
//

#include <windows.h>

static double elapsed_time(void)
{
  static LARGE_INTEGER frequency,last_time,current_time;
  static int first_time = 1;

  if(first_time != 0)
  {
    QueryPerformanceFrequency(&frequency);
    first_time = 0;
  }
  last_time = current_time;
  QueryPerformanceCounter(&current_time);
  return (double)(current_time.QuadPart - last_time.QuadPart) / (double)frequency.QuadPart;
}

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// function to generate a pseudo-random permutation
//

void random_permutation(int n,int t[n])
{
  assert(n >= 1 && n <= 1000000);
  for(int i = 0;i < n;i++)
    t[i] = i;
  for(int i = n - 1;i > 0;i--)
  {
    int j = (int)floor((double)(i + 1) * (double)random() / (1.0 + (double)RAND_MAX)); // range 0..i
    assert(j >= 0 && j <= i);
    int k = t[i];
    t[i] = t[j];
    t[j] = k;
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// place to store best and worst solutions (also code to print them)
//

static int min_cost,min_cost_assignment[max_n]; // smallest cost information
static int max_cost,max_cost_assignment[max_n]; // largest cost information
static long n_visited; // number of permutations visited (examined)
static int histogram[max_n*t_range +1]; // place your histogram global variable here
static double cpu_time;

#define minus_inf  -1000000000  // a very small integer
#define plus_inf   +1000000000  // a very large integer

static void reset_solutions(void)
{
  min_cost = plus_inf;
  max_cost = minus_inf;
  n_visited = 0l;
  memset(histogram, 0, (max_n*t_range +1)*sizeof(histogram[0])); // place your histogram initialization code here
  cpu_time = 0.0;
}

#define show_info_1        (1 << 0)
#define show_info_2        (1 << 1)
#define show_costs         (1 << 2)
#define show_min_solution  (1 << 3)
#define show_max_solution  (1 << 4)
#define show_histogram     (1 << 5)
#define show_all           (0xFFFF)

static void export_histogram(int hist[], int n, int seed){

	//exports to a txt file so the values can be imported in to matlab, can be altered to make a dat file for gnuplot

	char n_str[6];
	sprintf(n_str, "%d", n);
	char seed_str[6];
	sprintf(seed_str, "%d", seed);

	char fname[80];
	strcpy(fname, "histogram_");
	strcat(fname, n_str);
	strcat(fname, "_");
	strcat(fname, seed_str);
	strcat(fname, ".txt");

	FILE *fp = fopen(fname, "w");
	for(int i=0;i<(n*t_range); i++){
		fprintf(fp,"%d,",hist[i]);
	}
	fprintf(fp,"%d",hist[n*t_range]);
	fclose(fp);
}

static void plot_histogram(int hist[], int n, int seed, char *method){
	
	gnuplot = popen("gnuplot -persist","w");

	fprintf(gnuplot, "set term qt title '%s'\n",method);
	fprintf(gnuplot, "set style data histograms\n");
	fprintf(gnuplot, "set style histogram clustered gap 1.2\n");
	fprintf(gnuplot, "set boxwidth 0.8 relative\n");
	fprintf(gnuplot, "set style fill solid\n");
	fprintf(gnuplot, "plot '-' using 2 linecolor 'red' title 'Occurrences of Costs for n=%d and seed=%d'\n",n,seed);
	for(int i = min_cost; i <= max_cost; i++){
		fprintf(gnuplot, "%d %d\n",i,hist[i]);
	}
	fprintf(gnuplot,"e");
	fflush(gnuplot);
	pclose(gnuplot);
}

static void show_solutions(int n,char *header,int what_to_show)
{
  printf("%s\n",header);
  if((what_to_show & show_info_1) != 0)
  {
    printf("  seed .......... %d\n",seed);
    printf("  n ............. %d\n",n);
  }
  if((what_to_show & show_info_2) != 0)
  {
    printf("  visited ....... %ld\n",n_visited);
    printf("  cpu time ...... %.3fs\n",cpu_time);
  }
  if((what_to_show & show_costs) != 0)
  {
    printf("  costs .........");
    for(int a = 0;a < n;a++)
    {
      for(int t = 0;t < n;t++)
        printf(" %2d",cost[a][t]);
      printf("\n%s",(a < n - 1) ? "                 " : "");
    }
  }
  if((what_to_show & show_min_solution) != 0)
  {
    printf("  min cost ...... %d\n",min_cost);
    if(min_cost != plus_inf)
    {
      printf("  assignement ...");
      for(int i = 0;i < n;i++)
        printf(" %d",min_cost_assignment[i]);
      printf("\n");
    }
  }
  if((what_to_show & show_max_solution) != 0)
  {
    printf("  max cost ...... %d\n",max_cost);
    if(max_cost != minus_inf)
    {
      printf("  assignement ...");
      for(int i = 0;i < n;i++)
        printf(" %d",max_cost_assignment[i]);
      printf("\n");
    }
  }
  if((what_to_show & show_histogram) != 0)
  {
  	export_histogram(histogram, n, seed);					//uncomment to export histogram
  	plot_histogram(histogram, n, seed, header);
  	//printf("  histogram ...\n");						//	uncomment to print histogram values on the terminal
  	for(int i = min_cost; i <= max_cost; i++)
  	printf("                [%d]  %d\n",i, histogram[i]);
  	printf("\n");
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// code used to generate all permutations of n objects
//
// n ........ number of objects
// m ........ index where changes occur (a[0], ..., a[m-1] will not be changed)
// a[idx] ... the number of the object placed in position idx
//
// TODO: modify the following function to solve the assignment problem
//

static void evaluate_permutation(int n, int a[n])
{
	n_visited++;

    int perm_cost = 0;
    for(int i=0; i < n; i++)
    	perm_cost += cost[i][a[i]];

    if(perm_cost < min_cost)
    {
    	min_cost = perm_cost;
    	for(int i = 0; i < n; i++)
    		min_cost_assignment[i] = a[i];
    }

    if(perm_cost > max_cost)
    {
    	max_cost = perm_cost;
    	for(int i = 0; i < n; i++)
    		max_cost_assignment[i] = a[i];
    }
    histogram[perm_cost] += 1;
}

static void generate_all_permutations(int n,int m,int a[n])
{
  
  if(m < n - 1)
  {
    //
    // not yet at the end; try all possibilities for a[m]
    //
    for(int i = m;i < n;i++)
    {
#define swap(i,j)  do { int t = a[i]; a[i] = a[j]; a[j] = t; } while(0)
      swap(i,m);                            // exchange a[i] with a[m]
      generate_all_permutations(n,m + 1,a); // recurse
      swap(i,m);                            // undo the exchange of a[i] with a[m]
#undef swap
    }
  }
  else
  {

    evaluate_permutation(n,a);
  }
}

static void generate_all_permutations_branch_and_bound(int n, int m,int a[n], int parcial_cost)
{
	if(min_cost < parcial_cost + 3*(n - m))
		return ;

	if(m < n - 1)
	{
		for(int i = m;i < n;i++)
		{
#define swap(i,j)  do { int t = a[i]; a[i] = a[j]; a[j] = t; } while(0)
			swap(i,m);                            // exchange a[i] with a[m]
			generate_all_permutations_branch_and_bound(n,m + 1,a, parcial_cost + cost[m][a[m]]); // recurse
			swap(i,m);                            // undo the exchange of a[i] with a[m]
#undef swap
		}
	}
	else
	{
		n_visited++;
	 int total_cost = parcial_cost+cost[m][a[m]];
		
		if(min_cost > total_cost){
			min_cost = total_cost;
			for(int i = 0; i < n; i++)
				min_cost_assignment[i] = a[i];
		}
		
	}
}

static void max_cost_branch_and_bound(int n, int m,int a[n], int parcial_cost)
{
  if(max_cost > parcial_cost + 60*(n - m )) //minimum cost of each cell of the cost table is 3 *(n-m)
    return ; // pass to next m 

  if(m < n - 1)
  {
    for(int i = m;i < n;i++)
    {
#define swap(i,j)  do { int t = a[i]; a[i] = a[j]; a[j] = t; } while(0)
      swap(i,m);                            // exchange a[i] with a[m]
      max_cost_branch_and_bound(n,m + 1,a, parcial_cost + cost[m][a[m]]); // recurse
      swap(i,m);                            // undo the exchange of a[i] with a[m]
#undef swap
    }
  }
  else
  {
    int total_cost = parcial_cost+cost[m][a[m]];
    
    if(max_cost < total_cost){
      max_cost = total_cost;
      for(int j = 0; j < n; j++)
        max_cost_assignment[j] = a[j];
    }
    
    n_visited++;
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// main program
//

int main(int argc,char **argv)
{
  if(argc == 2 && argv[1][0] == '-' && argv[1][1] == 'e')
  {
    seed = 0;
    {
      int n = 3;
      init_costs(-3); // costs for the example with n = 3
      int a[n];
      for(int i = 0;i < n;i++)
        a[i] = i;
      reset_solutions();
      (void)elapsed_time();
      generate_all_permutations(n,0,a);
      cpu_time = elapsed_time();
      show_solutions(n,"Example for n=3",show_all);
      printf("\n");
    }
    {
      int n = 5;
      init_costs(-5); // costs for the example with n = 5
      int a[n];
      for(int i = 0;i < n;i++)
        a[i] = i;
      reset_solutions();
      (void)elapsed_time();
      generate_all_permutations(n,0,a);
      cpu_time = elapsed_time();
      show_solutions(n,"Example for n=5",show_all);
      return 0;
    }
  }
  if(argc == 2)
  {
    seed = atoi(argv[1]); // seed = student number
    if(seed >= 0 && seed <= 1000000)
    {
      for(int n = 1;n <= max_n;n++)
      {
        init_costs(n);
        show_solutions(n,"Problem statement",show_info_1 | show_costs);
        //
        // 2.
        //
        if(n <= 1)			//Brute Force Method
        {
          int a[n];
          for(int i = 0;i < n;i++)
            a[i] = i; // initial permutation
          reset_solutions();
          (void)elapsed_time();
          generate_all_permutations(n,0,a);
          cpu_time = elapsed_time();
          if(n<=1){		//chose values of n to display histogram
          	show_solutions(n,"Brute force",show_info_2 | show_min_solution | show_max_solution);
          } else{
          	show_solutions(n,"Brute force",show_info_2 | show_min_solution | show_max_solution | show_histogram);
          }
        }
#if 1
        if(n <= 1)			//Branch and Bound Method
        {
          int a[n];
          for(int i = 0;i < n;i++)
            a[i] = i; // initial permutation
          reset_solutions();
          (void)elapsed_time();
          generate_all_permutations_branch_and_bound(n,0,a,0);
          cpu_time = elapsed_time();
          show_solutions(n,"Brute force with branch-and-bound",show_info_2 | show_min_solution);
        }
#endif
         if(n<=1){
          int a[n];
          for(int i = 0;i < n;i++)
            a[i] = i; // initial permutation
          reset_solutions();
          (void)elapsed_time();
          max_cost_branch_and_bound(n,0,a,0);
          cpu_time = elapsed_time();
          show_solutions(n,"Max cost with branch-and-bound",show_info_2 | show_max_solution);
        }
        

        if (n <=14)			//Random Permutations Method
        {
        	int a[n];
        	reset_solutions();
        	(void)elapsed_time();
        	for (int i =0; i<1000000;i++)
        	{
        		random_permutation(n,a);
        		evaluate_permutation(n,a);
        	}
        	cpu_time = elapsed_time();
        	if(n<=12){		//chose values of n to display histogram
          		show_solutions(n,"Brute force with random permutations",show_info_2 | show_min_solution | show_max_solution);
          	} else{
          		show_solutions(n,"Brute force with random permutations",show_info_2 | show_min_solution | show_max_solution | show_histogram);
          	}

		}

        //
        // done
        //
        printf("\n");
      }
      return 0;
    }
  }
  fprintf(stderr,"usage: %s -e              # for the examples\n",argv[0]);
  fprintf(stderr,"usage: %s student_number\n",argv[0]);
  return 1;
}