//Made by Kai Wu
//Homework 2 version 1

#include "IOMngr.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

FILE *fp1,*fp2;//stream of file. fp1 is for source file and fp2 is for listing file
int column_num, line_num;//the column number and line number of the character
char array[MAXLINE+1];//the array is for storing the information that buffered from each line
int status = 0; //whether the line has already make a array that has the information of the line,0 means not, 1 means yes
char c;//the character

//open the Files
bool OpenFiles(const char * ASourceName, const char * AListingName){
	if((fp1 = fopen(ASourceName, "r"))==NULL){
		printf("There is something wrong when reading the source file.\n");
		return false;
	}
		if ((fp2 = fopen(AListingName,"w"))==NULL) {
		fp2 = stdout;
	}
	column_num = 0;
	line_num = 0;
	status==0;//set the initial values
	return true;
}

//close the files
void CloseFiles(){
	if(fclose(fp1)==0){
		printf("The source file close successfully\n");
	}else {
		printf("Can't close the source file\n");
	}

	if (fp2 != stdout) {//if the listing file has been provided
	if(fclose(fp2)==0){
		printf("The listing file close successfully\n");
	}else {
		printf("Can't close the listing file\n");
	}
	}else {
		printf("No output file can be closed\n");
	}

}

//get the next character from the source file
char GetSourceChar(){
	int i=0;
	//if the status has value 0, then store the whole information in the line
	if(status == 0){
		while(true){
			c=fgetc(fp1);
		   if (c == '\n') {
			   array[i] = '\0';//(MARK 1)store the information as '\0' instead of '\n' to avoid after printing the end of lines, the consor will go to next line		
			   break;
		   }else if (c == EOF) {
				array[i] = EOF;
				break;
			}			
			array[i] = c;
			i++;			
		}
		
	status = 1;//after make the array, set status to value 1, so that when call this function and get the character in the same line will not make the array again.
	fprintf(fp2,"%4d:%s\n",++line_num,array);	//print out the whole line into the output file
	}
	c = array[GetCurrentColumn()];//get the current character
	if (c!=EOF) {
			if(c == '\0'){//when c is a newline character, search for "MARK 1", for more infor mation
		        column_num = 0;				
				memset(array,0,MAXLINE+1);//clean the array
				status = 0;//set the status as value 0, since we have not stored information for the next line yet
				return '\n';
		}
		if(c!='\0'){
			column_num++;//if c is a charactor not at the end of the line, then just increase the column number
			return c;
		}
		}else {
			column_num = 0;//if c is the end of file then set the column value to 0, so that we can get catch the line in test file
			return EOF;
		}	
}
			 
//write the indicator
void WriteIndicator(int AColumn){
	int i;
		
	for(i=0;i<AColumn+4;i++){
		fprintf(fp2," ");
	}
	fprintf(fp2,"^");
}

//write the message
void WriteMessage(const char * AMessage){
	fprintf(fp2,"\n%s\n",AMessage);
}

//return the current line 			 
int GetCurrentLine(){
	return line_num;
}

//return the current column
int GetCurrentColumn(){
	return column_num;
}

