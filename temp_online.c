
/*

TITTLE         : RE TO DFA CONVERSION
 */


#include<stdio.h>
#include<stdlib.h>

char Po[25], In[25];
int node[10][10], flps[10][10], position[10][10], DF[10][10];
int NN=1, DD=1, S=0, var=0, terp=1, v;

struct STACK
{
   char data;
   struct STACK *D;
   struct STACK *next,*L,*R;
   int FP[10],LP[10];
}  *O, *A;

void PUSH(char a)
{
   struct STACK *t;
   t=(struct STACK *)malloc(sizeof(struct STACK));
   t->data=a;
   if (O!=NULL) t->next=O;
   else         t->next=NULL;
   O=t;
}

char POP()
{
   char l;
   l=O->data;
   if(O->next!=NULL) O=O->next;
   else              O=NULL;
   return l;
}

void PUSHNODE(struct STACK *a)
{
   struct STACK *t;
   t=(struct STACK *)malloc(sizeof(struct STACK));
   t->D=a;
   if(O!=NULL) t->next=O;
   else        t->next=NULL;
   O=t;
}

struct STACK *POPNODE()
{
   struct STACK *t;
   t=(struct STACK *)malloc(sizeof(struct STACK));
   t=O->D;
   if(O->next!=NULL) O=O->next;
   else              O=NULL;
   return t;
}

void INFIX2POSTFIX(char I[25])
{
   int i,j,k;
   char p;
   for(i=0;I[i]!='\0';i++){
       if(I[i]=='(')
	  PUSH('(');
       else if(I[i]==')')
	  while(1){
	       p=POP();
	       if(p!='(') Po[v++]=p;
	       else   	  break;
	  }
       else if(I[i]=='+'){
	  while((O->data=='*'||O->data=='+'||O->data=='.')&&O!=NULL)
	       Po[v++]=POP();
	  PUSH(I[i]);
       }
       else if(I[i]=='.'){
	  while((O->data=='*'||O->data=='.')&&O!=NULL)
	       Po[v++]=POP();
	  PUSH('.');
       }
       else if(I[i]=='*'&&O!=NULL)
	       PUSH(I[i]);
       else    Po[v++]=I[i];
   }
   while(O!=NULL){
	p=POP();
	if(p!='(') Po[v++]=p;
   }
   Po[v++]='$';
   Po[v++]='.';
   Po[v]='\0';
}

void FORMTREE(char Po[])
{
   int i,j,k;
   struct STACK *N;
   O=NULL;
   for(i=0;Po[i]!='\0';i++){
       N=(struct STACK *)malloc(sizeof(struct STACK));
       N->data=Po[i]; N->L=NULL; N->R=NULL; N->next=NULL;
       N->FP[0]=0; N->LP[0]=0;
       if(Po[i]=='.'||Po[i]=='+'){
	  N->R=POPNODE();
	  N->L=POPNODE();
       }
       else if(Po[i]=='*'){
	  N->L=POPNODE();
	  N->R=NULL;
       }
       PUSHNODE(N);
   }
}
void FLPDISP (struct STACK *a)
{
   int i;
   if(a!=NULL){
      FLPDISP(a->L);
      FLPDISP(a->R);
      S++;
      printf("\n    %c",a->data);
      printf("\nFIRSTPOS -> ");
      for(i=1;i<=a->FP[0];i++)
	  printf(" %d",a->FP[i]);
      printf("\nLASTPOS  -> ");
      for(i=1;i<=a->LP[0];i++)
	  printf(" %d",a->LP[i]);
      printf("\n---------------");
      //getch();
   }
}


void FP_LP_CAL(struct STACK *a)
{
   if(a!=NULL){
      int i, flg;
      FP_LP_CAL(a->L);
      FP_LP_CAL(a->R);
      if(a->data!='+'&&a->data!='.'&&a->data!='*'){
	 a->FP[0]++; a->LP[0]++;
	 a->FP[a->FP[0]]=terp;
	 a->LP[a->LP[0]]=terp++;
	 flg=0;
	 for(i=0;i<var;i++)
	   if(position[i][0]==a->data){
	      position[i][1]++;
	      position[i][position[i][1]+1]=terp-1;
	      flg=1;
	   }
	   if(flg==0){
	      position[var++][1]=1;
	      position[var-1][2]=terp-1;
	      position[var-1][0]=a->data;
	   }
	 }
	 else if(a->data=='+'){
	   for(i=1;i<=a->L->FP[0];i++){
	       a->FP[0]++;
	       a->FP[a->FP[0]]=a->L->FP[i];
	   }
	   for(i=1;i<=a->R->FP[0];i++){
	     if(PRESENT(a->FP, a->R->FP[i],1)==0){
		a->FP[0]++;
		a->FP[a->FP[0]]=a->R->FP[i];
	     }
	   }
	   for(i=1;i<=a->L->LP[0];i++){
	       a->LP[0]++;
	       a->LP[a->LP[0]]=a->L->LP[i];
	   }
	   for(i=1;i<=a->R->LP[0];i++)
	      if(PRESENT(a->LP, a->R->LP[i], 1)==0){
		 a->LP[0]++;
		 a->LP[a->LP[0]]=a->R->LP[i];
	      }
	 }
	 else if(a->data=='*'){
	   for(i=1;i<=a->L->FP[0];i++){
	       a->FP[0]++;
	       a->FP[a->FP[0]]=a->L->FP[i];
	   }
	   for(i=1;i<=a->L->LP[0];i++){
	       a->LP[0]++;
	       a->LP[a->LP[0]]=a->L->LP[i];
	   }
	 }
	 else if(a->data=='.'){
	 //firstpos
	    for(i=1;i<=a->L->FP[0];i++){
		a->FP[0]++;
		a->FP[a->FP[0]]=a->L->FP[i];
	    }
	    if(a->L->data=='*')
	      for(i=1;i<=a->R->FP[0];i++)
		 if(PRESENT(a->FP, a->R->FP[i],1)==0){
		    a->FP[0]++;
		    a->FP[a->FP[0]]=a->R->FP[i];
		 }
	 //lastpos
	    for(i=1;i<=a->R->LP[0];i++){
		a->LP[0]++;
		a->LP[a->LP[0]]=a->R->LP[i];
	    }
	    if(a->R->data=='*')
	    for(i=1;i<=a->L->LP[0];i++)
		if(PRESENT(a->LP,a->L->LP[i],1)==0){
		  a->LP[0]++;
		  a->LP[a->LP[0]]=a->L->LP[i];
		}
	 }
   }
}
int PRESENT(int p[], int a, int b)
{
   int i,j;
   for(i=b,j=0;j<p[b-1];i++,j++)
       if(p[i]==a)
	  return 1;
   return 0;
}

void FLPCAL(struct STACK *a, int n)
{
   int j;
   if(a!=NULL){
      FLPCAL(a->L,n);
      if(a->data=='.'){
	 if(PRESENT(a->L->LP,n,1)==1)
	    for(j=1;j<=a->R->FP[0];j++)
	       if(PRESENT(flps[n],a->R->FP[j],1)==0){
		  flps[n][1]++;
		  flps[n][flps[n][1]+1]=a->R->FP[j];
	       }
      }
      else if(a->data=='*'){
	 if(PRESENT(a->LP,n,1)==1)
	    for(j=1;j<=a->FP[0];j++)
	      if(PRESENT(flps[n],a->FP[j],1)==0){
		 flps[n][1]++;
		 flps[n][flps[n][1]+1]=a->FP[j];
	      }
      }
      FLPCAL(a->R,n);
   }
}
/***************/
void FOLLOWPOS()
{
   int i,j,k;
   A=O;
   flps[0][0]=terp;
   for(i=1;i<=terp;i++){
       flps[i][0]=i;
       flps[i][1]=0;
       FLPCAL(O->D,i);
   }
   O=A;
}

void DFA()
{
   int i,j,k,l,m,fg,find;
   int tmp[10], newn[10];
   node[0][0]=1;
   node[1][0]='A';
   node[1][1]=0;
   for(i=1;i<=O->D->FP[0];i++){
       node[1][1]++;
       node[1][node[1][1]+1]=O->D->FP[i];
   }
   DF[0][0]=var-1;
   for(i=1;i<var;i++)
      DF[0][i]=position[i-1][0];
   for(i=1;i<=node[0][0];i++){
       DF[i][0]=node[i][0];
       for(j=1;j<=DF[0][0];j++){
	   tmp[0]=0;
	   newn[0]=0;
	   for(k=2;k<=node[i][1]+1;k++){
	       if(PRESENT(position[j-1],node[i][k],2)==1)
		  tmp[++tmp[0]]=node[i][k];
		  for(l=1;l<=tmp[0];l++){
		     for(m=2;m<=flps[tmp[l]][1]+1;m++)
			if(PRESENT(newn,flps[tmp[l]][m],1)==0){
			   newn[0]++;
			   newn[newn[0]]=flps[tmp[l]][m];
			}
		  }
	   }
	   printf(" ");
	   fg=0;
	   for(l=1;l<=node[0][0];l++){
	       find=0;
	       if(newn[0]==node[l][1])
		  for(k=1;k<=newn[0];k++)
		     if(PRESENT(node[l],newn[k],2)==0)
			  fg=1;
		     else find++;
		  if(find==newn[0]&&find!=0)
		     goto l1;
	   }
	   if(newn[0]==0){
	      DF[i][j]='-';
	      goto l2;
	   }
	   node[0][0]++;
	   node[node[0][0]][0]=node[node[0][0]-1][0]+1;
	   for(m=0;m<=newn[0];m++)
	      node[node[0][0]][m+1]=newn[m];
	   DF[i][j]=node[node[0][0]][0];
	   goto l2;
l1:       DF[i][j]=node[l][0];
l2:      if(fg==1||fg==0)  printf(" ");
       }
   }
   printf("\n\nNODES:\n\n");
   for(i=1;i<=node[0][0];i++,printf("\n\n"))
      for(j=0;j<=node[i][1]+1;j++)
	 if(j==0)
	    printf(" %c ",node[i][j]);
	 else if(j==1)
	    printf(" => ");
	 else
	    printf(" %d ",node[i][j]);
   printf("\n\nDFA TABLE:\n\n");
   for(i=0;i<=node[0][0];i++,printf("\n\n"))
      for(j=0;j<=DF[0][0];j++)
	  if(i==0&&j==0)
	     printf("    ");
	  else
	     printf("%4c",DF[i][j]);
}


int main()
{
   int i,j,k, gd, gm;
   int ch;
   O=NULL; A=NULL; v=0;
do{

   //clrscr();

   printf("\n----------------------------");
   printf("\n        RE -> DFA    ");
   printf("\n----------------------------");
   printf("\n 	1. ENTER R.E.");
   printf("\n 	2. POSTFIX EXPRESSION ");
   printf("\n 	3. VIEW FOLLOWPOS");
   printf("\n 	4. VIEW DFA ");
   printf("\n 	5. QUIT");
   printf("\n---------------------------");
   printf("\n\n    ENTER YOUR CHOICE : ");
  scanf("%d",&ch);
   switch(ch)
   {
      case 1:
	       printf("\n\n    ENTER REGULAR EXPRESSION : ");
	       scanf("%s",&In);
	       break;
      case 2:
	       INFIX2POSTFIX(In);
	       printf("\n\n    POSTFIX EXPRESSION : \n\n\t%s",Po);
	       O=NULL;
	       FORMTREE(Po);
	       break;
      case 3:
	       FP_LP_CAL(O->D);
	       FLPDISP(O->D);
	       FOLLOWPOS();
	       printf("\n\n    FOLLOWPOS :\n\n");
	       for(i=1;i<terp;i++)
	       {
		   for(j=0;j<=flps[i][1]+1;j++)
		      if(j!=1)
			 printf(" %d ",flps[i][j]);
		      printf("\n");
	       }
	       break;
      case 4:
	       printf("\n\n    POSITION MATRIX :\n\n");
	       for(i=0;i<var;i++)
	       {
		   for(j=0;j<=position[i][1]+1;j++)
		       if(j==0)
			  printf(" %c ",position[i][j]);
		       else if (j!=1)
			  printf(" %d ",position[i][j]);
		       printf("\n\n");
	       }
	       DFA();
	       break;
      case 5:
	printf("\n	Thanks.....\a");

	       break;
      default :
	       printf("\n\n    SORRY!! WRONG CHOICE!\a");
	       break;
     }
   }while(ch!=5);
   //getch();
   return 0;
}



/*
OUTPUT:

******************************
*                            *
*        RE -> DFA           *
*                            *
******************************
*                            *
*   1 - ENTER R.E.           *
*                            *
*   2 - POSTFIX EXPRESSION   *
*                            *
*   3 - VIEW FOLLOWPOS       *
*                            *
*   4 - VIEW DFA             *
*                            *
*   5 - QUIT                 *
*                            *
******************************

    ENTER YOUR CHOICE :1

 ENTER REGULAR EXPRESSION : (a+b)*.b.b

PRESS 'Y' TO CONTINUE.....





******************************
*                            *
*        RE -> DFA           *
*                            *
******************************
*                            *
*   1 - ENTER R.E.           *
*                            *
*   2 - POSTFIX EXPRESSION   *
*                            *
*   3 - VIEW FOLLOWPOS       *
*                            *
*   4 - VIEW DFA             *
*                            *
*   5 - QUIT                 *
*                            *
******************************

    ENTER YOUR CHOICE :2

 POSTFIX EXPRESSION :

     ab+*b.b.$.

 PRESS 'Y' TO CONTINUE.....


******************************
*                            *
*        RE -> DFA           *
*                            *
******************************
*                            *
*   1 - ENTER R.E.           *
*                            *
*   2 - POSTFIX EXPRESSION   *
*                            *
*   3 - VIEW FOLLOWPOS       *
*                            *
*   4 - VIEW DFA             *
*                            *
*   5 - QUIT                 *
*                            *
******************************

    ENTER YOUR CHOICE :3
    a
FIRSTPOS ->  1
LASTPOS  ->  1
---------------
    b
FIRSTPOS ->  2
LASTPOS  ->  2
---------------
    +
FIRSTPOS ->  1 2
LASTPOS  ->  1 2
---------------
    *
FIRSTPOS ->  1 2
LASTPOS  ->  1 2
---------------
    b
FIRSTPOS ->  3
LASTPOS  ->  3
---------------
    .
FIRSTPOS ->  1 2 3
LASTPOS  ->  3
---------------
    b
FIRSTPOS ->  4
LASTPOS  ->  4
---------------
    .
FIRSTPOS ->  1 2 3
LASTPOS  ->  4
---------------
    $
FIRSTPOS ->  5
LASTPOS  ->  5
---------------
    .
FIRSTPOS ->  1 2 3
LASTPOS  ->  5
---------------

    FOLLOWPOS :

 1  1  2  3
 2  1  2  3
 3  4
 4  5
 5


    PRESS 'Y' TO CONTINUE.....

******************************
*                            *
*        RE -> DFA           *
*                            *
******************************
*                            *
*   1 - ENTER R.E.           *
*                            *
*   2 - POSTFIX EXPRESSION   *
*                            *
*   3 - VIEW FOLLOWPOS       *
*                            *
*   4 - VIEW DFA             *
*                            *
*   5 - QUIT                 *
*                            *
******************************

    ENTER YOUR CHOICE :4

NODES:

 A  =>  1  2  3

 B  =>  1  2  3  4

 C  =>  1  2  3  4  5



DFA TABLE:

       a   b

   A   A   B

   B   A   C

   C   A   C



    PRESS 'Y' TO CONTINUE.....
******************************
*                            *
*        RE -> DFA           *
*                            *
******************************
*                            *
*   1 - ENTER R.E.           *
*                            *
*   2 - POSTFIX EXPRESSION   *
*                            *
*   3 - VIEW FOLLOWPOS       *
*                            *
*   4 - VIEW DFA             *
*                            *
*   5 - QUIT                 *
*                            *
******************************

    ENTER YOUR CHOICE :5
*/


