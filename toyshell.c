#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <unistd.h>

void list(int argc, char **argv) {
	DIR *dir;
	struct dirent* dent;
	int n=0;
	if(argc != 3) {
		printf("Error: invalid no. of arguments.\n");
		return;
	}

	dir = opendir(argv[2]);

	if(strcmp("f",argv[1])==0) {
		while((dent=readdir(dir)) != NULL) {
			if(dent->d_type == DT_REG)
				printf("%s\n", dent->d_name);
		}
	} else if(strcmp("n",argv[1])==0) {
		while((dent=readdir(dir))!=NULL) {
			n++;
		}
		printf("No. of entries: %d\n",n);
	} else if(strcmp("i",argv[1])==0) {
		while((dent=readdir(dir))!=NULL) {
			printf("%s : %d\n",dent->d_name, dent->d_ino);
		}
	} else {
		printf("Error. invalid argument.\n");
	}
}

void typeline(int argc, char **argv) {
	FILE *fp;
	int n=1,x=0;
	char st[100];
	if(argc!= 3) {
		printf("Invalid number of arguments\n");
		return;
	}
	fp=fopen(argv[2],"r");
	if(strcmp("a",argv[1])==0) {
		while(!feof(fp)) {
			fgets(st,100,fp);
			printf("%d :%s",n++,st);
		}
	} else if(argv[1][0]=='+'){
		x=atoi(argv[1]);
		while(!feof(fp) && n <= x) {
			fgets(st, 100, fp);
			printf("%d : %s", n++,st);
		}
	}
	else if(argv[1][0]=='-') {
		x=atoi(argv[1]);
		while(!feof(fp)) {
			n++;
			fgets(st,100,fp);
		}
		x=n+x;//to calculate the position of the required line
		rewind(fp);
		n=1;
		while(!feof(fp) && n++<x)
			fgets(st,100,fp);
		while(!feof(fp)){
			fgets(st,100,fp);
			printf("%d : %s",n++,st);
		}

	}
}

int makeTokens(char *str, char **argv) {
	char *s=NULL;
	int i=0;

	s = strtok(str," ");
	while(s != NULL) {
		argv[i++] = s;
		s=strtok(NULL, " ");
	}

	return i;
}

void main(int argc, char **argv) {

	char str[100];
	int i, id; 

	printf("prog: %s\n",argv[0]);
	
	while(1) {
		printf("$ ");
		fgets(str, 100, stdin);
		if(str[0]=='\n') continue;

		argc=makeTokens(str,argv);
		argv[argc]=NULL;
		/*for(i=0;i<argc; i++) {
			printf("'%s' ", argv[i]);
		}
		printf("\n");*/

		//eliminate \n problem
		i=strlen(argv[argc-1])-1;
		if(argv[argc-1][i]=='\n') {
			argv[argc-1][i]='\0';
		}

		id=fork();
		if(id==0) { //child
			if(strcmp("list",argv[0])==0) {
				list(argc, argv);
				return;
			} else if(strcmp("typeline",argv[0])==0) {
				typeline(argc, argv);
			}
			else if(execvp(argv[0], argv)==-1) {
				printf("command not found. :(\n");
				return;
			}
		} else if(id < 0) { //process not created
			printf("Error: couldn't create child process.\n");
		} else { //parent
			//printf("parent %d\n",id);
			wait(NULL); //synchronous
		}
	}
}
