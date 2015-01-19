// 4i902View.cpp : implementation of the Ci902View class
//

#include "stdafx.h"
#include "4i902.h"

#include "4i902Doc.h"
#include "4i902View.h"
#include ".\4i902view.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci902View

IMPLEMENT_DYNCREATE(Ci902View, CFormView)

BEGIN_MESSAGE_MAP(Ci902View, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, OnDisplayGraph)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTONDENSITYTIMEGRAPH, OnBnClickedButtondensitytimegraph)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_UNEQUALCLIQUE, OnBnClickedUnequalclique)
	ON_BN_CLICKED(IDC_TOGGLEBUTTON, OnBnClickedTogglebutton)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
END_MESSAGE_MAP()

// Ci902View construction/destruction

Ci902View::Ci902View()
	: CFormView(Ci902View::IDD)
	, m_NoOfVeriticesL(10)
	, m_DensityOfGraph(60)
	, m_NoOfVeriticesM(10)
	, m_CliqueDensity(50)
{
	// TODO: add construction code here
	redrawCounter=0;
	gIndex=0;
	unEqualGraph=0;
}

Ci902View::~Ci902View()
{
}

void Ci902View::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_NoOfVeriticesL);
	DDX_Text(pDX, IDC_EDIT2, m_DensityOfGraph);
	DDX_Text(pDX, IDC_EDIT3, m_NoOfVeriticesM);
	DDX_Control(pDX, IDC_LIST_M, m_ListM);
	DDX_Control(pDX, IDC_LIST_L, m_ListL);
	DDX_Text(pDX, IDC_EDIT4, m_CliqueDensity);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressBar);
}

BOOL Ci902View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void Ci902View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
}


// Ci902View diagnostics

#ifdef _DEBUG
void Ci902View::AssertValid() const
{
	CFormView::AssertValid();
}

void Ci902View::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

Ci902Doc* Ci902View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Ci902Doc)));
	return (Ci902Doc*)m_pDocument;
}
#endif //_DEBUG


// Ci902View message handlers

void Ci902View::OnDisplayGraph()
{
	// TODO: Add your control notification handler code here
	SYSTEMTIME st;
	m_ProgressBar.SetRange(0,100);
	progressing=0;

	// capture the system time at the start of clique finding
	GetSystemTime(&st);
	long sTMillisecond=st.wMilliseconds;
	long sTSecond=st.wSecond;
	long sTMinute=st.wMinute;

	redrawCounter=1;
	clearPressed=0;
	indexL=0;indexM=0;uIndexL=0;uIndexM=0;
	// get the data from the interface
	UpdateData(TRUE);
	// create the matrix based on user input L*M
	// where L = Set1 & M = Set2
	graphMatrix = new int*[m_NoOfVeriticesL];
	
	for(int i=0;i<m_NoOfVeriticesL;i++)
		graphMatrix[i]=new int[m_NoOfVeriticesM];

	for( i=0;i<m_NoOfVeriticesL;i++)
		for(int j=0;j<m_NoOfVeriticesM;j++)
			graphMatrix[i][j]=0;
	// compute the number of edges allowed
	// L * M = max number of edges 
	// M is the columns in matrix and L is  row
	maxEdgesAllowed = m_NoOfVeriticesL*m_NoOfVeriticesM;

	// formula to find the no of edges present in the graph
	noOfEdgesPresent=maxEdgesAllowed*((float)m_DensityOfGraph/100);
	
	int dec = noOfEdgesPresent;
	int index=0,row,column;
	
	srand((unsigned)time(NULL));

	while(dec!=0)
	{
		index = rand()%maxEdgesAllowed;
		row = (index/m_NoOfVeriticesM);
		column = (index % m_NoOfVeriticesM);//M/// -1 ;
		if(graphMatrix[row][column]==0) //&& row >-1 && column >-1 && row < m_NoOfVeriticesM && column < m_NoOfVeriticesL)
		{
			graphMatrix[row][column] = 1;
			dec--;
		}
	}

	SetProgressBar();
	DrawGraph();

	SetProgressBar();

	if(m_DensityOfGraph!=100)
		PrepareForCombination();
	else
	{
		CString str;
		for(int i=0;i<m_NoOfVeriticesL;i++)
		{
			topL[indexL++]=i;
			str.Format("%d",i+1); // row, L, top
			m_ListL.AddString(str);
		}

		for(int j=0;j<m_NoOfVeriticesM;j++)
		{
			bottomM[indexM++]=j;
			str.Format("%d",j+1); // column, M, bottom
			m_ListM.AddString(str);
		}

	}// end of else

	SetProgressBar();
	
	DrawCliques();

	SetProgressBar();

	HighlightClique();

	// capture the system time at the end of clique finding
	GetSystemTime(&st);
	long eTMillisecond=st.wMilliseconds;
	long eTSecond=st.wSecond;
	long eTMinute=st.wMinute;

	// perform subtraction to compute the actual time
	eTMillisecond-=sTMillisecond;
	eTSecond-=sTSecond;
	eTMinute-=sTMinute;

	eTSecond*=1000;
	eTMinute=(eTMinute*60)*1000;

	// calculate time in milliseconds.
	totalTime=(eTMillisecond+eTSecond+eTMinute);
	if(uIndexL > 0 && uIndexM > 0)
	{	// we successfully found a clique of user specified density.
		gDensity[gIndex]=(int)m_CliqueDensity;
		gTime[gIndex++]=totalTime;
	}

	m_ProgressBar.SetPos(100);
}// end of  function


void Ci902View::DrawGraph(void)
{

CDC *pDC= GetDC();
CBrush brushL(RGB(100,255,0));
CBrush brushM(RGB(250,25,200));
CString str;
pDC->SelectObject(&brushL);
CRect rect;

rect.top =170;//150
rect.bottom =200;//180
rect.left =30;
rect.right =60;

for(int i=0;i<m_NoOfVeriticesL;i++)
{	
	pDC->Ellipse(&rect);
	str.Format("%d",i+1);
	pDC->TextOut(rect.left+10,rect.top+8,str);
	rect.left+=35;
	rect.right+=35;
}

rect.top =270;//250
rect.bottom =300;//280
rect.left =30;
rect.right =60;
pDC->SelectObject(&brushM);

for( i=0;i<m_NoOfVeriticesM;i++)
{	
	pDC->Ellipse(&rect);
	str.Format("%d",i+1);
	pDC->TextOut(rect.left+10,rect.top+8,str);
	rect.left+=35;
	rect.right+=35;
}


// now draw the lines.
int x=30;
int y=170;//150

for(int l=0;l<m_NoOfVeriticesL;l++)
	for(int m=0;m<m_NoOfVeriticesM;m++)
	{
		if(graphMatrix[l][m]==1)
		{ // draw an edge between them.
			x=(l*35)+30+15;
			y=200;//180 // bottom of upper circle
			pDC->MoveTo(x,y);
			x=(m*35)+30+15;
			y=270; //250// top of lower circle
			pDC->LineTo(x,y);

		}

	}


}// end of function

void Ci902View::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CFormView::OnPaint() for painting messages
	if(redrawCounter==1 && clearPressed!=1 && unEqualGraph==0)
	{
		CDC *pDC = GetDC();
		CBrush paintBackground(RGB(212,208,200));
		pDC->SelectObject(&paintBackground);
		pDC->Rectangle(10,167,1000,700);
		CString str;
		str.Format("Above is the Original Graph and below are the cliques found from this graph after processing.");
		pDC->TextOut(20,308,str);
		str.Format("Cliques of User Specified Density, if found are in Blue & Yellow color.");
		pDC->TextOut(20,480,str);

		DrawGraph();
		DrawCliques();
		HighlightClique();
	}
	else if(unEqualGraph==1 && clearPressed!=1)
	{
		CDC *pDC = GetDC();
		CBrush paintBackground(RGB(212,208,200));
		pDC->SelectObject(&paintBackground);
		pDC->Rectangle(10,167,1000,700);
		CString str;
		str.Format("Above is the Original Graph and below are the cliques found from this graph after processing.");
		pDC->TextOut(20,308,str);
		DrawGraph();
		DrawCliquesUnEqual();

	}


}

void Ci902View::PrepareForCombination(void)
{
	
	int combSum=0,combIndex=0;
	int **totalComb;
	int * dataSetN;
	int * dataSetR;
	dataSetN = new int[m_NoOfVeriticesL];

	for(int k=0;k<m_NoOfVeriticesL;k++)
		dataSetN[k]=k;

	for(int r=m_NoOfVeriticesL-1;r>0;r--)
	{
		SetProgressBar();
		combSum =  ( (BIG) GetFactorial(m_NoOfVeriticesL) )/( (BIG) GetFactorial(r)*GetFactorial(m_NoOfVeriticesL-r) )  ;
		if(combSum==0)
			combSum=1;
		combIndex=0;
		totalComb = new int*[combSum];
		for(int j=0;j<combSum;j++)
			totalComb[j] = new int[r];
	// we know the number of entries in each combination which is equal to r here.
	// so fill the array with required number of integers and send to get the combinations
		
		dataSetR = new int [r];

		for(int k=0;k<r;k++)
			dataSetR[k]=k;

		do
		{
			// here we'll process each combination or record it for future use.
			for(int h=0;h<r;h++)
				totalComb[combIndex][h] = dataSetR[h];
			
			combIndex++; // available combinations with r objects
		} 
		while(GetCombination(dataSetN,(dataSetN + (m_NoOfVeriticesL)),dataSetR ,(dataSetR + r)));

	// indicate the progress to the user
		SetProgressBar();



	// process the obtained values of the combinations to find clique in the graph
CString str;
int oneFound=0;
int cliqueFound=0;
int invalidM=0,invalidL=0;
int redEntry=0;

	for(int b=0;b<combIndex;b++)
	{			
			invalidL=0;
			for(int g=0;g<r;g++)
				for(int u=0;u<indexL;u++)
					{	
						if(topL[u]==totalComb[b][g])
							invalidL=1;
					}
				if(invalidL==1)
						continue;

		for(int c=0;c<combIndex;c++)
		{
			// comparison algorithm will go here.
			// get the values of the nodes.
			oneFound=0;
			invalidM=0;
				for(int g=0;g<r;g++)
					for(int u=0;u<indexM;u++)
					{	
						if(bottomM[u]==totalComb[c][g])
							invalidM=1;
					}
					if(invalidM==1)
						continue;

			if(redEntry==1)
			{
				invalidL=0;
				for(int g=0;g<r;g++)
				{	for(int u=0;u<indexL;u++)
						{	
							if(topL[u]==totalComb[b][g])
							invalidL=1;
						}
				}
				redEntry=0;
					if(invalidL==1)
						break;

			}

			for(int hL=0;hL<r;hL++)
			{	
				for(int hM=0;hM<r;hM++)
				{	//totalComb[c][h]; // column, M, bottom
					//totalComb[b][h]; // row, L, top

					if(graphMatrix[ totalComb[b][hL] ][ totalComb[c][hM] ]==1)
					{	
						oneFound++;
					}
				}// end of inner most for loop
			}
			if(oneFound==r*r)//zeroFound==0)
			{	
				cliqueFound++;
				str.Format("######");
				m_ListM.AddString(str);
				m_ListL.AddString(str);
				for(int h=0;h<r;h++)
				{	bottomM[indexM++]=totalComb[c][h]; // record the vertices involved in the clique
					topL[indexL++]=totalComb[b][h];
					str.Format("%d",totalComb[c][h]+1); // column, M, bottom
					m_ListM.AddString(str);
					str.Format("%d",totalComb[b][h]+1); // row, L, top
					m_ListL.AddString(str);
				}
				redEntry=1;
				bottomM[indexM++]=-1; 
				topL[indexL++]=-1;
				//str.Format("Clique Found of Size=%d",r);
				//MessageBox(str);
			}

		}// end of inner for loop

	}// end of for loop
	
	SetProgressBar();
	
	delete []dataSetR;
	delete []totalComb;
	
	}// end of outer for loop

	
GetUserDensityClique();

}// end of function




unsigned long double Ci902View::GetFactorial(int num)
{
	globalSum=1;
	Factorial(num);
	return re;
}


unsigned long double Ci902View::Factorial(int num)
{
   if(num < 2 )
   {	
		re = globalSum;
	   return globalSum;

   }
   else
   {
		globalSum = globalSum * (num);
		num-=1;
		Factorial(num);
   }// end of else
}
int Ci902View::GetCombination(int *n_Start, int *n_End,int *r_Start, int *r_End)
{
  bool markIt=false;
  int* recordR;

  int* nDecrement=n_End;
  nDecrement--;

  int *copyREnd = r_End;
  copyREnd--;
  int *copyRStart = r_Start;
  copyRStart--;

  for(int* rDecrement=copyREnd; rDecrement!=copyRStart ; rDecrement--,nDecrement--)
  {
    if(*rDecrement==*nDecrement )
    {
      if(rDecrement!=r_Start) //to ensure not at the start of r sequence
      {
        markIt=true;
        recordR=(--rDecrement);
        ++rDecrement;
        continue;
      }
      else
        return 0;

    }
    else
    {

      if(markIt==true)
      {
        int *recordN;
        for (int* nDecrement2=n_Start;nDecrement2!=n_End;++nDecrement2)
          if(*recordR==*nDecrement2)
		  		{
            recordN=nDecrement2;
            break;
            }
        int *nDecrement3=++recordN;

        for  (int* rDecrement2=recordR;rDecrement2!=r_End;++rDecrement2,++nDecrement3)
        {
          *rDecrement2=*nDecrement3;
        }
        return 1;
      }

      for(int* nDecrement4=n_Start; nDecrement4!=n_End; ++nDecrement4)
        if(*rDecrement==*nDecrement4)
        {
          *rDecrement=*(++nDecrement4);
          return 1;
        }
    }// end of else

  }// end of for

return 1;	
	
}

void Ci902View::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
delete []graphMatrix;
clearPressed=1;
CDC *pDC = GetDC();
CBrush paintBackground(RGB(212,208,200));
pDC->SelectObject(&paintBackground);
pDC->Rectangle(10,167,1000,700);

	for(int i=0;i<1000;i++)
	{
		//m_ListL.De`
		m_ListL.DeleteString(0);
		m_ListM.DeleteString(0);
	}	

m_ProgressBar.SetPos(0);
UpdateData(TRUE);
ReleaseDC(pDC);

}// end of function

void Ci902View::DrawCliques(void)
{

GetUserDensityClique();

CDC *pDC= GetDC();
CBrush brushL(RGB(100,255,0));
CBrush brushM(RGB(250,25,200));
CBrush uBrushL(RGB(255,255,0));
CBrush uBrushM(RGB(10,200,240));
int uMark=0;
CString str;
CRect rect;

pDC->SelectObject(&brushL);

rect.top =330;//310//150;
rect.bottom =360;//340//180;
rect.left =30;
rect.right =60;

for(int i=0;i<indexL;i++)
{	
	int num=topL[i];
	if(num!=-1)
	{
		uMark=0;
		for(int q=0;q<uIndexL;q++)
			if(uTopL[q]==num)
				{uMark=1;}
		
		if(uMark==1)
			pDC->SelectObject(&uBrushL);
		else
			pDC->SelectObject(&brushL);

		pDC->Ellipse(&rect);
		str.Format("%d",num+1);
		pDC->TextOut(rect.left+10,rect.top+8,str);
	}
	rect.left+=35;
	rect.right+=35;
}

rect.top =430;//430 250;
rect.bottom =460;//440 280;
rect.left =30;
rect.right =60;
pDC->SelectObject(&brushM);

uMark=0;
for( i=0;i<indexM;i++)
{	
	int num=bottomM[i];
	if(num!=-1)
	{
		uMark=0;
		for(int q=0;q<uIndexM;q++)
			if(uBottomM[q]==num)
				{uMark=1;}
		
		if(uMark==1)
			pDC->SelectObject(&uBrushM);
		else
			pDC->SelectObject(&brushM);


		pDC->Ellipse(&rect);
		str.Format("%d",num+1);
		pDC->TextOut(rect.left+10,rect.top+8,str);
	}
	rect.left+=35;
	rect.right+=35;
}


// now draw the lines.
int x=30;
int y=330; //310 150;
int startIndexM=0,startIndexL=0;
int internalCount=0,offset=0;
int totalInternalCount=0;

for(int l=startIndexL;l<indexL;l++)
{	
	if(topL[l]==-1)
	{
		totalInternalCount+=internalCount+1;
		startIndexM=totalInternalCount;
		startIndexL=l+1;
		internalCount=0;		
		continue;
	}
	internalCount=0;
	for(int m=startIndexM;m<indexM;m++)
	{
		 // draw an edge between them.
			if(bottomM[m]!=-1)
			{
				x=(l*35)+30+15;
				y=360; //340 // bottom of upper circle
				pDC->MoveTo(x,y);
				x=(m*35)+30+15;
				y=430; //410// top of lower circle
				pDC->LineTo(x,y);
				internalCount++;
			}
			else
				break;
	offset=0;
	}
}

DeleteObject(brushL);
DeleteObject(brushM);

ReleaseDC(pDC);

}// end of function

void Ci902View::HighlightClique()
{

// now draw the lines.
srand(time(NULL));
int x=30;
int y=170;//150
//CPen whitePen(PS_SOLID,1,RGB(138,17,238));
CPen blackPen(PS_SOLID,1,RGB(0,0,0));
CPen randomPen;
int internalCounter=0,startM=0;


CDC *pDC =GetDC();
randomPen.CreatePen(PS_DASHDOTDOT,1,RGB(138,17,238));
pDC->SelectObject(randomPen);

		for(int j=0;j<indexL;j++)
		{
			if(topL[j]==-1)
			{
				randomPen.DeleteObject();
				randomPen.CreatePen(PS_DASHDOTDOT,1,RGB(100+rand()%155,100+rand()%155,100+rand()%155));
				pDC->SelectObject(randomPen);
				startM+=internalCounter+1;
				internalCounter=0;
				continue;
			}
			internalCounter=0;
			for(int k=startM;k<indexM;k++)
			{
				if(bottomM[k]!=-1)
				{ // change the color of the edge between them.
					x=(topL[j]*35)+30+15;
					y=200; //180 // bottom of upper circle
					pDC->MoveTo(x,y);
					x=(bottomM[k]*35)+30+15;
					y=270; //250// top of lower circle
					pDC->LineTo(x,y);
					internalCounter++;
				}
				else
					break;
			}
		}


DeleteObject(randomPen);
pDC->SelectObject(blackPen);
DeleteObject(blackPen);
ReleaseDC(pDC);
}

void Ci902View::GetUserDensityClique()
{

int verticesInThisClique=0;
int thisCliqueDensity=0,maxEdgesAllowed=0,thisCliqueEdges=0;
int totalIndex=0;

for(int i=0;i<indexL;i++)
{

	if(topL[i]==-1)
	{
		thisCliqueEdges=verticesInThisClique*verticesInThisClique;
		maxEdgesAllowed = m_NoOfVeriticesL*m_NoOfVeriticesM;

		thisCliqueDensity=(thisCliqueEdges/(float)maxEdgesAllowed)*100;

		if(thisCliqueDensity >= m_CliqueDensity)
		{	// record the value of clique vertices

			for(int j=totalIndex;j<(totalIndex+verticesInThisClique);j++)
				uTopL[uIndexL++]=topL[j];

			for(int v=totalIndex;v<(totalIndex+verticesInThisClique);v++)
				uBottomM[uIndexM++]=bottomM[v];

			uTopL[uIndexL++] = -1;
			uBottomM[uIndexM++] = -1;

		}
		totalIndex+=verticesInThisClique+1;
		verticesInThisClique=0;
	}
	else
	{
		verticesInThisClique++;
	}
}


}

void Ci902View::OnBnClickedButtondensitytimegraph()
{
	// TODO: Add your control notification handler code here
	
	CDC *pDC = GetDC();
	CBrush paintBackground(RGB(212,208,200));
	pDC->SelectObject(&paintBackground);
	pDC->Rectangle(10,167,1000,700);
	ReleaseDC(pDC);

	CRect Rect;

	// place where chart will be displayed
	Rect.top=170;
	Rect.left=10;
	Rect.right=405;
	Rect.bottom=410;

	// setting properties chart 
	m_Chart2d.SetChartTitle("Density Time Graph");
	m_Chart2d.SetChartLabel("Density(%)","Time(ms)");
	m_Chart2d.SetGridXYNumber(5,2);
	m_Chart2d.SetAxisStyle(1);
	m_Chart2d.SetRange(0,100,0,2000);
	m_Chart2d.SetAxisStyle(0);
	m_Chart2d.mpSerie[0].m_plotColor = RGB(255,0,0);
	m_Chart2d.mpSerie[1].m_plotColor = RGB(255,255,255);

	m_Chart2d.m_BGColor = RGB(0,255,0);
	m_Chart2d.SetGridXYNumber(10,10);

	// Create chart .
	m_Chart2d.Create(WS_CHILD|WS_VISIBLE,Rect,this,IDC_CHART2D);


	// plot the recorded values
	m_Chart2d.nSerieCount=1;

	if ( !m_Chart2d.AllocSerie(10/*gIndex*/) ) 
	{
		AfxMessageBox("Error allocating chart series") ;
        return ;
	}

	m_Chart2d.SetXYValue(0,0,0,0);

	for(int i=0;i<gIndex;i++)
	{	
		m_Chart2d.SetXYValue(gDensity[i],(double)gTime[i],i+1,0);
	}



	m_Chart2d.Invalidate();



}

void Ci902View::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here

m_Chart2d.DestroyWindow();
Invalidate(TRUE);

}

void Ci902View::SetProgressBar()
{
	progressing+=2;
	m_ProgressBar.SetPos(progressing);
	UpdateData(TRUE);
	;
	;
	;
	;
	;
	;
	;
	;
	;
	;
}// end of function



void Ci902View::UnEqualSizeClique()
{
	int combSum=0,combIndex=0;
	int * dataSetN;
	int * dataSetR;

//******************* Declaration for L ************************//
	int ***totalCombL;
	int * saveRL; // numbers in each combinations
	int *saveCombIndexL;

	dataSetN = new int[m_NoOfVeriticesL];
	saveRL = new int[m_NoOfVeriticesL-1];
	saveCombIndexL = new int[m_NoOfVeriticesL-1];

	for(int k=0;k<m_NoOfVeriticesL;k++)
		dataSetN[k]=k;


	totalCombL = new int**[m_NoOfVeriticesL-1];

	SetProgressBar();
//******************* Declaration for L ************************//



	for(int r=m_NoOfVeriticesL-1,int wL=0;r>0;r--,wL++)
	{
		combIndex=0;
		combSum =  ( (BIG) GetFactorial(m_NoOfVeriticesL) )/( (BIG) GetFactorial(r)*GetFactorial(m_NoOfVeriticesL-r) )  ;
		if(combSum==0)
			combSum=1;
	
		totalCombL[wL] = new int*[combSum];
		for(int j=0;j<combSum;j++)
			totalCombL[wL][j] = new int [r];
	
		saveRL[wL]=r;	
	// we know the number of entries in each combination which is equal to r here.
	// so fill the array with required number of integers and send to get the combinations
		
		dataSetR = new int [r];

		for(int k=0;k<r;k++)
			dataSetR[k]=k;

		do
		{
			// here we'll process each combination or record it for future use.
			for(int h=0;h<r;h++)
				totalCombL[wL][combIndex][h] = dataSetR[h];
			
			combIndex++; // available combinations with r objects
		} 
		while(GetCombination(dataSetN,(dataSetN + (m_NoOfVeriticesL)),dataSetR ,(dataSetR + r)));

		saveCombIndexL[wL]=combIndex;
		delete []dataSetR;

	}// end of outer most for loop on L


	SetProgressBar();
//******************* Declaration for M ************************//
	int ***totalCombM;
	int * saveRM; // numbers in each combinations
	int *saveCombIndexM;

	delete [] dataSetN;
	dataSetN = new int[m_NoOfVeriticesM];
	saveRM = new int[m_NoOfVeriticesM-1];
	saveCombIndexM = new int[m_NoOfVeriticesM-1];

	for( k=0;k<m_NoOfVeriticesM;k++)
		dataSetN[k]=k;


	totalCombM = new int**[m_NoOfVeriticesM-1];


//******************* Declaration for M ************************//

int wM;
	for( r=m_NoOfVeriticesM-1, wM=0;r>0;r--,wM++)
	{
		combIndex=0;
		combSum =  ( (BIG) GetFactorial(m_NoOfVeriticesM) )/( (BIG) GetFactorial(r)*GetFactorial(m_NoOfVeriticesM-r) )  ;
		if(combSum==0)
			combSum=1;
	
		totalCombM[wM] = new int*[combSum];
		for(int j=0;j<combSum;j++)
			totalCombM[wM][j] = new int [r];
	
		saveRM[wM]=r;	
	// we know the number of entries in each combination which is equal to r here.
	// so fill the array with required number of integers and send to get the combinations
		
		dataSetR = new int [r];

		for(int k=0;k<r;k++)
			dataSetR[k]=k;

		do
		{
			// here we'll process each combination or record it for future use.
			for(int h=0;h<r;h++)
				totalCombM[wM][combIndex][h] = dataSetR[h];
			
			combIndex++; // available combinations with r objects
		} 
		while(GetCombination(dataSetN,(dataSetN + (m_NoOfVeriticesM)),dataSetR ,(dataSetR + r)));

		saveCombIndexM[wM]=combIndex;
	
		delete []dataSetR;
	
	}// end of outer most for loop on M

	SetProgressBar();
//******************* Comparison Algorithm ***********************//
int combIndexM=0;
int rL,rM;
int invalidL,invalidM;
int onesCounter=0;
int redEntry=0;
CString str;

	for(int i=0;i<m_NoOfVeriticesL-1;i++)
	{
		SetProgressBar();
		combIndex = saveCombIndexL[i];
		for(int m=0;m<combIndex;m++)
		{
			rL=saveRL[i];

			invalidL=0;
			for(int g=0;g<rL;g++)
				for(int u=0;u<indexL;u++)
				{	
					if(topL[u]==totalCombL[i][m][g])
							invalidM=1;
				}
			if(invalidL==1)
				continue;



			for(int j=0;j<m_NoOfVeriticesM-1;j++)
			{
				combIndexM = saveCombIndexM[j];
				rM=saveRM[j];
				for(int n=0;n<combIndexM;n++)
				{
					// here we'll perform the comparison
					onesCounter=0;
					
					invalidM=0;
					for(int g=0;g<rM;g++)
						for(int u=0;u<indexM;u++)
						{	
							if(bottomM[u]==totalCombM[j][n][g])//totalComb[c][g])
								invalidM=1;
						}
					if(invalidM==1)
						continue;


					if(redEntry==1)
					{
						invalidL=0;
						for(int g=0;g<rL;g++)
						{	
							for(int u=0;u<indexL;u++)
							{	
								if(topL[u]==totalCombL[i][m][g])
									invalidL=1;
							}
						}
						redEntry=0;
						if(invalidL==1)
							break;

					}



					for(int e=0;e<rL;e++)
					{
						for(int f=0;f<rM;f++)
						{
							if(graphMatrix[ totalCombL[i][m][e] ][ totalCombM[j][n][f] ]==1 )
								onesCounter++;
						}

					}// end of inner for loop on e

					if(onesCounter==rL*rM)
					{	// clique found so record its entries
						///////////////////////////////////////
						invalidL=0;
						for(int g=0;g<rL;g++)
						{	
							for(int u=0;u<indexL;u++)
							{	
								if(topL[u]==totalCombL[i][m][g])
									invalidL=1;
							}
						}
						if(invalidL==1)
							continue;

						///////////////////////////////////////
						str.Format("######");
						m_ListM.AddString(str);
						m_ListL.AddString(str);
						for(int h=0;h<rL;h++)
						{	
							topL[indexL++]=totalCombL[i][m][h];
							str.Format("%d",totalCombL[i][m][h]+1); // row, L, top
							m_ListL.AddString(str);
						}

						for( h=0;h<rM;h++)
						{	
							bottomM[indexM++]=totalCombM[j][n][h]; // record the vertices involved in the clique
							str.Format("%d",totalCombM[j][n][h]+1); // column, M, bottom
							m_ListM.AddString(str);
						}

						bottomM[indexM++]=-1; 
						topL[indexL++]=-1;
						redEntry=1;

					}// end of if on clique found

				}

			}
		}

	}// end of outer for loop


SetProgressBar();
//delete []totalCombM;
}// end of function

void Ci902View::DrawCliquesUnEqual()
{

CDC *pDC= GetDC();
CBrush brushL(RGB(100,255,0));
CBrush brushM(RGB(250,25,200));
CBrush uBrushL(RGB(255,255,0));
CBrush uBrushM(RGB(10,200,240));
CString str;
CRect rect;

pDC->SelectObject(&brushL);

rect.top =330;//310//150;
rect.bottom =360;//340//180;
rect.left =30;
rect.right =60;

for(int i=0;i<indexL;i++)
{	
	int num=topL[i];
	if(num!=-1)
	{
		pDC->SelectObject(&uBrushL);
		pDC->Ellipse(&rect);
		str.Format("%d",num+1);
		pDC->TextOut(rect.left+10,rect.top+8,str);
	}
	rect.left+=35;
	rect.right+=35;
}

rect.top =430;//430 250;
rect.bottom =460;//440 280;
rect.left =30;
rect.right =60;
pDC->SelectObject(&brushM);

for( i=0;i<indexM;i++)
{	
	int num=bottomM[i];
	if(num!=-1)
	{
		pDC->SelectObject(&uBrushM);
		pDC->Ellipse(&rect);
		str.Format("%d",num+1);
		pDC->TextOut(rect.left+10,rect.top+8,str);
	}
	rect.left+=35;
	rect.right+=35;
}


// now draw the lines.
int x=30;
int y=330; //310 150;
int startIndexM=0,startIndexL=0;
int internalCount=0,offset=0;
int totalInternalCount=0;

for(int l=startIndexL;l<indexL;l++)
{	
	if(topL[l]==-1)
	{
		totalInternalCount+=internalCount+1;
		startIndexM=totalInternalCount;
		startIndexL=l+1;
		internalCount=0;		
		continue;
	}
	internalCount=0;
	for(int m=startIndexM;m<indexM;m++)
	{
		 // draw an edge between them.
			if(bottomM[m]!=-1)
			{
				x=(l*35)+30+15;
				y=360; //340 // bottom of upper circle
				pDC->MoveTo(x,y);
				x=(m*35)+30+15;
				y=430; //410// top of lower circle
				pDC->LineTo(x,y);
				internalCount++;
			}
			else
				break;
	offset=0;
	}
}

DeleteObject(brushL);
DeleteObject(brushM);

ReleaseDC(pDC);


}// end of function

void Ci902View::OnBnClickedUnequalclique()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	progressing=0;
	m_ProgressBar.SetRange(0,100);
	
	// capture the system time at the start of clique finding
	unEqualGraph=1;
	redrawCounter=1;
	clearPressed=0;
	indexL=0;indexM=0;uIndexL=0;uIndexM=0;
	// get the data from the interface
	UpdateData(TRUE);
	// create the matrix based on user input L*M
	// where L = Set1 & M = Set2
	graphMatrix = new int*[m_NoOfVeriticesL];
	
	for(int i=0;i<m_NoOfVeriticesL;i++)
		graphMatrix[i]=new int[m_NoOfVeriticesM];

	for( i=0;i<m_NoOfVeriticesL;i++)
		for(int j=0;j<m_NoOfVeriticesM;j++)
			graphMatrix[i][j]=0;
	// compute the number of edges allowed
	// L * M = max number of edges 
	// M is the columns in matrix and L is  row
	maxEdgesAllowed = m_NoOfVeriticesL*m_NoOfVeriticesM;

	// formula to find the no of edges present in the graph
	noOfEdgesPresent=maxEdgesAllowed*((float)m_DensityOfGraph/100);
	
	int dec = noOfEdgesPresent;
	int index=0,row,column;
	
	srand((unsigned)time(NULL));

	while(dec!=0)
	{
		index = rand()%maxEdgesAllowed;
		row = (index/m_NoOfVeriticesM);
		column = (index % m_NoOfVeriticesM);//M/// -1 ;
		if(graphMatrix[row][column]==0) //&& row >-1 && column >-1 && row < m_NoOfVeriticesM && column < m_NoOfVeriticesL)
		{
			graphMatrix[row][column] = 1;
			dec--;
		}
	}

	SetProgressBar();
	DrawGraph();


	if(m_DensityOfGraph!=100)
		UnEqualSizeClique();		
	else
	{
		CString str;
		for(int i=0;i<m_NoOfVeriticesL;i++)
		{
			topL[indexL++]=i;
			str.Format("%d",i+1); // row, L, top
			m_ListL.AddString(str);
		}

		for(int j=0;j<m_NoOfVeriticesM;j++)
		{
			bottomM[indexM++]=j;
			str.Format("%d",j+1); // column, M, bottom
			m_ListM.AddString(str);
		}

	}// end of else
	
	
	
DrawCliquesUnEqual();
m_ProgressBar.SetPos(100);

}

void Ci902View::OnBnClickedTogglebutton()
{
	// TODO: Add your control notification handler code here
if(unEqualGraph==1)
	unEqualGraph=0;
else
	unEqualGraph=1;

}

void Ci902View::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
gIndex=0;

}
