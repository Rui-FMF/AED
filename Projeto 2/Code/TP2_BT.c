#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#define initial_size 8192

//BINARY TREES IMPLEMENTATION

int unique_word_count = 0;
int total_words = 0;

typedef  struct node {
	struct node *left; //aponta para o proximo nó à esqueda
	struct node *right; //aponta para o próximo nó à direita
	char word[64];  //palavra associada
	int firstPos;
	int prevWordPos;  // index da ultima vez que esta palavra ocorreu (no ficheiro)
	int totaldistance;
	int mindistance; //distancia minima entre ocorrencias 
	int maxdistance; //distancia maxima entre ocorrencias
	int counter; // contar o numero de ocorrencias 
} node;

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

unsigned int hash_function(const char *str,unsigned int s)
{
	unsigned int h;
	for(h = 0u;*str != '\0' ;str++)
	h = 157u * h + (0xFFu & (unsigned int)*str); // arithmetic overflow may occur here (just ignore it!)
	return h % s; // due to the unsigned int data type, it is guaranteed that 0 <= h % s < s
}

static node *initNewNode(int pos,char *word){
	node *newnode = (node *)malloc(sizeof(node));
	if(newnode == NULL){
		fprintf(stderr,"Out of memory\n");
		exit(1);
	}
	strcpy(newnode->word,word);
	newnode->firstPos=pos;
	newnode->prevWordPos=pos;
	newnode->counter=1;
	newnode->mindistance=INT_MAX;
	newnode->maxdistance=0;
	newnode->totaldistance=0;
	newnode->left=NULL;
	newnode->right=NULL;
	return newnode;
}
void updateNode(node * n,int pos){
	
	int tempdist=pos-n->prevWordPos;
	n->counter+=1;
	n->totaldistance+=tempdist;
	if(tempdist>n->maxdistance)
		n->maxdistance=tempdist;
	if(tempdist<n->mindistance)
		n->mindistance=tempdist;
	n->prevWordPos=pos;
}

 node** add_word(char *key ,node**hashtable, int hash_size, int pos){
 		unsigned int idx;
 		node *root;//root da binary tree
		idx = hash_function(key,hash_size);
		root=hashtable[idx];

		if(root == NULL){
    		hashtable[idx] = initNewNode(pos,key);
    		unique_word_count++;
    		return hashtable;
		}
		 while(1){
	    	int c = strcmp(key,root->word);
	    	if     (c <  0) { if(root->left  == NULL) { root->left  = initNewNode(pos,key); unique_word_count++; return hashtable; } else root = root->left;  }
	    	else if(c == 0) { updateNode(root,pos); return hashtable;}
	    	else            { if(root->right == NULL) { root->right = initNewNode(pos,key); unique_word_count++; return hashtable; } else root = root->right; }
	  	}
 }

 node** add_resize(node* old_root, node**hashtable, int hash_size){
 	unsigned int idx;
 	idx=hash_function(old_root->word,hash_size);

 	node *tmp = (node *)malloc(sizeof(node));
 	tmp->left = NULL;
 	tmp->right = NULL;
	strcpy(tmp->word, old_root->word);
	tmp->firstPos = old_root->firstPos;
	tmp->prevWordPos = old_root->prevWordPos;
	tmp->totaldistance = old_root->totaldistance;
	tmp->mindistance = old_root->mindistance;
	tmp->maxdistance = old_root->maxdistance;
	tmp->counter = old_root->counter;

 	node *new_root = hashtable[idx];

 	if(new_root == NULL){
    		hashtable[idx] = tmp;
	}else{
		while(1){
	    	int c = strcmp(old_root->word,new_root->word);
	    	if     (c <  0) { if(new_root->left  == NULL) { new_root->left  = tmp; break; } else new_root = new_root->left;  }
	    	else if(c == 0) { break;}
	    	else            { if(new_root->right == NULL) { new_root->right = tmp; break; } else new_root = new_root->right; }
	  	}
	}

	if(old_root->left!=NULL){
		hashtable = add_resize(old_root->left, hashtable, hash_size);
	}
	if(old_root->right!=NULL){
		hashtable = add_resize(old_root->right, hashtable, hash_size);
	}
	return hashtable;
 }

 node** resizeTable(int old_size, node**hashtable, int new_size){
	//initialize new Hash Table
	node** new_hashtable = calloc(new_size, sizeof(node));
	//fill new Hash Table with old values
	int hashcode=0;
	for(int i=0;i<old_size;i++){
		if (hashtable[i]!=NULL){
			new_hashtable = add_resize(hashtable[i], new_hashtable, new_size);
		}
	}
	free(hashtable);
	return new_hashtable;
}

void printCount(node * node){

	printf("%s  ", node->word);
	printf("COUNT: %d  \n\n", node->counter);
	if(node->left!=NULL){
		printCount(node->left);
	}
	if(node->right!=NULL){
		printCount(node->right);
	}
}

void printPositions(node * node){

	printf("%s  ", node->word);
	printf("FIRST POS: %d   ", node->firstPos);
	printf("LAST POS: %d\n\n", node->prevWordPos);
	if(node->left!=NULL){
		printPositions(node->left);
	}
	if(node->right!=NULL){
		printPositions(node->right);
	}
}

void printDistances(node * node){

	printf("%s  ", node->word);
	printf("MAX DISTANCE: %d  ", node->maxdistance);
	if(node->mindistance==INT_MAX){
		printf("MIN DISTANCE: 0   ");
		printf("Average DISTANCE: 0\n\n");
	}else{
		printf("MIN DISTANCE: %d   ", node->mindistance);
		printf("AVERAGE DISTANCE: %d\n\n", (int)( (node->totaldistance)/(node->counter-1) ));
	}
	if(node->left!=NULL){
		printDistances(node->left);
	}
	if(node->right!=NULL){
		printDistances(node->right);
	}
}

 void printInfo(node * node){

	printf("%s  ", node->word);
	printf("FIRST POS: %d  ", node->firstPos);
	printf("LAST POS: %d  ", node->prevWordPos);
	printf("COUNT: %d  ", node->counter);
	printf("MAX DISTANCE: %d ", node->maxdistance);
	if(node->mindistance==INT_MAX){
		printf("MIN DISTANCE: 0  ");
		printf("Average DISTANCE: 0\n\n");
	}else{
		printf("MIN DISTANCE: %d  ", node->mindistance);
		printf("AVERAGE DISTANCE: %d\n\n", (int)( (node->totaldistance)/(node->counter-1) ));
	}
	if(node->left!=NULL){
		printInfo(node->left);
	}
	if(node->right!=NULL){
		printInfo(node->right);
	}
}


int main(int argc,char *argv[]){

	//OPTIONS
	int opt = -1;
	  if(argc >= 2 && strcmp(argv[1],"-a") == 0) opt = 'a';
	  if(argc >= 2 && strcmp(argv[1],"-u") == 0) opt = 'u';
	  if(argc >= 2 && strcmp(argv[1],"-c") == 0) opt = 'c';
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
	node ** hashtable=calloc(initial_size,sizeof(node));
	int hash_size = initial_size;

	file_data_t *fd = (file_data_t *)malloc(sizeof(file_data_t));

	if(open_text_file(argv[2],fd)==-1){
		exit(1);
	}	

	while(fd->is_end == 0){
		read_word(fd);
		total_words++;
		if(unique_word_count>((int)hash_size*0.8))
		{
			hashtable = resizeTable(hash_size,hashtable,(int)hash_size*1.5);
			hash_size = (int)hash_size*1.5;
		}
		hashtable=add_word(fd->word,hashtable,hash_size,fd->word_pos);
	}

	//OPTION TREATMENT
	  switch(opt)
      {
      case 'a': printf("The file %s contains %d words\n",argv[2],total_words); break;
      case 'u': printf("The file %s contains %d unique words\n",argv[2],unique_word_count); break;
      case 'c': printf("Contents of the file %s:\n",argv[2]); for(int i=0;i<hash_size;i++) if(hashtable[i]!=NULL) printCount(hashtable[i]); break;
      case 'p': printf("Contents of the file %s:\n",argv[2]); for(int i=0;i<hash_size;i++) if(hashtable[i]!=NULL) printPositions(hashtable[i]); break;
      case 'd': printf("Contents of the file %s:\n",argv[2]); for(int i=0;i<hash_size;i++) if(hashtable[i]!=NULL) printDistances(hashtable[i]); break;
      case 'i': printf("Contents of the file %s:\n",argv[2]); for(int i=0;i<hash_size;i++) if(hashtable[i]!=NULL) printInfo(hashtable[i]); break;
      }
}
