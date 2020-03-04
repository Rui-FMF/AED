static void export_histogram(int hist[], int n, int seed){

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
