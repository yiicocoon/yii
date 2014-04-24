#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <CTYPE.H>
#include <WINDOWS.H>
#define APile 0
#define BPile 1
#define CPile 2
#define Tiao  0
#define Bing  1
#define Feng  2

/******总牌堆索引名称*******/
char *MainPileTable[84]={"T1","T1","T1","T1","T2","T2","T2","T2","T3",
                  "T3","T3","T3","T4","T4","T4","T4","T5","T5",
				  "T5","T5","T6","T6","T6","T6","T7","T7","T7",
				  "T7","T8","T8","T8","T8","T9","T9","T9","T9",
				  "B1","B1","B1","B1","B2","B2","B2","B2","B3",
				  "B3","B3","B3","B4","B4","B4","B4","B5","B5",
				  "B5","B5","B6","B6","B6","B6","B7","B7","B7",
				  "B7","B8","B8","B8","B8","B9","B9","B9","B9",
                  "X1","X1","X1","X1","X2","X2","X2","X2","X3","X3","X3","X3"};


/*******总牌堆*************/
int MainPile[84];
int *Pm=MainPile,*PmEnd=&MainPile[83];                                                   
int MainLeftNum=45;

/*******出牌堆************/
int DirtyPile[84];
int *Pd=DirtyPile;

/****Pile[APile]为A手牌，PLow[APile]手牌首指针，PHigh[APile]手牌尾指针**********/
int Pile[3][14]={0,4,8,9,10,11,12,24,28,32,39,44,49,54};
int *PLow[3]={&Pile[0][0],&Pile[1][0],&Pile[2][0]};
int *PHigh[3]={&Pile[0][12],&Pile[1][12],&Pile[2][12]};

/***PengPile[APile]为A碰的牌，PengHigh为尾指针****************/
int PengPile[3][16];
int *PengHigh[3]={&PengPile[0][0],&PengPile[1][0],&PengPile[2][0]};


int PossibilityToGet[3][21];

/****FirstTinghuChoice[XPile]里放着所胡的牌的选择，第一列为该行所胡牌的个数，0为没有停牌，SomeWin标志是否有人赢了****/
/**例FirstTinghuChoice[APile]={3, 9, 20, 32, -1, -1, -1...      胡3个牌，胡3、6、9条
						 	 2, 9, 12, -1, -1, -1, -1...		 胡2个牌，胡3、4条
						   	-1,-1, -1, -1, -1, -1, -1...	 没有胡
							-1,-1, -1, -1...
							-1,-1...
							...
							}****************************************************************************/
int FirstTinghuChoice[3][14][15]; 
int TileToWin[3][15];

/*****初始0，自摸加2，被点炮了加1，点炮的每点一个减1*******/
int SomeWin[3]={0,0,0};

int SecondTinghuChoice[3][14][23];


/********统计量，条数、饼数、风数**********************/
int nTiao[3]={0,0,0},nBing[3]={0,0,0},nFeng[3]={0,0,0},nAll[3]={0,0,0};
int nX1[3]={0,0,0},nX2[3]={0,0,0},nX3[3]={0,0,0};
int *pTiao[3],*pBing[3],*pFeng[3];


void DisplayMain(void);
void Display(int XPile);
void DisplayAll(void);
void Shuffle(int Times);
void Deal(void);

void swap(int *x,int *y);
int iInXPile(int XPile,int i);
int SameKind(int *p,int *p2);
int SameTile(int *p1,int *p2);
int SequenceTile(int *p1,int *p2);
int XHasCompany(int XPile,int *p);
void ClearFirstTinghuChoice(int XPile);

void QuickSort(int *p,int low,int high);
void Sort(int XPile);
void SortAll(void);

int AbleToPeng(int XPile);
int GoodToPeng(int XPile);
int Peng(int XPile);
int OthersPeng(int XPile);

void TakeFront(int XPile);

void GetPossibility(int XPile);
int OneToThrow(int XPile);
void UnThrow(int XPile);
void ThrowAction(int XPile,int specific);
void Throw(int XPile);

int QualifyThrowAnswer(char *answer,int XPile);
int TakeSequence(int *p);
int TakeThree(int *p);
int Combination(int a[],int n);
int AllCombination(int XPile);
int Win(int XPile);
void Warn(int XPile);
int SingleFirst(int XPile);
int FirstTinghu(int XPile);
int OthersWin(int XPile);

/********小函数***********/
void swap(int *x,int *y)
{
	int temp;
	temp=*x;
	*x=*y;
	*y=temp;
}


/******手里是否有i这张牌*********/
int iInXPile(int XPile,int i)
{
	int *p=PLow[XPile];
	while(p<=PHigh[XPile])
	{
		if(*p==i) return 1;
		else p++;
	}
	return 0;
}

/*****手里有几张和i相同的牌，返回个数*****/
int XInXPile(int XPile,int i)
{
	int *p=PLow[XPile],n=0;
	while(p<=PHigh[XPile])
	{
		if(SameTile(p,&i))
			n++;
		p++;
	}
	return n;
}

int SameKind(int *p1,int *p2) 
{
	if(*p1<0 || *p1>83) return 0;
	if(*p2<0 || *p2>83) return 0;
	if(((*p1)/36)==((*p2)/36)) return 1;
	else return 0;
}

int SameTile(int *p1,int *p2)
{
	if(SameKind(p1,p2))
		if( ((*p1)%36)/4 == ((*p2)%36/4) )
			return 1;
		else return 0;
	else return 0;
}
int SequenceTile(int *p1,int *p2)
{
	if(SameKind(p1,p2))
		if( (((*p1)%36)/4+1) == ((*p2)%36/4) )
			return 1;
		else return 0;
	else return 0;
}


void Warn(int XPile)
{
	int i=0;
	printf("%c is waiting for ",('A'+XPile));
	

	for(i=0;i<14;i++)
	{
		if(TileToWin[XPile][i]==-1) break;
		if(XPile==APile)
			printf("%s ",MainPileTable[TileToWin[XPile][i]]);
		else
			printf("* ");
	}
	printf(" to win!!!\n");
}

void ClearFirstTinghuChoice(int XPile)
{
	int *p,i=0;
	for(i=0;FirstTinghuChoice[XPile][i][0]!=-1;i++)
	{
		p=&FirstTinghuChoice[XPile][i][0];
		while(*p!=-1)
			*p++=-1;
	}

}
void ClearSecondTinghuChoice(int XPile)
{
	int *p,i=0;
	for(i=0;SecondTinghuChoice[XPile][i][0]!=-1;i++)
	{
		p=&SecondTinghuChoice[XPile][i][0];
		while(*p!=-1)
			*p++=-1;
	}
	
}


void ShowWelcome()
{
	printf("  〓〓§〓〓〓〓〓§〓〓〓〓〓§〓〓〓〓〓§〓〓〓〓〓§〓〓\n");
	printf("      ↓　　　　　↓　　　　  ↓　　　　　↓　　　　　↓\n"); 
	printf("    ☆★☆　　　☆★☆　　  ☆★☆　　　☆★☆　　　☆★☆\n"); 
	printf("   ☆　  ☆　  ☆　  ☆　  ☆　　☆　  ☆　  ☆　  ☆　  ☆\n"); 
	printf("  ☆　欢  ☆　☆  乐  ☆  ☆　卡　☆　☆　五　☆　☆　星　☆\n"); 
	printf("   ☆　  ☆　  ☆　  ☆　  ☆　  ☆　  ☆　　☆　  ☆　　☆\n"); 
	printf("    ☆★☆　　　☆★☆　　  ☆★☆　　　☆★☆　　　☆★☆\n"); 
	printf("      ↓　　　　　↓　　　　  ↓　　　　　↓　　　　　↓       KickMeFive V2.0\n"); 
	printf("      ※　　　　　※　　　　  ※　　　　　※　　　　　※                By Yii\n");

	printf("How to Play?\n");
	printf("            T :表示条。例如T2代表二条\n");
	printf("            B :表示饼。例如B4代表四饼\n");
	printf("            X1:代表白板\n");
	printf("            X2:代表红中\n");
	printf("            X3:代表发财\n\n");
	printf("            根据提示出牌，例如要出T1，就输入：T1或者t1\n");

}
char GameOver(void)
{
	int i,d=0;
	char answer;

    /******双响*****************/
	for(i=0;i<3;i++)
	{
		if(SomeWin[i]==-2)
		{
			if(i)
				printf("倒霉的 %c 点了个双响，不幸啊~~~~~~~~~~~~\n",'A'+i);
			else
				printf("你活生生点了两炮，亮了!!!!!!!!\n");
			d=1;
			break;
		}
	}

	/*****单响*************/
	if(d==0)
	{	
		/*****没有A参与*************/
		if(SomeWin[0]==0)
		{
			for(i=1;i<3;i++)
				if(SomeWin[i]==2)
				{
					printf("C win 远方传来 %c 自摸的消息， 不服不行...\n",'A'+i);
					break;
				}
			if(SomeWin[1]==1)
				printf("B win 儿骗，B 狠狠响了一炮 from C，骚年你不来一发吗？\n");
			else
			{
				if(SomeWin[2]==1)
					printf("C win 可恶啊 C炮响了! B点的。局外人你好，局外人再见\n");
				else
					printf("No one win 没有人是赢家，啊，多么痛的领悟...\n");
			}
		}
		else
		{
			if(SomeWin[0]==2)
				printf("You win 小伙子不错嘛，自摸出感觉了...\n");
			else
			{
				if(SomeWin[0]==1)
				{
					if(SomeWin[1]==-1)
					printf("You win 你被 B 点了一下炮，还不还手？\n");
					else
						printf("You win 你被 C 点了，请向后接着点下去...\n");
				}
				else
				{
					if(SomeWin[1]==1)
						printf("B win 啊哈哈哈哈，你点炮了2货!\n");
					else
						printf("C win 听说你给人家C点了个炮\n");
				}
			}
		}
	}
	Display(APile);
	Display(BPile);
	Display(CPile);
	printf("Would you like to play another round?(y/n):");
	scanf("%s",&answer);
	if(answer=='y' || answer=='Y') 
	{
		system("cls");  	
		return 'y';
	}
	else 
	{
		printf("\n");
		
		printf("		╭︿︿︿╮\n");
	    printf("		{/ ·· /}\n");
		printf("	  	 ( (00) )\n");
		printf("		  ︶︶︶\n");
		printf("	Hey! You , Close me! ");
		return 'n';
	}
}

/***********返回指定出的牌的偏移量，输入不符合则返回-1***********/
int QualifyThrowAnswer(char *answer,int XPile)
{
	int n=0,i=0,danswer=-1,one=-1;
	int *p2;
	char *p=answer;
	while(*p!='\0')
	{
		n++;
		*p=toupper(*p);
		p++;
	}
	p=answer+1;
	/*******粗判定****************/
	if(((*answer!='T' && *answer!='B') && *answer!='X') || n!=2)
	{
		printf("Illegal input,you stupid\n");
		return -1;
	}
	if(!isdigit(*p) || *p=='0')
	{
		printf("Illegal input,you stupid\n");
		return -1;
	}
	if(*answer=='X' && (((*p!='1') && (*p!='2')) && (*p!='3')))
	{
		printf("Illegal input,you stupid\n");
		return -1;
	}
    /***********计算偏移*****************/
	while(i<84)
	{
		if(strcmp(MainPileTable[i],answer)==0) 
		{
			danswer=i;
			break;
		}
		else
			i+=4;
	}
	if(i>=84) return -1;
	p2=PLow[XPile];
	while(p2<=PHigh[XPile])
	{
		if(SameTile(p2,&danswer))
		{
			one=(p2-PLow[XPile]);
		    return one;
		}
		else 
			p2++;
	}
	printf("2huo! You don't have a %s\n",answer);
	return one;
}
/*	
		else return 1;
		*/
void InitAllData(void)
{
	int i=0,*p;
		/*******初始化主牌堆和出牌堆**********/
	for(i=0;i<84;i++)
	{
		MainPile[i]=i;
		DirtyPile[i]=-1;
	}
		/*******初始化碰牌堆*************/
	p=&PengPile[0][0];
	while(p<=&PengPile[2][15])
		 *p++=-1;

		/*******初始化1、2阶停胡选择表*************/
     p=&FirstTinghuChoice[0][0][0];
	 while(p<=&FirstTinghuChoice[2][13][14])
		 *p++=-1;
	 p=&SecondTinghuChoice[0][0][0];
	 while(p<=&SecondTinghuChoice[2][13][22])
		 *p++=-1;
		/*******初始化停胡表*****************/
	 p=&TileToWin[0][0];
	 while(p<=&TileToWin[2][14])
		 *p++=-1;
	
		/*******初始化概率表,都为4**************/
	 p=&PossibilityToGet[0][0];
	 while(p<=&PossibilityToGet[2][20])
		 *p++=4;
	
		/******初始化赢牌标记数组和各类指针******/
	 for(i=0;i<3;i++)
	 {
		 SomeWin[i]=0;
		 PLow[i]=&Pile[i][0];
		 PHigh[i]=&Pile[i][12];
		 PengHigh[i]=&PengPile[i][0];
	 }

	 Pm=MainPile;
	 PmEnd=&MainPile[83];
	 Pd=&DirtyPile[0];
	 



}

/********洗牌*************/
void Shuffle(int Times)
{
	int i=0,random=0,j=0;
	
	srand((unsigned)time(NULL));
	for(j=0;j<Times;j++)
	{
		for(i=83;i>=1;i--)
		{
			random=rand()%(i+1);
			swap(MainPile+i,MainPile+random);
		}
		
	}
	
}

/************显示************/
void DisplayMain(void)
{
    int *p1=Pm;
	printf("MainPile:\n");
	while(p1<=&MainPile[83])
		printf("%s ",MainPileTable[*p1++]);
	printf("\n");
}
void DisplayDirty(void)
{
	int *p2=DirtyPile;
	printf("DirtyPile:\n");
	while(*p2!=-1)
		printf("%s ",MainPileTable[*p2++]);
	printf("\n");
}
void Display(int XPile)
{
	int *p1=&PengPile[XPile][0],*p2=PLow[XPile];
	int i=0;

	printf("       %c  Peng:",'A'+XPile);
	while(p1<PengHigh[XPile])
		printf("%s ",MainPileTable[*p1++]);
    printf("\n");
    printf("          hand:");
    while(p2<=PHigh[XPile])
		printf("%s ",MainPileTable[*p2++]);	
	printf("\n");
}

void DisplayAll(void)
{
    DisplayMain();
	DisplayDirty();
	Display(APile);
	Display(BPile);
	Display(CPile);
	printf("\n");
}


/*********发牌****************/
void Deal(void)                          //完成后，Pm指向第40张牌，Pm==&MainPile[39]
{
	int *P1=PLow[APile];
	int *P2=PLow[BPile];
    int *P3=PLow[CPile];
	int flag=0;
	int i=0;

	for(i=0;i<36;i++)
	{
		flag=(i/4)%3;
		if(flag==0)
			*P1++=MainPile[i];
		else if(flag==1)
			*P2++=MainPile[i];
		else *P3++=MainPile[i];
		Pm++;
	}
	*P1=*Pm++;
	*P2=*Pm++;
	*P3=*Pm++;
}





/**********排序****************/
void QuickSort(int *XPile,int low,int high)
{
	int x=XPile[low];
	int i=low,j=high;
	if(low<high)
	{
		while(i<j)
		{
			while(i<j && XPile[j]>=x )
			{
				j--;
				
			}
			if(i<j)
				XPile[i++]=XPile[j];
			while(i<j && XPile[i]<x)
			{
				i++;	 
			}
			if(i<j)
				XPile[j--]=XPile[i];
		}
		XPile[i]=x;
		QuickSort(XPile,low,i-1);
		QuickSort(XPile,i+1,high);
	}
}

void Sort(int XPile)
{   int low,high;
    low=PLow[XPile]-&Pile[XPile][0];
	high=PHigh[XPile]-&Pile[XPile][0];
	QuickSort(Pile[XPile],low,high);	
}

void SortAll(void)
{
	Sort(APile);
	Sort(BPile);
	Sort(CPile);
}


/*******手牌的各种统计量******/
void Statistics(int XPile)
{
	int *P=PLow[XPile];
	nTiao[XPile]=0;
	nBing[XPile]=0;
	nFeng[XPile]=0;
	nAll[XPile]=0;
	nX1[XPile]=0;
	nX2[XPile]=0;
	nX3[XPile]=0;
	while(P<=PHigh[XPile])
	{
		if((*P)/36==0)
			nTiao[XPile]++;
		else if((*P)/36==1)
			nBing[XPile]++;
		else 
		{
			nFeng[XPile]++;
			if(((*P)%36)/4==0)
				nX1[XPile]++;
			else if(((*P)%36)/4==1)
				nX2[XPile]++;
			else
				nX3[XPile]++;
		}
		P++;
	}
	nAll[XPile]=nTiao[XPile]+nBing[XPile]+nFeng[XPile];
	if(nTiao[XPile])
	{
		pTiao[XPile]=PLow[XPile];
		if(nBing[XPile])
		{
			pBing[XPile]=pTiao[XPile]+nTiao[XPile];
			if(nFeng[XPile])
				pFeng[XPile]=pBing[XPile]+nBing[XPile];
		}
		else
			if(nFeng[XPile])
				pFeng[XPile]=pTiao[XPile]+nTiao[XPile];
	}
	else
		if(nBing[XPile])
		{
			pBing[XPile]=PLow[XPile];
			if(nFeng[XPile])
				pFeng[XPile]=pBing[XPile]+nBing[XPile];
		}
		else
			 pFeng[XPile]=PLow[XPile];

}



/* 从p处取顺子，不能取则返回0，能取则将该顺子摆放到p，p+1，p+2*/
int TakeSequence(int *p)
{
	int *p1=p,*p2=p;
	int k1=0,k2=0,temp1=0,temp2=0;
	int eg=72;
	if(SameTile(p,&eg)) 
		return 0;
	while(SameTile(p,p1))
	{
		p1++;
		k1++;
	}
	if(!SequenceTile(p,p1)) return 0;
	p2=p1;
	temp1=*p2;
	while(SameTile(p2,p1))
	{
		p1++;
		k2++;
	}
	if(!SequenceTile(p2,p1)) return 0;
    temp2=*p1;
	while(*(--p1)!=*p2)
		*(p1+1)=*p1;
	while(*(--p2)!=*p)
		*(p2+2)=*p2;
	*(p+1)=temp1;
	*(p+2)=temp2;
	return 1;
}
/*从p处取刻子，不能取则返回0，能取则返回1*/
int TakeThree(int *p)
{
	if(SameTile(p,p+1) && SameTile(p+1,p+2)) return 1;
	else return 0;
}

int Combination(int a[],int n)
{
	int *p=a;
	if(n==0) return 1;
	while(n!=0)
	{
		if(TakeThree(p))
		{
			if(Combination(p+3,n-3))
			   return 1;
		   else return 0;
		}
		else if(TakeSequence(p))
			{
				if(Combination(p+3,n-3))
					return 1;
				else return 0;
			}
			else return 0;
	}
}



/********牌的组合表*******/
int AllCombination(int XPile)
{
	int *p=PLow[XPile],*p1=PLow[XPile],*p2=PLow[XPile],*p3=PLow[XPile];
	int k=0,n=0;
	int temp[12],*pt=temp;


           /*p:所取对的起指针  p2:取对后的下一指针  p1:下一候选对的起指针  p3:临时指针*/

	while(p1<PHigh[XPile])
	{
		while(SameTile(p,p1))
		{
			p1++;
			k++;
		}
		if(k==1) 
		{
			p=p1;
			k=0;
			continue;
		}
            /*****k>=2,可以取做对*******/
            /*将除对子外的其他牌放到临时数组中*/
		p2=p+2;
		pt=temp;
		n=0;
		if(p==PLow[XPile])							
		{	
			while(p2<=PHigh[XPile]) 
			{
				*pt++=*p2++;
				n++;
			}
			    
		}
		else
		{
			p3=PLow[XPile];
			while(p3!=p)
			{
				*pt++=*p3++;
				n++;
			}
			while(p2<=PHigh[XPile])
			{
				*pt++=*p2++;
				n++;
			}
		}
		
		if(Combination(temp,n))
			return 1;
		p=p1;
		k=0;
	}
    return 0;
	
}


/********判断胡牌***********/
int Win(int XPile)
{	int *p=PLow[XPile];
    Statistics(XPile);
	if(nFeng[XPile]==1 || nFeng[XPile]==4) return 0;          //1或4张风无法胡，直接返回0
	if(nFeng[XPile]==7 || nFeng[XPile]==10) return 0;         //7或10张风无法胡，直接返回0
	if(nFeng[XPile]==0 || nFeng[XPile]==3)
		 if(nTiao[XPile]==4 || nTiao[XPile]==10) 
				return 0;                       //0或3张风时，如果有4或10张条则无法胡，返回0
	if(nFeng[XPile]==2 || nFeng[XPile]==5)
			if((nTiao[XPile]/3)!=0) 
				return 0;                       //2或5张风时，如果nTiao不为3的倍数则无法胡，返回0	
	

	if((PHigh[XPile]-PLow[XPile])==13)          //7对
	{
		while(p<PHigh[XPile] && SameTile(p,p+1)) 
			p+=2;
		if(p>PHigh[XPile]) return 1;
	}

    if(AllCombination(XPile))
		return 1;
	else return 0;	
}

/********是否停胡,是则将要胡的牌放到FirstTinghuChoice[XPile]的下一行********/
/********该行首元素为待胡牌的个数，返回行序号.不听胡返回-1*********/
int SingleFirst(int XPile)
{
	int i=0,ok=-1,nok=-1,UpLimit=83,LowLimit,n=0;
	int *p=PLow[XPile],*p2=PHigh[XPile],*p4=PLow[XPile],*p5=PLow[XPile];
	int *p3;

	/*********缩小摸牌范围**************/
	LowLimit=*PLow[XPile]-4;
	if(LowLimit<0)
		LowLimit=0;
	UpLimit=*PHigh[XPile]+4;
	if(UpLimit>83)
		UpLimit=83;

	/********找到下一行**************/
	while(FirstTinghuChoice[XPile][i][0]!=-1) 
		i++;
	n=i;
	
	/********先将行首个数-1设置为0，p3指向其后,等待写入****************/
	FirstTinghuChoice[XPile][n][0]=0;
	p3=&FirstTinghuChoice[XPile][n][1];

    for(i=LowLimit;i<=UpLimit;i++)
	{
	    /******跳过刚扔的牌、手里同序号的牌、已试过的相同牌*******/
		if(SameTile(&i,Pd-1)) continue;
		if(iInXPile(XPile,i)) continue;
		if(ok!=-1 && SameTile(&i,&ok)) continue;
		if(nok!=-1 && SameTile(&i,&nok)) continue;
		*++PHigh[XPile]=i;
		Sort(XPile);

		/******如果赢了，向停胡选择表里写入它，行首个数加1。不论赢否，标记该牌*******/
		if(Win(XPile))
		{
			*p3++=i;
			ok=i;
			FirstTinghuChoice[XPile][n][0]++;
		}
		else
			nok=i;

		/******判断完该i后，恢复原状**************************/
		while(p4<PHigh[XPile])
		{
			if(*p4==i)
			{
				p5=p4;
				while(p5<PHigh[XPile])
				{
					*p5=*(p5+1);
					p5++;
				}
				--PHigh[XPile];
				p4=PLow[XPile];
				break;
			}
			else
				p4++;
		}
		if(p4==PHigh[XPile])
			--PHigh[XPile];
	}
	
	/*******如果行首为0，说明没有可以停胡的牌，恢复为-1，返回-1.否则返回行号n********************/
	if(FirstTinghuChoice[XPile][n][0]==0)
	{
		FirstTinghuChoice[XPile][n][0]=-1;
		return -1;
	}
	return n;

}

/***********判断手牌是否停胡，将要胡的牌放到停胡表中***********/
int FormalTry(int XPile)
{
	int *p1,*p2=&TileToWin[XPile][0];

	/******清空停胡选择表和停胡表******/
	ClearFirstTinghuChoice(XPile);
	p1=&TileToWin[XPile][0];
	while(p1<=&TileToWin[XPile][14])
		*p1++=-1;

	if(SingleFirst(XPile)==-1) 
		return 0;

	/******将停胡选择表第一行复制到停胡表****/
	p1=&FirstTinghuChoice[XPile][0][1];
	while(*p1!=-1)
		*p2++=*p1++;

	/******清空停胡选择表****************/
	ClearFirstTinghuChoice(XPile);
	return 1;
}

/*****是否打出哪张牌后就能停胡**********/
/*****是，则更新FirstTinghuChoice[XPile]，并在每行尾加上其要出的牌。*****/
/*****返回要打的最优牌的序号。否则返回-1********/
int FirstTinghu(int XPile)
{
	int n=0,better=-1,bigger=0,current=0,temp;
    int *p1=PLow[XPile],*p2=PLow[XPile],*p3=&FirstTinghuChoice[0][0][0];

		/*****************清除FirstTinghuChoice******************/
    
	ClearFirstTinghuChoice(XPile);
	while(p1<=PHigh[XPile])
	{
		/***挨个出牌。跳过相同的牌，记下出的牌temp****/
		if(p1!=PLow[XPile] && SameTile(p1-1,p1)) 
		{
			p1++;
			continue;
		}
		temp=*p1;
		ThrowAction(XPile,(p1-PLow[XPile]));

		/*****如果赢了，将要出的牌添加到行尾，并选出胡牌数最多的better*********/
		if((n=SingleFirst(XPile))!=-1)
		{
			if(n==0)
			{
				better=p1-PLow[XPile];
				bigger=FirstTinghuChoice[XPile][n][0];
				FirstTinghuChoice[XPile][n][bigger+1]=temp;
			}
			else 
			{
				if ((current=FirstTinghuChoice[XPile][n][0])>=bigger)
				{
				bigger=current;
			    better=p1-PLow[XPile];
				}
				FirstTinghuChoice[XPile][n][current+1]=temp;
			}
		}
        
		/********恢复，试下一个牌*********/
		UnThrow(XPile);
		p1++;
	}
	/******返回使停胡个数最多的要出的牌的序号**********/
	return better;
}


/********二阶停胡填表,XPile为13张，是则将要胡的牌放到FirstTinghuChoice[XPile]的下一行********/
/********该行首元素为待胡牌的个数，返回行序号.不听胡返回-1*********/
int SingleSecond(int XPile)
{
	int i=0,ok=-1,nok=-1,n=0;
	int *p=PLow[XPile],*p2=PHigh[XPile],*p4=PLow[XPile],*p5=PLow[XPile];
	int *p3;
	
	
	while(SecondTinghuChoice[XPile][i][0]!=-1) 
		i++;
	n=i;
	SecondTinghuChoice[XPile][n][0]=0;
	p3=&SecondTinghuChoice[XPile][n][1];
    for(i=0;i<=83;i++)
	{
		if(SameTile(&i,Pd-1)) continue;
		if(iInXPile(XPile,i)) continue;
		if(ok!=-1 && SameTile(&i,&ok)) continue;
		if(nok!=-1 && SameTile(&i,&nok)) continue;
		*++PHigh[XPile]=i;
		Sort(XPile);
		if(FirstTinghu(XPile)!=-1)
		{
			ClearFirstTinghuChoice(XPile);
			*p3++=i;
			ok=i;
			SecondTinghuChoice[XPile][n][0]++;
		}
		else
			nok=i;
		while(p4<PHigh[XPile])
		{
			if(*p4==i)
			{
				p5=p4;
				while(p5<PHigh[XPile])
				{
					*p5=*(p5+1);
					p5++;
				}
				--PHigh[XPile];
				p4=PLow[XPile];
				break;
			}
			else
				p4++;
		}
		if(p4==PHigh[XPile])
			--PHigh[XPile];
	}
	if(SecondTinghuChoice[XPile][n][0]==0)
	{
		SecondTinghuChoice[XPile][n][0]=-1;
		return -1;
	}
	return n;	
}

/*****二阶停胡（打一张牌后能FirstTinghu）**********/
/*****是，则更新SecondTinghuChoice[XPile]，并在每行加上其要出的牌,返回要打的最优牌的序号*****/
/*****否则返回-1********/
int SecondTinghu(int XPile)
{
	int n=0,better=-1,bigger=0,current=0,temp;
    int *p1=PLow[XPile],*p2=PLow[XPile],*p3=&FirstTinghuChoice[0][0][0];

		/*****************清除SecondTinghuChoice******************/
	ClearSecondTinghuChoice(XPile);

	while(p1<=PHigh[XPile])
	{
		/***********跳过相同的牌**************/
		if(p1!=PLow[XPile] && SameTile(p1-1,p1)) 
		{
			p1++;
			continue;
		}
		temp=*p1;
		ThrowAction(XPile,(p1-PLow[XPile]));

		/*****如果二阶停胡，选出better*********/
		if((n=SingleSecond(XPile))!=-1)
		{
			if(n==0)
			{
				better=p1-PLow[XPile];
				bigger=SecondTinghuChoice[XPile][n][0];
				SecondTinghuChoice[XPile][n][bigger+1]=temp;
			}
			else 
			{
				if ((current=SecondTinghuChoice[XPile][n][0])>bigger)
				{
				bigger=current;
			    better=p1-PLow[XPile];
				}
				SecondTinghuChoice[XPile][n][current+1]=temp;
			}
		}

		UnThrow(XPile);
		p1++;
	}
	

	return better;
}

int OthersWin(int XPile)
{
	int NextPile[2];
	int i=0;
	int *p1;
	int winflag=0;
	NextPile[0]=XPile;
	NextPile[1]=(XPile+1)%3;
	if(Pd==DirtyPile) return 0;
	for(i=0;i<2;i++)
	{
			p1=&TileToWin[NextPile[i]][0];
			while(*p1!=-1)
			{
				if(SameTile(p1,Pd-1))
				{
				    SomeWin[NextPile[i]]=1;
					SomeWin[(XPile+2)%3]--;
					winflag=1;
					break;
				}
				else
					p1++;
			}
	}
	return winflag;
}


/*********正常接牌***********/
void TakeFront(int XPile)
{
	*++PHigh[XPile]=*Pm++;
	MainLeftNum--;
	printf("%c takes %s\n",'A'+XPile,MainPileTable[*PHigh[XPile]]);
}
	
/*********杠接牌************/
void TakeBack(int XPile)
{
	*++PHigh[XPile]=*PmEnd--;
	MainLeftNum--;
	printf("%c takes %s from behind\n",'A'+XPile,MainPileTable[*PHigh[XPile]]);
}



/*****从废牌堆中取回一张，放回手牌中*******/
void UnThrow(int XPile)
{
	int *p=PLow[XPile],temp,*p2=PHigh[XPile];
	temp=*--Pd;
	while(*p<temp) p++;
    while(p2>=p)
	{
		*(p2+1)=*p2;
		p2--;
	}
	*++p2=temp;
	PHigh[XPile]++;
	*Pd=-1;
}
/********出牌动作,specific为输入的要出的牌的对PLow的偏移量，如果为-1，则按照算法出牌*************/
void ThrowAction(int XPile,int specific)
{   
    int one=0,i=0;
	char answer='a';
	char *p1=MainPileTable[0];
	int *p2=PLow[XPile];
	
	
    if(specific!=-1)
		one=specific;
	else
	{
		if(XPile!=APile)
			one=OneToThrow(XPile);
		else
		{
			
			Display(XPile);
			do 
			{
			printf("        (Input T1/t1) Let's Throw:");
			scanf("%s",&answer);
			}
			while((one=QualifyThrowAnswer(&answer,XPile))==-1);	
		}
	}
	p2=PLow[XPile]+one;
	*Pd++=*p2;
    while(p2<PHigh[XPile])
	{
		*p2=*(p2+1);
		p2++;
	}
	PHigh[XPile]--;
	
}
/********出牌****************/
void Throw(int XPile)
{
	int better1=-1,better2=-1,better=-1,i=0,ting=0,n=0,temp=0,*p1,*p2;

	/********更新停胡选择表****************************************/
	if((better1=FirstTinghu(XPile))==-1)
		better2=SecondTinghu(XPile);

	

	/********如果1阶停胡，人出牌则提示所有可以停胡的可能**********/
	if(better1!=-1)
	{
		if(XPile==APile)
		{
			while(FirstTinghuChoice[XPile][i][0]!=-1)
				{
					/************p1指首，p2指尾***************/
					p2=&FirstTinghuChoice[XPile][i][0];
					while(*p2!=-1) 
						p2++;
					p2--;
					printf("        Tips:if you throw %s ,you will tinghu ",MainPileTable[*p2]);
					p1=&FirstTinghuChoice[XPile][i][1];
					while(p1!=p2)
					{
						printf("%s ",MainPileTable[*p1]);
						p1++;
					}
					printf("\n");
					i++;
				}
		    ThrowAction(XPile,-1);
		}
		else
			ThrowAction(XPile,better1);
	}
	else    /*************如果2阶停胡*******************/
		if(better2!=-1)
		{
			if(XPile==APile)
			{
				while(SecondTinghuChoice[XPile][i][0]!=-1)
				{
					/************p1指首，p2指尾***************/
					p2=&SecondTinghuChoice[XPile][i][0];
					while(*p2!=-1) 
						p2++;
					p2--;
					printf("        Tips:Throw %s ---> If get ",MainPileTable[*p2]);
					p1=&SecondTinghuChoice[XPile][i][1];
					while(p1!=p2)
					{
						printf("%s ",MainPileTable[*p1]);
						p1++;
					}
					printf("---> tinghu!\n");
					i++;
				}
				ThrowAction(XPile,-1);
			}
			else
			ThrowAction(XPile,better2);
		}
		else
			ThrowAction(XPile,-1);
    printf("%c throws %s\n",('A'+XPile),MainPileTable[*(Pd-1)]);
   
	if(FormalTry(XPile)) 
		Warn(XPile);
	else
	{
		for(i=0;i<14;i++)
			TileToWin[XPile][i]=-1;
	}
	
	
}






/****能否碰，返回碰的牌的偏移量，返回-1为不能碰*****/
int AbleToPeng(int XPile)
{
	int *p=PLow[XPile],*p1=Pd-1,n=0;
	if(Pd==DirtyPile) return -1;
	while(p<PHigh[XPile])
		if(SameTile(p,p1) && SameTile(p,p+1))
		{
			n=p-PLow[XPile];
			return n;
		}
		else p++;
    return -1;
}


/*******是否碰的选择算法************/
int GoodToPeng(int XPile)
{
	char penganswer='a';
	if(XPile==APile)
	{
		printf("PlayerA Peng?:(y/n)");
		scanf("%s",&penganswer);
		if(penganswer=='y' || penganswer=='Y')
			return 1;
		else 
			return 0;
	}
	else
		return 1;
}


int Peng(int XPile)
{
	int *p=PLow[XPile],*p1=Pd-1,*p2=PLow[XPile],n=0;
	int temp1=0,temp2=0,temp3=0,j=0;
	if(Pd==DirtyPile) 
		return 0;
	if(((n=AbleToPeng(XPile))!=-1) && (GoodToPeng(XPile)))
    {
		p+=n;
		*++PHigh[XPile]=*--Pd;
		*Pd=-1;
		Sort(XPile);
		for(j=0;j<3;j++)
		*PengHigh[XPile]++=*p++;
		while(p<=PHigh[XPile])
		{
			*(p-3)=*p;
			p++;
		}
		PHigh[XPile]-=3;

		printf("%c Peng\n",'A'+XPile);
		return 1;
	}
	else 
		return 0;

}

/**********是否有人碰，有则返回下一个接牌的XPile，无则返回-1******/
int OthersPeng(int XPile)
{
	int NextPile[3];
	int i=0,Next=-1,SomePeng=0;

	NextPile[0]=XPile;
	NextPile[1]=(XPile+1)%3;
	NextPile[2]=(XPile+2)%3;
	for(i=0;i<2;i++)
	{
		if(Peng(NextPile[i]))
		{
			
			Throw(NextPile[i]);
			Next=NextPile[i+1];
			SomePeng=1;
			break;	
		}
	}
	
	return Next;
}

/********能点杠，返回杠的首偏移量，否则返回-1********/
int AbleToDianGang(int XPile)
{
	int *p=PLow[XPile],*p1=Pd-1,n=0;
	if(Pd==DirtyPile || PLow[XPile]==PHigh[XPile]) return -1;
	/****PHigh-PLow>=3*****/
	while(p<=(PHigh[XPile]-2))
		if((SameTile(p,p1) && SameTile(p,p+1)) && SameTile(p,(p+2)))
		{
			n=p-PLow[XPile];
			return n;
		}
		else p++;
    return -1;
}

/********如果杠有益身心，返回1，否则返回0**********/
int GoodToGang(int XPile)
{
	char ganganswer='a';
	if(XPile==APile)
	{
		printf("PlayerA Gang?:(y/n)");
		scanf("%s",&ganganswer);
		if(ganganswer=='y' || ganganswer=='Y')
			return 1;
		else 
			return 0;
	}
	else
		return 1;
}

int DianGang(int XPile)
{
	int *p=PLow[XPile],*p1=Pd-1,*p2=PLow[XPile],n=0,j=0;
	int temp1=0;
	if(Pd==DirtyPile) 
		return 0;
	if(((n=AbleToDianGang(XPile))!=-1) && (GoodToGang(XPile)))
	{
		p+=n;
		*++PHigh[XPile]=*--Pd;
		*Pd=-1;
		Sort(XPile);
		for(j=0;j<4;j++)
			*PengHigh[XPile]++=*p++;
		while(p<=PHigh[XPile])
		{
			*(p-4)=*p;
			p++;
		}
		PHigh[XPile]-=4;
		printf("%c Gang\n",'A'+XPile);
		return 1;
	}
	else 
		return 0;	
}

int AbleToSelfGang(int XPile)
{
	int *p=PLow[XPile],n;
	if((PHigh[XPile]-PLow[XPile])==0) return -1;
	while(p<=(PHigh[XPile]-3))
		if((SameTile(p,p+1) && SameTile(p,p+2)) && SameTile(p,(p+3)))
		{
			n=p-PLow[XPile];
			return n;
		}
		else p++;
	return -1;
}

int SelfGang(int XPile)
{
	int *p=PLow[XPile],*p1=Pd-1,*p2=PLow[XPile],n=0,j=0;
	int temp=0;
	if(((n=AbleToSelfGang(XPile))!=-1) && (GoodToGang(XPile)))
	{
		p+=n;
		for(j=0;j<4;j++)
			*PengHigh[XPile]++=*p++;
		while(p<=PHigh[XPile])
		{
			*(p-4)=*p;
			p++;
		}
		PHigh[XPile]-=4;
		/*if(n==0)
			PLow[XPile]+=4;
		else
		{
			p2=p-1;
			temp=*p;
			while(p2>=PLow[XPile])
			{
				*(p2+4)=*p2;
				p2--;
			}
			*PLow[XPile]++=temp;
			*PLow[XPile]++=temp+1;
			*PLow[XPile]++=temp+2;
			*PLow[XPile]++=temp+3;
		}*/
		
		printf("%c Gang\n",'A'+XPile);
		return 1;
	}
	else 
		return 0;	
}

int OthersGang(int XPile)
{
	int NextPile[3];
	int i=0,Next=-1,Somegang=0;
	
	NextPile[0]=XPile;
	NextPile[1]=(XPile+1)%3;
	NextPile[2]=(XPile+2)%3;
	for(i=0;i<2;i++)
	{
		if(DianGang(NextPile[i]))
		{
			
			TakeBack(NextPile[i]);
			Sort(NextPile[i]);
			Throw(NextPile[i]);
			Next=NextPile[i+1];
			Somegang=1;
			break;	
		}
	}
	
	return Next;
}

void GetPossibility(int XPile)
{
	int *p=DirtyPile;
	int tile,i=0;
	/********出牌堆里的牌***********/
	while(*p!=-1)
	{
		tile=(*p)/4;
		PossibilityToGet[XPile][tile]--;
		p++;
	}
	/********三方已经碰杠的牌********/
	for(i=0;i<3;i++)
	{
		p=&PengPile[XPile][0];
		while(p<=PengHigh[XPile])
		{
			tile=(*p)/4;
			PossibilityToGet[XPile][tile]--;
			p++;
		}
	}
	/********自己手里的牌*************/
	p=PLow[XPile];
	while(p<=PHigh[XPile])
	{
		tile=(*p)/4;
		PossibilityToGet[XPile][tile]--;
		p++;
	}
}

/*****判断*p是否成对或靠张，都不返回0，成对返回1，靠张返回2***********/
int XHasCompany(int XPile,int *p)
{
	int temp1,temp2;
	temp1=*p-4;
	temp2=*p+4;
	if(XInXPile(XPile,*p)>=2) return 1;
	if((XInXPile(XPile,temp1) && SameKind(p,&temp1)) || (XInXPile(XPile,temp2) && SameKind(p,&temp2)))
		return 1;
	else
		return 0;
}

/********核心算法***********/
int OneToThrow(int XPile)
{
	int random=0,k1=0,k2=0,tile1,tile2;
	int *p1=PLow[XPile],*p2=PHigh[XPile];
	Statistics(XPile);
	GetPossibility(XPile);
	
	/******两张牌，扔概率小的******/
	if(nAll[XPile]==2)
	{
		tile1=(*p1)/4;
		tile2=(*p2)/4;
		/****有五，前期扔5，后期留5****/
		if(tile1==4 || tile1==13)
			if(MainLeftNum>20)
				return 0;
			else return 1;
		if(tile2==4 || tile2==13)
			if(MainLeftNum>20)
				return 1;
			else 
				return 0;

		return (PossibilityToGet[XPile][tile1]>PossibilityToGet[XPile][tile2]?1:0);
	}
    
	/******5张以上******/

	if(nFeng[XPile])
	{
		p1=pFeng[XPile];
		
		/*******单风***********/
		if(nX1[XPile]==1) 
			return (p1-PLow[XPile]);
		if(nX2[XPile]==1)
			return (p1+nX1[XPile]-PLow[XPile]);
		if(nX3[XPile]==1)
			return (p1+nX1[XPile]+nX2[XPile]-PLow[XPile]);
		}

	/*******单1、9条1、9饼*********/
	if(nTiao[XPile])
	{
		if(nTiao[XPile]==1) return 0;

		/****单1条,没2或者没3 直接打掉*****/
		if(XInXPile(XPile,0)==1)
		{
			if(!XInXPile(XPile,4) || !XInXPile(XPile,8))
				return 0;
		}
		
		/****单9条,没7或者没8，直接打掉*****/
		if(XInXPile(XPile,32)==1)
			if(!XInXPile(XPile,28) || !XInXPile(XPile,24))
				return (nTiao[XPile]-1);
	}

	if(nBing[XPile])
	{
		if(nBing[XPile]==1) return nTiao[XPile];
		
		/****单1饼*****/
		if(XInXPile(XPile,36)==1)
			if(!XInXPile(XPile,40) || !XInXPile(XPile,44))
				return nTiao[XPile];
			
		/****单9饼*****/
		if(XInXPile(XPile,68)==1)
			if(!XInXPile(XPile,64) || !XInXPile(XPile,60))
				return (nTiao[XPile]+nBing[XPile]-1);
	}
    
	/******不靠张**********/
	p1=PLow[XPile];
	while(p1<=PLow[XPile]+nTiao[XPile]+nBing[XPile]-1)
	{
		
		if(!XHasCompany(XPile,p1)) 
			return (p1-PLow[XPile]);
		p1++;
	}
	
	/********只剩对子和连续***********/
		return 0;
}

void main(void)
{


	int i;
	char answer;
	int MainLeftNum;
	int TempXPile;
	int XPile;
	char endchar='a';
	do 
	{

	i=0;
	answer='a';
	MainLeftNum=45;
	TempXPile=0;
	XPile=APile;
    
	
	InitAllData();
	Shuffle(3);	                //洗牌
    Deal();						//发牌
	SortAll();					//整牌
	ShowWelcome();
 //   DisplayAll();
	
	while(MainLeftNum>0)
	{
	
		/*******是否有人胡********/
		if(OthersWin(XPile))
		{
			printf("\n");
			break;
		}
		
		/*******是否有人杠********/
		if((TempXPile=OthersGang(XPile))!=-1)
		{
			XPile=TempXPile;
			continue;
		}

		/*******是否有人碰********/
		if((TempXPile=OthersPeng(XPile))!=-1)			
		{
			XPile=TempXPile;
			continue;
		}
		
		/*********摸牌***********/
		TakeFront(XPile);
		Sort(XPile);
		
		/*******是否自摸********/
		if(Win(XPile))
		{
			SomeWin[XPile]=2;
			printf("\n");
			break;
		}

		/*******是否暗杠********/
		if(SelfGang(XPile))
		{
			TakeBack(XPile);
			Sort(XPile);
		}

		/*******出牌***********/
		Throw(XPile);
		XPile=(XPile+1)%3;
	 }
	}
	while(GameOver()=='y');
    scanf("%s",&endchar);

    /*测试部分
	int XPile=APile;
	int i,j,k;
	PHigh[XPile]++;
	InitAllData();
	
	Display(XPile);
	if(SelfGang(XPile)!=-1)  
	{
	     Display(XPile);
	}
	else
			printf("NONONO");*/

}

