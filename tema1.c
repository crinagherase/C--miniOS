#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;

typedef struct Dir{
	char *name;
	struct Dir* parent;
	struct File* head_children_files;
	struct Dir* head_children_dirs;
	struct Dir* next;
} Dir;

typedef struct File {
	char *name;
	struct Dir* parent;
	struct File* next;
} File;

void touch (Dir* parent, char* name) {
	struct File* helper = parent->head_children_files;
	struct Dir* helper2 = parent->head_children_dirs;
	File* prev_file = helper;
	int nuExista = 1;
	//verific daca exista fisier cu acelasi nume
	while( helper ){
		prev_file = helper;
		if (strcmp(helper->name, name) == 0)
			nuExista = 0;
		helper = helper->next;
	}
	//verific daca exista director cu acelasi nume
	while( helper2 ){
		if (strcmp(helper2->name, name) == 0)
			nuExista = 0;
		helper2 = helper2->next;
	}
	if(nuExista == 0)
		printf("File already exists\n");
	else {
		helper = (File*)malloc(sizeof(File));
		helper->name = (char*)malloc(strlen(name) * sizeof(char));
		//daca nu e primul copil-fisier fac legaturile
		if(prev_file != NULL)
			prev_file->next = helper;
		strcpy(helper->name, name);
		helper->parent = parent;
		helper->next = NULL;
	}
}

void mkdir (Dir* parent, char* name) {
	struct File* helper = parent->head_children_files;
	struct Dir* helper2 = parent->head_children_dirs;
	Dir* prev_dir = helper2;
	int nuExista = 1;
	printf("A intrat %s", parent->name);
	//verific daca exista fisier cu acelasi nume
	while( helper ){
		printf("A");
		prev_dir = helper2;
		if (strcmp(helper->name, name) == 0)
			nuExista = 0;
		helper = helper->next;
	}
	//verific daca exista director cu acelasi nume
	while( helper2 ){
		printf("B");printf("%s\n",helper->name);
		if (strcmp(helper2->name, name) == 0)
			nuExista = 0;
		helper2 = helper2->next;
	}
	if(nuExista == 0)
		printf("File already exists\n");
	else{
		helper2 = (Dir*)malloc(sizeof(Dir));
		helper2->name = (char*)malloc(strlen(name) * sizeof(char));
		strcpy(helper2->name, name);
		if(prev_dir != NULL)
			prev_dir->next = helper2;
		helper2->head_children_dirs = NULL;
		helper2->head_children_files = NULL;
		helper2->parent = parent;
		helper2->next = NULL;
	}
}

void ls (Dir* parent) {
	struct File* pFisiere = parent->head_children_files;
	struct Dir* pDirectoare = parent->head_children_dirs;
	while(pDirectoare){
		printf("%s\n", pDirectoare->name);
		pDirectoare = pDirectoare->next;
	}
	while(pFisiere){
		printf("%s\n", pFisiere->name);
		pFisiere = pFisiere->next;
	}
}

void rm (Dir* parent, char* name) {
	struct File* helper = parent->head_children_files;
	struct File* prev = parent->head_children_files;
	int nuExista = 1;
	//caut fisierul ce trebuie sters
	while(helper){
		if (strcmp(helper->name, name) == 0){
			nuExista = 0;
			break;
		}
		prev = helper;
		helper = helper->next;
	}
	if(nuExista == 1)
		printf("Could not find the file\n");
	else{
		//daca fisierul ce trebuie sters nu este nici primul, nici ultimul din lista
		if(strcmp(prev->name, helper->name) != 0 && helper->next != NULL){
			prev->next = helper->next;
		}
		free(helper->name);
		free(helper);
	}
}

void rmdir (Dir* parent, char* name) {
	struct Dir* helper = parent->head_children_dirs;
	struct Dir* prev = parent->head_children_dirs;
	int nuExista = 1;
	//caut directorul ce trebuie sters
	while(helper){
		if (strcmp(helper->name, name) == 0){
			nuExista = 0;
			break;
		}
		prev = helper;
		helper = helper->next;
	}
	if(nuExista == 1)
		printf("Could not find the dir\n");
	else{
		//golesc directorul recursiv
		struct File* fis = helper->head_children_files;
		struct Dir* dirs = helper->head_children_dirs;
		while( fis ){
			rm( helper, fis->name);
			fis = fis->next;
		}
		while( dirs ){
			rmdir( helper, dirs->name);
			dirs = dirs->next;
		}
		//daca directorul ce trebuie sters nu este nici primul, nici ultimul din lista
		if(strcmp(prev->name, helper->name) != 0 && helper->next != NULL){
			prev->next = helper->next;
		}
		free(helper->name);
		free(helper);
	}
}

void cd(Dir** target, char *name) {
	struct Dir* dirs = (*target)->head_children_dirs;
	if(strcmp(name, "..") == 0 && strcmp((*target)->parent->name,"home") != 0)
		*target = (*target)->parent;
	else{
		while( dirs ){
			//caut directorul
			if(strcmp(name, dirs->name) == 0){
				*target = dirs;
				break;
			}
			dirs = dirs->next;
		}
	}
}

char *pwd (Dir* target) {
	int i,j;
	char *sir = malloc(MAX_INPUT_LINE_SIZE * sizeof(char));
	//aloc o matrice de siruri pentru a putea salva numele fiecarui director
	char **matrix = (char**) calloc(50, sizeof(char*));
	for(i = 0; i < 50; i++){
		matrix[i] = (char*) calloc(50, sizeof(char));
	}
	i=0;
	while( target ){
		strcpy(matrix[i],"/");
		strcpy(matrix[i]+1, target->name);
		i++;
		target = target->parent;
	}
	//pun numele in ordine inversa pentru a forma o cale
	for(j = i-1; j >= 0; j--){
		strcat(sir, matrix[j]);
	}
	for(j = 0; j < 50; j++){
		free(matrix[j]);
	}
	free(matrix);
	return sir;
}

void stop (Dir* target) {
	if(target == NULL)
		return;
	//eliberez fiecare fisier din directorul curent
	while(target->head_children_files){
		File* helper = target->head_children_files;
		target->head_children_files = target->head_children_files->next;
		free(helper->name);
		free(helper);
	}
	//eliberez recursiv copiii director
	while(target->head_children_dirs){
		Dir* helper2 = target->head_children_dirs;
		stop(helper2);
		target->head_children_dirs = target->head_children_dirs->next;
	}
	free(target->name);
	free(target);
}

void tree (Dir* target, int level) {
	struct Dir* ptd = target->head_children_dirs;
	struct File* ptf = target->head_children_files;
	int i;
	while( ptd ){
		for(i = 0; i < level; i++)
			printf("    ");
		printf("%s\n", ptd->name);
		tree(ptd, level+1);
		ptd = ptd->next;
	}
	while( ptf ){
		for(i = 0; i < level; i++)
			printf("    ");
		printf("%s\n", ptf->name);
		ptf = ptf->next;
	}
}

void mv(Dir* parent, char *oldname, char *newname) {}

int main () {
	int k = 0;
	Dir* home = (Dir*)malloc(sizeof(Dir));
	//creez directorul home
	home->name = (char*)malloc(5 * sizeof(char));
	strcpy(home->name, "home");
	home->parent = NULL;
	home->head_children_dirs = NULL;
	home->head_children_files = NULL;
	home->next = NULL;
	Dir *whereami = home;
	do
	{
		char *buf = malloc(MAX_INPUT_LINE_SIZE * sizeof(char));
		fgets(buf, MAX_INPUT_LINE_SIZE, stdin);
		buf = realloc(buf, strlen(buf)+1);
		char *command = malloc (strlen(buf) * sizeof(char));
		command = strtok(buf, " ");
		printf("comanda:%s\n", command);
		if( strcmp(command, "touch") == 0 )
			{
				char *name = malloc (strlen(buf) * sizeof(char));
				name = strtok(NULL, " ");
				touch(whereami, name);
				free(name);
			}
		if( strcmp(command, "mkdir") == 0 )
			{
				//printf("%s ", whereami->name);
				//char *name = malloc (strlen(buf) * sizeof(char));
				char *name = strtok(NULL, " ");
				printf("numele: %s", name);
				mkdir(whereami, name);
				//free(name);
				//printf("CCEE");
			}
		if( strcmp(command, "rm") == 0 )
			{
				char *name = malloc (strlen(buf) * sizeof(char));
				name = strtok(NULL, " ");
				rm(whereami, name);
				free(name);
			}
		if( strcmp(command, "rmdir") == 0 )
			{
				char *name = malloc (strlen(buf) * sizeof(char));
				name = strtok(NULL, " ");
				rmdir(whereami, name);
				free(name);
			}
		if( strcmp(command, "cd") == 0 )
			{
				char *name = malloc (strlen(buf) * sizeof(char));
				name = strtok(NULL, " ");
				cd(&whereami, name);
				free(name);
			}
		if( strcmp(command, "ls") == 0 )
			{
				printf("PTLSS");
				ls(whereami);
			}
		if( strcmp(command, "pwd") == 0 )
			{
				pwd(whereami);
			}
		if( strcmp(command, "tree") == 0 )
			{
				tree(whereami, 0);
			}
		if( strcmp(command, "stop") == 0 )
			{
				printf("a intrat la stop");
				k = 1;
				stop(home);
			}
		free(buf);
		//free(command);
	} while (k == 0);
	
	return 0;
}