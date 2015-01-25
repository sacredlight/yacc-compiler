/*SymTab.c
By Kai Wu 2012 Jan 30th Version Collected after interview
*/

#include "SymTab.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//free the entry
void FreeSymEntry(struct SymEntry * E){
	struct SymEntry *temp = E;
	struct SymEntry *temp1;
	
	if(E != NULL){
	while (temp -> Next != NULL) {
		temp1 = temp -> Next;
		free(temp->Name);
		free(temp);
		temp = temp1;
	}
		free(temp->Name);
		free(temp);
	}
} 

//create a hash table with size Size
struct SymTab * CreateSymTab(int Size) {
	 struct SymTab * table = (struct SymTab *)malloc(sizeof(struct SymTab));
	table->Size = Size;
	struct SymEntry ** toTable = (struct SymEntry **)malloc(Size * sizeof(struct SymEntry *));
	table -> Contents = toTable;
	int i;
	for(i=0; i<Size; i++){
			table->Contents[i] = NULL;
	}
	return table;
}


void DestroySymTab(struct SymTab *ATable) {
	int j;
	
	for (j = ATable -> Size; j > 0; j--) {
			FreeSymEntry(ATable->Contents[j-1]);
			ATable -> Contents[j-1] = NULL;
	}
	free(ATable -> Contents);
	free(ATable);
}

			 
//enter the name into the hash table
bool EnterName(struct SymTab *ATable, const char *Name, struct SymEntry * *AnEntry){
	int sum = 0;
	int length = strlen(Name);
	int j;
	int index;
	struct SymEntry *temp = (struct SymEntry *)malloc(sizeof(struct SymEntry));
	struct SymEntry *temp1;
	
	//the rules for putting the strings
	for(j=0; j < length - 1; j++){
		sum += Name[j];
	}
	
	index = length%(ATable->Size);	
	//if the entry of contents is not null, then add to the end of this list.(horizonnal)
	if (ATable -> Contents[index] != NULL) {
		temp1 = ATable -> Contents[index];
		while (temp1 -> Next != NULL) {
			if(strcmp(temp1 -> Name,Name) == 0){
				(*AnEntry) = temp1;
				free(temp);
				return true;
			}else{
				temp1 = temp1 -> Next;
			}
		}
		
		if(strcmp(temp1 -> Name,Name) == 0){
			(*AnEntry) = temp1;
			free(temp);
			return true;
		}
		
		temp -> Next = NULL;
		temp -> Attributes = NULL;
		temp -> Name = (char *)malloc(strlen(Name)+1);
		strcpy(temp->Name, Name);
		temp1-> Next= temp;
		(*AnEntry) = temp;
	}else {//if the contents is null
		temp -> Name = (char *)malloc(strlen(Name)+1);
		strcpy(temp->Name,Name);	
		temp -> Next = NULL;
		temp-> Attributes = NULL;
		ATable -> Contents[index] = temp;
		(*AnEntry) = temp;
	}
	return true;
}
//find an entry by a name			 
struct SymEntry * FindName(struct SymTab *ATable, const char * Name){
	int length = strlen(Name);
	int index;
	int i;
	struct SymEntry * temp1;
	
	index = length%(ATable -> Size);
	if (ATable -> Contents[index] != NULL) {
		temp1 = ATable -> Contents[index];
		while (temp1 -> Next != NULL) {
			if(strcmp(temp1 -> Name,Name) == 0){
				return temp1;
			}else{
				temp1 = temp1 -> Next;
			}
		}
		if(strcmp(temp1 -> Name,Name) == 0){
			return temp1;
		}
		return NULL;
	}
	return NULL;
}

//return the firstentry in the hash table
struct SymEntry *  FirstEntry(struct SymTab *ATable) {
	struct SymEntry *temp,*temp1;
	int i = 1;
	
	if (ATable == NULL) {
		return NULL;
	}else {
		if (ATable -> Contents[0] == NULL) {
			while (ATable->Contents[i] == NULL) {
				i++;
				if(i+1 == ATable ->Size){
					return NULL;
				}
			}
			return ATable -> Contents[i];
		}else{
			return ATable -> Contents[0];
}
}
}

//set the attribute for the entry			 
void SetAttr(struct SymEntry *AnEntry, void *Attributes){
	(AnEntry -> Attributes) = Attributes;
}

//get the attribute for the entry
void * GetAttr(struct SymEntry *AnEntry){
	if(AnEntry == NULL){
          return NULL;
	}
	void *tempAttr = AnEntry -> Attributes;
	return tempAttr;
}
			 
//get the name for the entry
const char * GetName(struct SymEntry *AnEntry){
	if(AnEntry == NULL){
          return NULL;
	}
	char *tempName = AnEntry -> Name;
	return tempName;
}

//retrun the nextentry of the entry which is inputed.
struct SymEntry *  NextEntry(struct SymTab *ATable, struct SymEntry *AnEntry) {
	char *nextname = AnEntry->Name; 	
	int length = strlen(nextname);
	struct SymEntry *currentEntry;
	int index;
	int pointer;
		
			
	index = length%(ATable -> Size);
	pointer = index;
	if(ATable == NULL){
		return NULL;
	}else {//first go in the horizonal direction. when the horizonal direction's entry runs out, then go vertical direction
		if(AnEntry -> Next == NULL){
			
			if(pointer + 1 == ATable -> Size){	
							
					return NULL;
				}
			while (ATable-> Contents[pointer+1] == NULL) {				
				pointer ++;
				if(pointer + 1 == ATable ->Size){
					return NULL;
				}
			}
			return ATable -> Contents[pointer+1];
		}else {
			return AnEntry -> Next;
		}

	}
	return NULL;
}

