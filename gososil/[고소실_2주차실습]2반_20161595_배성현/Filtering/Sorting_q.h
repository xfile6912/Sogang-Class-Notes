#include <stdlib.h>
#include <iostream>
#include <math.h>

using namespace std;

template <class Etype> void display(char* cs, Etype [], int);

template <class Etype>
inline void
Swap( Etype & A, Etype & B )
{
    Etype Tmp = A;
    A = B;
    B = Tmp;
}


//=====================QuickSort (by recursion)===================
// from Weiss 1995, page 289

// Quicksort: sort first N items in array A
// Etype: must have copy constructor, operator=, and operator<

static const int Cutoff = 20;

template <class Etype>
void
QuickSort( Etype A[ ], int Low, int High )
{
    //    if( Low + Cutoff > High )
    //        InsertionSort( &A[ Low ], High - Low + 1 );
    //    else
    //    {
    //Sort Low, Middle, High

    if (High==Low)   /* stop when there are less than or equal to 3 elements */
	return;				
    else if (High-Low+1 <= 3)
    {
	int Middle = ( Low + High ) / 2;
        if( A[ Middle ] < A[ Low ] )
            Swap( A[ Low ], A[ Middle ] );
        if( A[ High ] < A[ Low ] )
            Swap( A[ Low ], A[ High ] );
        if( A[ High ] < A[ Middle ] )
            Swap( A[ Middle ], A[ High ] );
	return;
    }
    else  // Sorting is performed in splitting (in partitioning, more precisely)
    {	    
	int Middle = ( Low + High ) / 2;
	
	if( A[ Middle ] < A[ Low ] )
	    Swap( A[ Low ], A[ Middle ] );
	if( A[ High ] < A[ Low ] )
	    Swap( A[ Low ], A[ High ] );
	if( A[ High ] < A[ Middle ] )
	    Swap( A[ Middle ], A[ High ] );
	
	// Place pivot at Position High-1
	Etype Pivot = A[ Middle ];

	Swap( A[ Middle ], A[ High-1] );	
	// Swap( A[ Middle ], A[ High] );	
	
	// Begin partitioning

	int i, j;
        for( i = Low, j = High - 1; ; )
        {
            while( A[ ++i ] < Pivot ) // Symantec may require { }
                ;                     // instead of ;
            while( Pivot < A[ --j ] ) // for both these
                ;                     // loops. It should not.
            if( i < j )
                Swap( A[ i ], A[ j ] );
            else
                break;
        }
	/*int i, j;
	i = -1;
	j = High;
	
	while (i < j) 
	{    
	    while( i < High && A[ ++i ] <= Pivot ) // Symantec may require
		;                     // { } instead of ;
	    while( Pivot < A[ --j ] ) // for both these
		;                     // loops. It should not.
	    if( i < j )
		Swap( A[ i ], A[ j ] );
	}
	// Restore pivot
	Swap( A[ i ], A[ High ] );
	*/

	Swap( A[ i ], A[ High - 1 ] );
	
        if ( i > Low+1 )           // with more than one element
	    QuickSort( A, Low, i - 1 );   // Sort small elements
	if ( i < High-1 )          // with more than one element         
	    QuickSort( A, i + 1, High );  // Sort large elements	
    }
}

template <class Etype>
void
QuickSort( Etype A[ ], int N )
{
    QuickSort( A, 0, N-1 );
}


//=====================QuickMedian===================
// Unsang Park, May 2003

// Search Nth element without full sorting

template <class Etype>
void QuickMedian( Etype A[ ], int Low, int High, int index )
{
    if (High==Low)  /* 1 element */
	return;				
    else if (High-Low==1) {   /* 2 elements */
		if( A[ High ] < A[ Low ] )
			Swap( A[ Low ], A[ High ] );
		return;
    }   
    else  
    {	    
		int Middle = ( Low + High ) / 2;
	
		if( A[ Middle ] < A[ Low ] )
			Swap( A[ Low ], A[ Middle ] );
		if( A[ High ] < A[ Low ] )
			Swap( A[ Low ], A[ High ] );
		if( A[ High ] < A[ Middle ] )
			Swap( A[ Middle ], A[ High ] );
	
		// Place pivot at Position High-1
		Etype Pivot = A[ Middle ];
		Swap( A[ Middle ], A[ High-1] );	
	
		// Begin partitioning
		int i, j;
			for( i = Low, j = High - 1; ; )
			{
				while( A[ ++i ] < Pivot ) // Symantec may require { }
					;                     // instead of ;
				while( Pivot < A[ --j ] ) // for both these
					;                     // loops. It should not.
				if( i < j )
					Swap( A[ i ], A[ j ] );
				else
					break;
			}
	
		Swap( A[ i ], A[ High - 1 ] );
		// End partitioning, 'i' is the index of current pivot value
	
		if (i==index) {
			return;
		}
	
		else if ( i > index ) {
			QuickMedian( A, Low, i - 1, index );   // Search higher parts
		}
	
		else {
			QuickMedian( A, i + 1, High, index );  // Search lower parts  
		}
    }
}



//void algorithm(int A[] , int N, int prog )
//{ int i, j, k, ncopy, actual;
//  char *who;
//
//  switch( prog ) {
//  case 1:  cout << "Algor: You chose option 1: InsertionSort" << endl;
//           InsertionSort( A, N);
//           break;
//  case 2:  cout << "Algor: You chose option 2: QuickSort" << endl;
//           QuickSort( A, N);
//           break;
////  case 3:  cout << "Algor: You chose option 3: MergeSort" << endl;
////           MergeSort( A, N);
////           break;            
//  case 4:  cout << "Algor: You chose option 4: HeapSort" << endl;
//           HeapSort(A,N);
//           break;
//  } // end switch
//}

//template <class Etype>
//void display(char* cs, Etype A[], int N)		
//{
//	cout << cs << ": ";
//	for(int i=0; i<N; i++) 
//		cout << A[i] << " ";
//	cout << endl;
//}
//
//template <class Etype>
//void MM_Remove(Etype A[], Etype B[], int N, int m)
//{   // Function for Moving Median operation
//    // Remove B from A, A is sorted and B is not
//    // Similar with set difference, but the output set size is N-m always (disregard from A[N-m-1] to A[N-1]
//    
//    int i, a_pos, b_pos;
//    a_pos = b_pos = 0;
//
//    i = 0;   
//   /* while ( i<N-m ) 
//    {
//	while ( a_pos<N && b_pos > m-1 )
//	    A[i++] = A[a_pos++];
//	
//	while ( (a_pos<N && A[a_pos] < B[b_pos]) ) {
//	    A[i++] = A[a_pos++];
//	}	    
//	while ( a_pos<N && b_pos < m && A[a_pos] > B[b_pos]) {///
//	    b_pos++;
//	}
//	while ( a_pos<N && A[a_pos] == B[b_pos]) {///
//	    a_pos++;
//	    b_pos++;
//	}	
//    }
//    */
//    while ( i<N-m ) 
//    {
//	while ( a_pos<N && (b_pos > m-1 || A[a_pos] < B[b_pos]) ) 
//	{
//	    	    
//	    A[i++] = A[a_pos++];
////	    cout << i << " ";
////	    cout << a_pos << " " << b_pos << endl;
//	}
//	
//	    
//	while ( a_pos<N && b_pos < m && A[a_pos] > B[b_pos]) {///
//	    b_pos++;
//	}
//	while ( a_pos<N && b_pos<m && A[a_pos] == B[b_pos]) {///
//	    a_pos++;
//	    b_pos++;
////	    cout << a_pos << " " << b_pos << endl;
//	}
//	//cout << endl << endl;
//	
//	//cout << a_pos << " " << b_pos << endl;
//	
//    }
//}
//	
//template <class Etype>
//void MM_Add(Etype A[], Etype B[], int N, int m)
//{   // Function for Moving Median operation
//    // Add B to A, A is sorted and B is not
//    // Output set size is N+m always
//    // Operate from the back
//    
//    int i, a_pos, b_pos;
//    a_pos = N-1;
//    b_pos = m-1;
//
//    i = N+m-1;   
//    while ( a_pos>-1 || b_pos>-1 ) 
//    {
//	
//	while ( a_pos > -1 && b_pos < 0 )
//	    A[i--] = A[a_pos--];
//	
//	while ( a_pos < 0 && b_pos > -1 )
//	    A[i--] = B[b_pos--];
//	
//	while ( (a_pos > -1 && i > -1 && A[a_pos] > B[b_pos]) ) {
//	    A[i--] = A[a_pos--];
//	}	    
//	while ( a_pos > -1 && b_pos > -1 && A[a_pos] < B[b_pos]) {///
//	    A[i--] = B[b_pos--];
//	}
//	while ( a_pos > -1 && A[a_pos] == B[b_pos]) {///
//	    A[i--] = A[a_pos--];
//	    A[i--] = B[b_pos--];
//	}	
//    } 
//}    
//	
	    
