/*
 * src/tutorial/intset.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "postgres.h"
#include <string.h>
#include "utils/builtins.h"
#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */
#include "access/hash.h"
PG_MODULE_MAGIC;

typedef struct intSet
{
	//stable
	
	int length;
	int intset[1];
}intSet;


/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(intset_in);//inform this to pgsql

Datum
intset_in(PG_FUNCTION_ARGS)
{
	char *token;
	
	char *str = PG_GETARG_CSTRING(0);
	intSet *result;
	char *delim="{, }";
	//loop
	int *array,length=2,element_count=0,found=0,element=-1,f=0,judge_intput=1;
	if (strcmp(str, "") == 0){ereport(ERROR,(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),errmsg("invalid syntax intset")));}

	array=malloc(sizeof(int)*length);
	token = strtok(str, delim);
   while( token != NULL ) {
		char str_t[30];
		stpcpy(str_t,token);
		for(int yy=0;yy<strlen(str_t);yy++)
		{
			if(str_t[yy]-0<48 || str_t[yy]-0>57)
			{
				free(array);
		   ereport(ERROR,(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),errmsg("invalid syntax intset")));
				
			}
		}
	//copy
	  sscanf(token, "%d", &element);
	   if (element_count==length)
	   {
		   length=length*2;
		   array=realloc(array,sizeof(int)*length);
	   }
	   found=0;
	   for(int i=0;i<element_count;i++)
	   {
		   if(array[i]==element)
		   {
			   found=1;
			   break;
		   }
	   }
	   if(found==0)
	   {
	array[element_count]=element;
	  element_count++;
	   }
	   token = strtok(NULL,delim);
   }
   result=(intSet *)palloc(VARHDRSZ+(sizeof(int)*(element_count+3)));
   
   //SET
   SET_VARSIZE(result ,VARHDRSZ+(sizeof(int)*(element_count+3)));
   //sort such as heap or qsort
   if(judge_intput>0){
       for(int i=0;i<element_count-1;i++){
        for (int j=i+1;j<element_count;j++){
            if(array[i]>array[j])  
            {
              int t=array[i];
                array[i]=array[j];
                array[j]=t;
            }
		}
	   }
	   
	result->intset[0]=element_count;
	while(f<element_count)
	{
		result->intset[f+1]=array[f];
		f++;
	}
   }
   else{result->intset[0]=-100;}
   //free memory
   free(array);
   PG_RETURN_POINTER(result);
}

//output
PG_FUNCTION_INFO_V1(intset_out);//delcare function

Datum
intset_out(PG_FUNCTION_ARGS)
{

	intSet *paramSet = (intSet *) PG_GETARG_POINTER(0);
	//in order to test,using fixed length.
	char strNum[550];//something can be changed.
	char result[102400];
   //SET
   SET_VARSIZE(result ,VARHDRSZ+(sizeof(char)*(102400)));
	result[0]='\0';
	strcat(result,"{"); 
	if (paramSet->intset[0]>0)
	{
		sprintf(strNum, "%d", paramSet->intset[1]);
		strcat(result,strNum);
		for(int i=1;i<paramSet->intset[0];i++)
		{
			strcat(result,",");	
			sprintf(strNum, "%d", paramSet->intset[i+1]);
			strcat(result,strNum);			
		}
	}
	strcat(result,"}");
	PG_RETURN_CSTRING(psprintf("%s",result));
}


//= equal
PG_FUNCTION_INFO_V1(intset_eq);
//intSets A and Bare equal;
Datum
intset_eq(PG_FUNCTION_ARGS)
{
	intSet *a = (intSet *) PG_GETARG_POINTER(0);
	intSet *b = (intSet *) PG_GETARG_POINTER(1);
	int result=1;
	if (a->intset[0]!=b->intset[0]){
		result=0;
	}
	else
	{
		for(int i=1;i<=a->intset[0];i++)
		{
			if (a->intset[i]!=b->intset[i])
			{
				result=0;
				break;
			}
		}
	}
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(intset_cardinality);
//cardinality  

//Give the cardinality, or number of distinct elements in, intSet S
Datum
intset_cardinality(PG_FUNCTION_ARGS)
{
	intSet *a = (intSet *) PG_GETARG_POINTER(0);
	
	PG_RETURN_INT32(a->intset[0]);
}


PG_FUNCTION_INFO_V1(intset_difference);
//difference  

//Takes the set difference, and produces an intSet 
//containing elements that are in Aand not in B.
Datum
intset_difference(PG_FUNCTION_ARGS)
{
	int *array,length=3,element_count=0,f=0;
	intSet *a = (intSet *) PG_GETARG_POINTER(0);
	intSet *b = (intSet *) PG_GETARG_POINTER(1);
	intSet *result;
	array=malloc(sizeof(int)*length);
    for(int i=1;i<=a->intset[0];i++)
	{
		int found=0;
		for(int j=1;j<=b->intset[0];j++)
		{
			if(a->intset[i]==b->intset[j])
			{
				found=1;
				break;
			}
		}
		if(found==0)
		{
			array[element_count]=a->intset[i];
			element_count=element_count+1;
			if(element_count==length)
			{
				length=length*2;
		        array=realloc(array,sizeof(int)*length);
			}
		}
	}		
   //allocate memory
   result=(intSet *)palloc(VARHDRSZ+(sizeof(int)*(element_count+2)));
   //SET
   result->intset[0]=element_count;
   SET_VARSIZE(result ,VARHDRSZ+(sizeof(int)*(element_count+2)));
   while(f<element_count)
   {
	   result->intset[f+1]=array[f];
	   f++;
   }
   free(array);
   PG_RETURN_POINTER(result);
}


PG_FUNCTION_INFO_V1(intset_qu);
//qu 
//intSet Scontains the integer i; 
Datum
intset_qu(PG_FUNCTION_ARGS)
{
	int a = PG_GETARG_INT32(0);
	intSet *b = (intSet *) PG_GETARG_POINTER(1);
	int result=0;
	for(int i=1;i<=b->intset[0];i++)
	{
		if(a==b->intset[i]){
			result=1;
			break;
		}
	}
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(intset_intersection);
//Takes the set intersection, and produces an intSet 
//containing the elements common to A and B;That is, A∩B

//intersection 
Datum
intset_intersection(PG_FUNCTION_ARGS)
{
	intSet *a = (intSet *) PG_GETARG_POINTER(0);
	intSet *b = (intSet *) PG_GETARG_POINTER(1);
	intSet *result;
	int *array,length=3,element_count=0,f=0;
	array=malloc(sizeof(int)*length);
	if(a->intset[0]<=b->intset[0]){
	    for(int i=1;i<=a->intset[0];i++)
	{
		int found=0;
		for(int j=1;j<=b->intset[0];j++)
		{
			if(a->intset[i]==b->intset[j])
			{
				found=1;
				break;
			}
		}
		if(found==1)
		{
			array[element_count]=a->intset[i];
			element_count=element_count+1;
			if(element_count==length)
			{
				length=length*2;
		        array=realloc(array,sizeof(int)*length);
			}
		}
	}	
	}	
     if(a->intset[0]>b->intset[0]){
	    for(int i=1;i<=b->intset[0];i++)
	{
		int found=0;
		for(int j=1;j<=a->intset[0];j++)
		{
			if(a->intset[j]==b->intset[i])
			{
				found=1;
				break;
			}
		}
		if(found==1)
		{
			array[element_count]=b->intset[i];
			element_count=element_count+1;
			if(element_count==length)
			{
				length=length*2;
		        array=realloc(array,sizeof(int)*length);
			}
		}
	}	
	}		
   //allocate memory
   result=(intSet *)palloc(VARHDRSZ+(sizeof(int)*(element_count+2)));
   //SET
   SET_VARSIZE(result ,VARHDRSZ+(sizeof(int)*(element_count+2)));
   result->intset[0]=element_count;
   while(f<element_count)
   {
	   result->intset[f+1]=array[f];
	   f++;
   }
   free(array);
	PG_RETURN_POINTER(result);
}

//union
//AUB
PG_FUNCTION_INFO_V1(intset_union);
Datum
intset_union(PG_FUNCTION_ARGS)
{
	int *array,length=3,element_count=0,found=0,f=0;
	intSet *a = (intSet *) PG_GETARG_POINTER(0);
	intSet *b = (intSet *) PG_GETARG_POINTER(1);
	intSet *result;
	int border=0;
	//insert b
	array=malloc(sizeof(int)*length);	
	    for(int i=1;i<=b->intset[0];i++)
	{
			array[element_count]=b->intset[i];
			element_count=element_count+1;
			if(element_count==length)
			{
				length=length*2;
		        array=realloc(array,sizeof(int)*length);
			}
	}
	border=b->intset[0];

    //insert a
	array=realloc(array,sizeof(int)*(length+2));
	for(int i=1;i<=a->intset[0];i++)
	{
		found=0;
		for(int j=0;j<border;j++)
		{
			if(a->intset[i]==array[j]){
				found=1;
				break;
			}
		}
		if(found==0)
		{
			array[element_count]=a->intset[i];
			element_count=element_count+1;
			if(element_count==length)
			{
				length=length*2;
		        array=realloc(array,sizeof(int)*length);
			}
		}
	}
	   //allocate memory
   result=(intSet *)palloc(VARHDRSZ+(sizeof(int)*(element_count+2)));
   //SET
   SET_VARSIZE(result ,VARHDRSZ+(sizeof(int)*(element_count+2)));
   result->intset[0]=element_count;
	 for(int i=0;i<element_count-1;i++){
        for (int j=i+1;j<element_count;j++){
            if(array[i]>array[j])
            {
              int t=array[i];
                array[i]=array[j];
                array[j]=t;
            }
			}
	 }
	while(f<element_count)
	{
		result->intset[f+1]=array[f];
		f++;
	}
   free(array);
	PG_RETURN_POINTER(result);
}

//contain b
PG_FUNCTION_INFO_V1(intset_contain_b);
Datum
intset_contain_b(PG_FUNCTION_ARGS)
{
	intSet *a = (intSet *) PG_GETARG_POINTER(0);
	intSet *b = (intSet *) PG_GETARG_POINTER(1);
	int result=0,element=0;
	if(a->intset[0]<b->intset[0])
	{
		result=0;
	}
	if(a->intset[0]>=b->intset[0])
	{
		
		for(int i=1;i<=b->intset[0];i++)
		{
			for(int j=1;j<=a->intset[0];j++)
			{
				if(a->intset[j]==b->intset[i])
				{
					element++;
					result=1;
					break;
				}
			}
		}
	}
	if (element==b->intset[0])result=1;
	else result=0;
	PG_RETURN_BOOL(result);
}

//contain a
//>@
PG_FUNCTION_INFO_V1(intset_contain_a);
Datum
intset_contain_a(PG_FUNCTION_ARGS)
{
	intSet *a = (intSet *) PG_GETARG_POINTER(0);
	intSet *b = (intSet *) PG_GETARG_POINTER(1);
	int result=0,element=0;
	if(b->intset[0]<a->intset[0])
	{
		result=0;
	}
	if(b->intset[0]>=a->intset[0])
	{
		
		for(int i=1;i<=a->intset[0];i++)
		{
			for(int j=1;j<=b->intset[0];j++)
			{
				if(b->intset[j]==a->intset[i])
				{
					element++;
					result=1;
					break;
				}
			}
		}
	}
	if (element==a->intset[0])result=1;
	else result=0;
	PG_RETURN_BOOL(result);
}

//<> not equal
PG_FUNCTION_INFO_V1(intset_contain_not_equal);

Datum
intset_contain_not_equal(PG_FUNCTION_ARGS)
{
	intSet *a = (intSet *) PG_GETARG_POINTER(0);
	intSet *b = (intSet *) PG_GETARG_POINTER(1);
	int result=0;
	int num=0;
	if(a->intset[0]==0 && b->intset[0]==0){
		result=0;
	}
	if(a->intset[0]!=b->intset[0]){
		result=1;
	}
	if(a->intset[0]==b->intset[0]){
		for(int i=1;i<=a->intset[0];i++)
		{
			if(a->intset[i]==b->intset[i])
			{
				num=num+1;
			}
			else {break;}
		}
		if(num==b->intset[0]){
			result=0;
		}
		if(num!=b->intset[0]){
			result=1;
		}
	}
	PG_RETURN_BOOL(result);
}


PG_FUNCTION_INFO_V1(intset_dis);
//dis
//Takes the set disjunction, and produces an intSet 
//containing elements that are in Aand not in Bor that are in Band not in 

//
Datum
intset_dis(PG_FUNCTION_ARGS)
{
    int *array,length=3,element_count=0,f=0;
	intSet *a = (intSet *) PG_GETARG_POINTER(0);
	intSet *b = (intSet *) PG_GETARG_POINTER(1);
	intSet *result;
	array=malloc(sizeof(int)*length);
	//This for a.
    for(int i=1;i<=a->intset[0];i++)
	{
		int found=0;
		for(int j=1;j<=b->intset[0];j++)
		{
			if(a->intset[i]==b->intset[j])
			{
				found=1;
				break;
			}
		}
		if(found==0)
		{
			array[element_count]=a->intset[i];
			element_count=element_count+1;
			if(element_count==length)
			{
				length=length*2;
		        array=realloc(array,sizeof(int)*length);
			}
		}
	}

//This for b.
    for(int i=1;i<=b->intset[0];i++)
	{
		int found=0;
		for(int j=1;j<=a->intset[0];j++)
		{
			if(a->intset[j]==b->intset[i])
			{
				found=1;
				break;
			}
		}
		if(found==0)
		{
			array[element_count]=b->intset[i];
			element_count=element_count+1;
			if(element_count==length)
			{
				length=length*2;
		        array=realloc(array,sizeof(int)*length);
			}
		}
	}
	
   //allocate memory
   result=(intSet *)palloc(VARHDRSZ+(sizeof(int)*(element_count+2)));
   //SET
   SET_VARSIZE(result ,VARHDRSZ+(sizeof(int)*(element_count+2)));
   result->intset[0]=element_count;
       for(int i=0;i<element_count-1;i++){
        for (int j=i+1;j<element_count;j++){
            if(array[i]>array[j])  
            {
              int t=array[i];
                array[i]=array[j];
                array[j]=t;
            }
		}
	   }
        while(f<element_count)	
		{
			result->intset[f+1]=array[f];
			f++;
		}
   free(array);
   PG_RETURN_POINTER(result);
}