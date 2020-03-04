#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#define initial_size 8192

int unique_word_count = 0;
int total_words = 0;

typedef struct data_cell
{
	struct data_cell *next; //aponta pra proxima palavra
	char word[64];  //palavra associada
	int firstPos;
	int prevWordPos;  // index da ultima vez que esta palavra ocorreu (no ficheiro)
	int totaldistance;
	int mindistance; //distancia minima entre ocorrencias 
	int maxdistance; //distancia maxima entre ocorrencias
	int counter; // contar o numero de ocorrencias 
} data_cell;

typedef struct file_data
{
	// public data
	long word_pos;    // zero-based
	long word_num;    // zero-based
	char word[64];
	// private data 
	FILE *fp;
	long current_pos; // zero-based
	int is_end;
}
file_data_t;


int open_text_file(char *file_name,file_data_t *fd)
{
	fd->fp = fopen(file_name,"rb");
	if(fd->fp == NULL){
		fprintf(stderr,"unable to open file %s\n",file_name);
		return -1;
	}
	fd->word_pos = -1;
	fd->word_num = -1;
	fd->word[0] ='\0';
	fd->current_pos = -1;
	fd->is_end = 0;
	return 0;
}

void close_text_file(file_data_t *fd)
{
	fclose(fd->fp);
	fd->fp = NULL;
}

int read_word(file_data_t *fd)
{
	int i,c;
	// skip white spaces
	do
	{
		c = fgetc(fd->fp);
		if(c == EOF){
			fd->is_end = 1;
			return -1;
		}
		fd->current_pos++;
	}while(c <= 32);
	// record word
	fd->word_pos = fd->current_pos;
	fd->word_num++;
	fd->word[0] = (char)c;
	for(i = 1;i < (int)sizeof(fd->word) - 1;i++)
	{
		c = fgetc(fd->fp);
		if(c == EOF){
			fd->is_end = 1;
			break; // end of file
		}
		fd->current_pos++;
		if(c <= 32)
			break; // terminate word
		fd->word[i] = (char)c;
	}
	fd->word[i] ='\0';
	return 0;
}


// função de hash....
unsigned int hash_function(const char *str,unsigned int s)
{
	unsigned int h;
	for(h = 0u;*str != '\0' ;str++)
	h = 157u * h + (0xFFu & (unsigned int)*str); // arithmetic overflow may occur here (just ignore it!)
	return h % s; // due to the unsigned int data type, it is guaranteed that 0 <= h % s < s
}
//...inicializar cada posiçao da linked list
static data_cell *initNewCell(int pos,char *word){
	data_cell *newcell = (data_cell *)malloc(sizeof(data_cell));
	if(newcell == NULL){
		fprintf(stderr,"Out of memory\n");
		exit(1);
	}
	strcpy(newcell->word,word);
	newcell->firstPos=pos;
	newcell->prevWordPos=pos;
	newcell->counter=1;
	newcell->mindistance=INT_MAX;
	newcell->maxdistance=0;
	newcell->totaldistance=0;
	newcell->next=NULL;
	return newcell;
}

//procurar informação
data_cell *find_data(const char *key,data_cell*hashtable[],int hash_size){
	unsigned int idx; //index-> hash code posteriormente calculado
	data_cell *dc;//head da linked list
	idx = hash_function(key,hash_size);
	dc=hashtable[idx];
	while(dc!=NULL && strcmp(key,dc->word)!=0){
		dc=dc->next;
	}
	return dc;
}
//
void updateCell(data_cell * cell,int pos){
	
	int tempdist=pos-cell->prevWordPos;
	cell->counter+=1;
	cell->totaldistance+=tempdist;
	if(tempdist>cell->maxdistance)
		cell->maxdistance=tempdist;
	if(tempdist<cell->mindistance)
		cell->mindistance=tempdist;
	cell->prevWordPos=pos;
}

data_cell** resizeTable(int old_size, data_cell**hashtable, int new_size){
	//initialize new Hash Table
	data_cell** new_hashtable = calloc(new_size, sizeof(data_cell));
	//fill new Hash Table with old values
	int hashcode=0;
	for(int i=0;i<old_size;i++){
		for(data_cell* h = hashtable[i]; h!=NULL; h=h->next){
			hashcode=hash_function(h->word,new_size);

			data_cell *head = new_hashtable[hashcode], *previous = NULL;
			data_cell *tmp_h = (data_cell *)malloc(sizeof(data_cell));

			tmp_h->next = NULL;
	        strcpy(tmp_h->word, h->word);
	        tmp_h->firstPos = h->firstPos;
	        tmp_h->prevWordPos = h->prevWordPos;
	        tmp_h->totaldistance = h->totaldistance;
	        tmp_h->mindistance = h->mindistance;
	        tmp_h->maxdistance = h->maxdistance;
		    tmp_h->counter = h->counter;
		    
		    while (head!=NULL) {
		        previous = head;
		        head = head->next;
		    }
		    if (previous!=NULL){
		        previous->next = tmp_h;
		    }else{
		        new_hashtable[hashcode] = tmp_h;
		    }
		}
	}
	free(hashtable);
	return new_hashtable;
}


void printCount(data_cell * cell){

	printf("%s  ", cell->word);
	printf("COUNT: %d  \n\n", cell->counter);
	if(cell->next!=NULL){
		printCount(cell->next);
	}
}

void printPositions(data_cell * cell){

	printf("%s  ", cell->word);
	printf("FIRST POS: %d   ", cell->firstPos);
	printf("LAST POS: %d\n\n", cell->prevWordPos);
	if(cell->next!=NULL){
		printPositions(cell->next);
	}
}

void printDistances(data_cell * cell){

	printf("%s  ", cell->word);
	printf("MAX DISTANCE: %d  ", cell->maxdistance);
	if(cell->mindistance==INT_MAX){
		printf("MIN DISTANCE: 0   ");
		printf("Average DISTANCE: 0\n\n");
	}else{
		printf("MIN DISTANCE: %d   ", cell->mindistance);
		printf("AVERAGE DISTANCE: %d\n\n", (int)( (cell->totaldistance)/(cell->counter-1) ));
	}
	if(cell->next!=NULL){
		printDistances(cell->next);
	}
}

void printInfo(data_cell * cell){

	printf("%s  ", cell->word);
	printf("FIRST POS: %d  ", cell->firstPos);
	printf("LAST POS: %d  ", cell->prevWordPos);
	printf("COUNT: %d  ", cell->counter);
	printf("MAX DISTANCE: %d ", cell->maxdistance);
	if(cell->mindistance==INT_MAX){
		printf("MIN DISTANCE: 0  ");
		printf("Average DISTANCE: 0\n\n");
	}else{
		printf("MIN DISTANCE: %d  ", cell->mindistance);
		printf("AVERAGE DISTANCE: %d\n\n", (int)( (cell->totaldistance)/(cell->counter-1) ));
	}
	if(cell->next!=NULL){
		printInfo(cell->next);
	}
}

int main(int argc, char * argv[])
{

	//OPTIONS
	int opt = -1;
	  if(argc >= 2 && strcmp(argv[1],"-a") == 0) opt = 'a'; // all words
	  if(argc >= 2 && strcmp(argv[1],"-u") == 0) opt = 'u'; // unique words
	  if(argc >= 2 && strcmp(argv[1],"-c") == 0) opt = 'c'; // list words
	  if(argc >= 2 && strcmp(argv[1],"-p") == 0) opt = 'p';
	  if(argc >= 2 && strcmp(argv[1],"-d") == 0) opt = 'd';
	  if(argc >= 2 && strcmp(argv[1],"-i") == 0) opt = 'i';
	  if(opt < 0)
	  {
	    fprintf(stderr,"\e[5;31m"); // blink on (may not work in some text terminals), text in red
	    fprintf(stderr,"usage: %s -a text_file  # count how many words are in the file\n",argv[0]);
	    fprintf(stderr,"       %s -u text_file  # count how many unique words are in the file\n",argv[0]);
	    fprintf(stderr,"       %s -c text_file  # list words and their count\n",argv[0]);
	    fprintf(stderr,"       %s -p text_file  # list words and their positions\n",argv[0]);
	    fprintf(stderr,"       %s -d text_file  # list words and their distances\n",argv[0]);
	    fprintf(stderr,"       %s -i text_file  # list words and all info\n",argv[0]);
	    fprintf(stderr,"\e[0m"); // normal output
	    return 1;
	  }

	//MAIN PROGRAM
	data_cell** hashtable = calloc(initial_size, sizeof(data_cell));
	int hash_size = initial_size;

	// read all words and put them on the hash table
	data_cell *tmp=NULL;
	int hashcode=0;
	file_data_t *fd = (file_data_t *)malloc(sizeof(file_data_t));

	if(open_text_file(argv[2],fd)==-1){
		exit(1);
	}

	while(fd->is_end==0){
		if (unique_word_count>((int)hash_size*0.8))
		{
			hashtable = resizeTable(hash_size,hashtable,(int)(hash_size*1.5));
			hash_size = (int)(hash_size*1.5);
		}
		read_word(fd);
		total_words++;
		tmp = find_data(fd->word,hashtable,hash_size);
		if(tmp==NULL)
		{
			unique_word_count++;
			hashcode=hash_function(fd->word,hash_size);

			data_cell *head = hashtable[hashcode], *previous = NULL;
		    while (head!=NULL) {
		        previous = head;
		        head = head->next;
		    }

		    data_cell *nd = initNewCell(fd->word_pos,fd->word);

		    if (previous!=NULL) previous->next = nd;
		    else hashtable[hashcode] = nd;
		
		}
		else updateCell(tmp,fd->word_pos);
	}

	//OPTION TREATMENT
	  switch(opt)
      {
      case 'a': printf("The file %s contains %d words\n",argv[2],total_words);; break;
      case 'u': printf("The file %s contains %d unique words\n",argv[2],unique_word_count); break;
      case 'c': printf("Contents of the file %s:\n",argv[2]); for(int i=0;i<hash_size;i++) if(hashtable[i]!=NULL) printCount(hashtable[i]); break;
      case 'p': printf("Contents of the file %s:\n",argv[2]); for(int i=0;i<hash_size;i++) if(hashtable[i]!=NULL) printPositions(hashtable[i]); break;
      case 'd': printf("Contents of the file %s:\n",argv[2]); for(int i=0;i<hash_size;i++) if(hashtable[i]!=NULL) printDistances(hashtable[i]); break;
      case 'i': printf("Contents of the file %s:\n",argv[2]); for(int i=0;i<hash_size;i++) if(hashtable[i]!=NULL) printInfo(hashtable[i]); break;
      }
	
	close_text_file(fd);
	free(fd);

}
