#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
FILE *fp;
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

	int n=1,x=0;
	char st[100];
	if(argc!= 3) {
		printf("Invalid number of arguments\n");
		return;
	}
	if((fp=fopen(argv[2],"r"))==NULL) {
        printf("No file found!");
        return;
    }
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


void count(int argc, char **argv) {

	int c=0,w=0,l=0;
	char ch='\0';
	if(argc!= 3) {
		printf("Invalid number of arguments\n");
		return;
	}
	if((fp=fopen(argv[2],"r"))==NULL) {
        printf("No file found!");
        return;
    }
    while(!feof(fp))
   {
    ch=fgetc(fp);
     if(ch=='\n'){
        c++;
        w++;
        l++;
      }
     else if(isspace(ch)){
       c++;
       w++;
     }
    else
       c++;
   }
	if(strcmp("c",argv[1])==0) {
        printf("The number of characters in the file is= %d ",c);
    }
    else if(strcmp("w",argv[1])==0){
		printf("The number of words in the file is= %d ", w);
	}
	else if(strcmp("l",argv[1])==0) {
			printf("The number of lines in the file is= %d ",l);
	}
}

void search(int argc,char **argv) {
    int c=0;
    char st[100];

    fp=fopen(argv[2],"r");
    if(argc!=4){
        printf("Invalid number of arguments");
        return;
    }
    if(fp==NULL) {
        printf("File not found");
        return;
    }
    int l=0;
    if(strcmp(argv[1],"f")==0) {
        while(fgets(st,100,fp)!=NULL) {
                l++;
            if(strstr(st,argv[3])!=NULL){
                printf("\nFirst occurence:%d .%s\n",l,st);
                    return;
            }
        }
    }

    if(strcmp(argv[1],"a")==0) {
        while(fgets(st,100,fp)!=NULL) {
                l++;
            if(strstr(st,argv[3])!=NULL){
                printf("\n%d .%s",l,st);

            }
        }
    }
    if(strcmp(argv[1],"c")==0) {
        while(fgets(st,100,fp)!=NULL) {
                l++;
            if(strstr(st,argv[3])!=NULL){
                c++;

            }
        }
        printf("\nThe pattern %s is found %d times",argv[3],c);
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

int main(int argc, char **argv) {

	char str[100];
	int i, id, ret = 0;

	printf("prog: %s\n",argv[0]);

	while(ret == 0) {
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
			} else if(strcmp("typeline",argv[0])==0) {
				typeline(argc, argv);
			}
			else if(strcmp("count",argv[0])==0) {
				count(argc, argv);
			}
			else if(strcmp("search",argv[0])==0){
				search(argc, argv);
			}
			else if(strcmp("exit",argv[0])==0){
				printf("Exit\n");
				return -12;
			}
			else if(execvp(argv[0], argv)==-1) {
				printf("command not found. :(\n");
			}
			
		} else if(id < 0) { //process not created
			printf("Error: couldn't create child process.\n");
		} else { //parent
			//printf("parent %d\n",id);
			ret = wait(NULL); //synchronous
		}
	}

	return 0;
}
