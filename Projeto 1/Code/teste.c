#include <stdio.h>
#include <string.h>
int  main(){

int n = 13;
char s[5];
sprintf(s, "%d", n);

char str[80];
strcpy(str, "histogram");
strcat(str, s);
strcat(str, ".txt");
printf("%s", str);

int arr[3] = {101, 203, 303};
FILE *fp;

fp = fopen(str, "w");
for(int i=0;i<3; i++){
fprintf(fp,"%d,",arr[i]);
}
fclose(fp);
return 0;
}

if (n >=10){
        	int a[n];
        	reset_solutions();
        	(void)elapsed_time();
        	for (int i =0; i<100000;i++){
      			int temporarycost=0;
      			random_permutation(n,a);
				  	for(int i =0; i<n;i++){
				  		temporarycost+=cost[i][a[i]];
				  	}
				  	if(temporarycost<min_cost){
				  		min_cost=temporarycost;
				  		for(int k=0;k<n;k++)
				  			min_cost_assignment[k]=a[k];
				  		}
				  	if(temporarycost>max_cost){
					max_cost=temporarycost;
				  		for (int k = 0; k < n; k++){
				  			max_cost_assignment[k]=a[k];
				  	}
  				}
        	}
        		cpu_time = elapsed_time();
      			show_solutions(n," Brute Force Random Permutations",show_info_2 | show_min_solution | show_max_solution);

}



if (n <=12)
        {
        	int a[n];
        	reset_solutions();
        	(void)elapsed_time();
        	for (int i =0; i<100000;i++)
        	{
        		random_permutation(n,a);
        		evaluate_permutation(n,a);
        	}
        	cpu_time = elapsed_time();
      		show_solutions(n," Brute Force Random Permutations",show_info_2 | show_min_solution | show_max_solution);

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
		int total_cost = 0;
		for(int i = 0; i < n; i++)
			total_cost += cost[i][a[i]];
		
		if(min_cost > total_cost){
			min_cost = total_cost;
			for(int j = 0; j < n; j++)
				min_cost_assignment[j] = a[j];
		}
		
		n_visited++;
	}
}