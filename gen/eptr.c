/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Module Name:	gen/src/eptr.c
*
*   Purpose:
*       This file contains routines which operate on the EPTR
*	structure defined in eptr.h.  All function passed to these
*	routines are expected to reference data structures via a 
*	pointer.
*
*   Routines:
*	EPTR		*eptr_alloc	: Allocate an EPTR structure.
*	void		*eptr_clear	: Oposit of eptr_init. Deletes
*					  all data in an EPTR.
*	void		eptr_del	: Free up n things pointed to by an 
*					  EPTR.
*	EPTR 		*eptr_fit	: Frees unused memory.
*	void		eptr_free	: Deallocates an EPTR structure.
*	EPTR		*eptr_insert	: Insert a pointer into pointer eptray.
*	EPTR		*eptr_init	: Allocate n eptray elements.
*	void		eptr_replace	: Replaces an element within a pointer 
*					  eptray.
*	boolean		eptr_search	: Binary search pointer eptray.
*	EPTR		*eptr_sort	: Sorts a pointer eptray. 
*
*   Date		: Dec 10, 1990
*
*   SCCS data		: @(#)
*	Module Name	: eptr.c
*	Version Number	: 1.12
*	Release Number	: 1
*	Last Updated	: 07/10/97
*
*   Programmer		: Severin Gaudet
*
*   Modification History:
*	91/07/09 BE	: Added eptr_fit().
*	92/05/22 SM	: Rewrote the whole lot.
*	95/08/23 PW	: Changes eptr_del, eptr_free, eptr_replace free 
*			  function from pfi to pfv.
*	97/07/03 nrh	: Added the eptr_clear function. Changed eptr_free
*			  to use eptr_clear.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include "gen_types.h"
#include "gen_eptr.h"

#define FM 	7875		/* FM, FA, FC are randomization 
				   variables, used in eptr_qsort.	*/
#define FA      211
#define FC 	1663
#define M 	7		/* Smallest list size to preform quick
				   algorithm.				*/

/*
 *  Macros.
 */

#define	EP_CHECK_NULL( e )	if (e==NULL) {return(NULL);}	


/*+
************************************************************************
*
*   Synopsis:
*	EPTR	*eptr_alloc( n )
*
*   Purpose:
*       Allocates an EPTR structure and also allocates sufficient storage
*       for n pointers in its pointer eptray.  A pointer to the newly
*       created structure is returned.
*
*   Parameters:
*	int	n		: (in)  Initial length of pointer eptray.
*
*   Values Returned:
*       EPTR	*e		: Pointer to newly allocated EPTR.
*       EPTR	NULL		: Memory allocation error.
*
************************************************************************
-*/

EPTR	*eptr_alloc(int n)
{
    EPTR *e;
    
    e = (EPTR *) gen_alloc( sizeof( EPTR ) );
    
    EP_CHECK_NULL( e );

    return( eptr_init( e, n ) );
}

/*+
************************************************************************
*
*   Synopsis:
*	void	eptr_clear( eptr, e_free )
*
*   Purpose:
*	To free all memory used by an eptr.
*
*   Parameters:
*	EPTR	*eptr		: (mod) A pointer to the EPTR to free.
*	pfv	e_free		: (in)	Function responsible for freeing 
*					pointer eptray allocated memory.
*
************************************************************************
-*/

void eptr_clear(EPTR *eptr, pfv e_free)
{
    pointer	*p;

    if ( e_free != NULL )
    {
	for ( p = eptr->e_pointer; *p; p++ )
	{
	    e_free( *p );
	}
    }

    gen_free( eptr->e_pointer );
    eptr->e_pointer = NULL;
}

/*+
************************************************************************
*
*   Synopsis:
*	void	eptr_del( eptr, start, n, e_free )
*
*   Purpose:
*       This routine free 'n' items that 'eptr's pointer eptray points
*       to, starting at position 'start'.  Note that this does NOT
*       deallocate any space in the pointer eptray itself, just the
*       items pointed to.
*
*   Parameters:
*       EPTR	*eptr		: (mod) The pointer eptray to free from.
*       int	start		: (in)  Index of the first thing to free.
*       int	n		: (in)  Number of things to free.
*	pfv	e_free		: (in)	Function which free's allocated
*					memory from the pointer eptray.
*
************************************************************************
-*/

void	eptr_del(EPTR *eptr, int start, int n, pfv e_free)
{
    int		i;
    pointer	*p;
    pointer	*s;

    s = p = eptr->e_pointer + start;
    
    if ( e_free != NULL )
    {
	for ( i = 0; i < n; ++i, ++p )
	{
	    e_free( *p );
	}
    }
    else
    {
	p += n;
    }
    
    for ( ; *p; ++p, ++s )
    {
        *s = *p;
    }
    *s = NULL;
    
    eptr->e_used -= n;
}

/*+
************************************************************************
*
*   Synopsis:
*	void	eptr_free( eptr, e_free )
*
*   Purpose:
*	To free all allocated memory referenced by eptr.
*
*   Parameters:
*	EPTR	*eptr		: (mod) A pointer to the EPTR to free.
*	pfv	e_free		: (in)	Function responsible for freeing 
*					pointer eptray allocated memory.
*
************************************************************************
-*/

void eptr_free(EPTR *eptr, pfv e_free)
{
    eptr_clear( eptr, e_free );
    gen_free( eptr );
}

/*+
************************************************************************
*
*   Synopsis:
*	EPTR	*eptr_fit( eptr )
*
*   Purpose:
*	To free any memory not required in the eptr.e_pointer eptray.
*
*   Parameters:
*	EPTR	*eptr		: (mod) A pointer to the EPTR to fit.
*
*   Values Returned:
*	EPTR	*eptr		: Successful completion.
*	EPTR	NULL		: Memory error.
*
************************************************************************
-*/

EPTR	*eptr_fit(EPTR *eptr)
{
    if ( ( eptr->e_used + 1 ) < eptr->e_allocd )
    {
	eptr->e_pointer = (pointer *) gen_realloc( eptr->e_pointer,
		( ( eptr->e_used + 1 ) * sizeof( pointer ) ) );

	EP_CHECK_NULL( eptr->e_pointer );
	
	eptr->e_allocd = eptr->e_used;
	eptr->e_pointer[eptr->e_allocd] = NULL;
    }

    return( eptr );
}

/*+
************************************************************************
*
*   Synopsis:
*	EPTR	*eptr_insert( eptr, index, ptr )
*
*   Purpose:
*	Inserts 'ptr' into the pointer eptray 'eptr' at position 'index' and
*	shifts all pointers from index to eptr->used right one position to
*	accomodate the insertion.
*
*   Parameters:
*	EPTR	*eptr		: (mod) Pointer eptray to insert into.
*	int	index		: (in)  Insertion position in eptray.
*	pointer ptr		: (in)  Pointer to be inserted.
*
*   Values Returned:
*	EPTR	*eptr		: Pointer inserted.
*	EPTR	NULL		: Memory allocation error.
*
************************************************************************
-*/

EPTR *eptr_insert(EPTR *eptr, int index, pointer ptr)
{
    int     i;
    pointer save;
    int     size;
    
#ifdef DEBUG
    if ( index < 0 || index > eptr->e_used )
    {
        (void) printf( "eptr_insert :  bad insertion position!\n" );
	abort();
    }
#endif    

    /*
     *  See if we need to expand the pointer eptray.
     */
     
    if ( eptr->e_used + 1 > eptr->e_allocd )
    {
	size = ( eptr->e_allocd + eptr->e_increment + 1 ) * sizeof( pointer );
        eptr->e_pointer = (pointer *) gen_realloc( eptr->e_pointer, size );
	EP_CHECK_NULL( eptr->e_pointer );
	eptr->e_allocd += eptr->e_increment;
    }
    
    /*
     *  Insert the pointer at position 'index' in the eptray.
     */
     
    for ( i = index; i <= eptr->e_used; ++i )
    {
	save = eptr->e_pointer[i];
	eptr->e_pointer[i] = ptr;
	ptr = save;
    }
    
    eptr->e_pointer[i] = NULL;
    ++eptr->e_used;
    
    return( eptr );
}

/*+
************************************************************************
*
*   Synopsis:
*	EPTR	*eptr_init( eptr, n )
*
*   Purpose:
*	Allocates a pointer eptray of sufficient length to hold n pointers
*	and stores with the EPTR 'eptr'.  Note that this does NOT allocate
*	an EPTR structure itself!
*
*   Parameters:
*	EPTR	*eptr		: (in) pointer to an EPTR structure.
*	int	n		: (in) number of elements requested.
*
*   Values Returned:
*	EPTR	*eptr		: success.
*	EPTR	NULL		: not enough memory.
*
************************************************************************
-*/

EPTR    *eptr_init(EPTR *eptr, int n)
{
    pointer *elements;
    
    elements = ptr_alloc( n+1 ) ;

    EP_CHECK_NULL( elements );

    eptr->e_pointer = elements;
    (eptr)->e_pointer[0] = NULL;
    eptr->e_used = 0;
    eptr->e_allocd = n;
    eptr->e_increment = n;

    return( eptr );    
}	    

/*+
************************************************************************
*
*   Synopsis:
*	void	eptr_replace( eptr, p, index, e_free )
*
*   Purpose:
*	Replace element index in the pointer eptray with memory pointed to 
*	by p.
*
*   Parameters:
*	EPTR	*eptr		: (mod)	Points to eptr eptray.
*	pointer	p		: (in)	Pointer to add to pointer eptray.
*	int	index		: (in)	Index in pointer eptray to pointer
*					that will be replaced.
*	pfv	e_free		: (in)	Function which frees memory pointed
*					to by the pointer eptray.
*
*
************************************************************************
-*/

void    eptr_replace(EPTR *eptr, pointer p, int index, pfv e_free)
{

#ifdef DEBUG
    if ( index < 0 || index > eptr->e_used )
    {
        (void) printf( "eptr_insert :  bad insertion position!\n" );
	abort();
    }
#endif    

    if ( e_free != NULL )
    {
	e_free( eptr->e_pointer[index] );
    }

    eptr->e_pointer[index] = p;
}

/*+
************************************************************************
*
*   Synopsis:
*	boolean	eptr_search( eptr, key, index, e_compare )
*
*   Purpose:
*	Performs a binary search on the eptray of pointers. 'key' is the value
*	searched for. 'e_compare' is a function which compares the data in
*	the eptray of pointers to the key value. 
*
*   Parameters:
*	EPTR_P  eptr		: (in)  Array of pointers to be searched.
*	pointer key		: (in)  Points to key value searching for.
*	int	*index		: (out)	Index to position key was found or
*					should be found at.
*	pfi     e_compare	: (in)  Function which can compare data of
*					type pointed to by eptr.
*
*   Values Returned:
*	boolean TRUE		: Search was successful.
*	boolean FALSE		: Key was not found in the eptray of pointers.
*
************************************************************************
-*/

boolean eptr_search(EPTR_P eptr, pointer key, int *index, pfi e_compare)
{
    int		cv;
    pointer	*end;
    pointer	*middle;
    pointer	*start;
    int		status;



    *index = 0;

    /*
     *  Check that there is a list to search.
     */

    if ( eptr->e_used == 0 )
    {
	return( FALSE );
    }


    /*
     *  Set pointers to the begining and end of the list.
     */

    start = eptr->e_pointer;
    end = start + (eptr->e_used - 1);


    /*
     *  If the key value is less than or equal to the first value,
     *  the search is unnecessary.
     */

    if ( ( cv = e_compare( key, *start ) ) < 0 )
    {
	return( FALSE );
    }
    if ( cv == 0 )
    {
	return( TRUE );
    }


    /*
     *  If the key value is greater than or equal to the last value,
     *  the search is unnecessary
     */

    if ( ( cv = e_compare( key, *end ) ) > 0 )
    {
	*index = eptr->e_used;
	return( FALSE );
    }
    if ( cv == 0 )
    {
	*index = eptr->e_used - 1;
	return( TRUE );
    }


    /*
     *  If the key value is in the eptray of pointers, middle will eventually 
     *  point to it and will be returned. if the key value is not in the list, 
     *  start and end will eventually point to adjacent elements in the list, 
     *  and when middle is calculated it will be the same as start. when the 
     *  key value is not in the list start+1 is returned as it points to the 
     *  next largerst element than key within the list.
     */

    /* CONSTANTCONDITION */
    while ( TRUE )
    {
	middle = start + ( ( end - start ) / 2 );
	if ( middle == start )
	{
	    *index = start - eptr->e_pointer + 1;
	    status = FALSE;
	    break;
	}
	if ( ( cv = e_compare( key, *middle ) ) < 0 )
	{
	    end = middle;
	}
	else if ( cv > 0 )
	{
	    start = middle;
	}
	else 
	{
	    *index = middle - eptr->e_pointer;
	    status = TRUE;
	    break;
	}
    }

    return( status );
}

/*+
************************************************************************
*
*   Synopsis:
*	EPTR 	*eptr_sort( eptr, e_compare )
*
*   Purpose:
*	Sort an array of pointers using an optimized version of quick sort
*	and a comparison function supplied by the application programme.  
*	e_compare is expected to take a pointer to the structure unlike
*	the C library version of quick sort, qsort, which expects the 
*	comparison function to reference the structure from a pointer to a
*	pointer.  eptr_sort will dynamicaly allocate the stack used in this
*	routine.  This will avoid having every programme link to the math 
*	library when using this library to enable eptr_sort to use the 
*	function log2.  The required stack size as stated in the reference 
*	text is (log2 n) * 2, where n is the size of the list to be sorted. 
*
*   Parameters:
*	EPTR    *eptr		: (mod) Pointer array to be sorted.
*	pfi     e_compare	: (in)  Fuction to evaluate two structures
*					for sorting purposes.
*   Values returned:
*	EPTR	*eptr		: Success.
*	EPTR	*NULL		: Memory allocation error.
*
*   References:
*	Press, Flanner, Teukolsky, Vetterling, Numerical Recipes in C,
*	p. 251.
*	also see /usr/local/share/numerical_recipes/c/qcksrt.c
*
************************************************************************
-*/

EPTR 	*eptr_sort(EPTR *eptr, pfi e_compare)
{
    pointer 	a;
    pointer	*e_pointer;
    long int 	fx;
    int		i;
    int		rand_index;
    int		current_right;
    int 	*istack;
    int 	current_left;
    int		j;
    int		jstack;
    int 	n;
    int		log2n;

    e_pointer = eptr->e_pointer;
    fx = 0L;
    jstack = 0;
    current_left = 1;
    n = eptrlen( eptr );
    current_right = n;

    if ( n < 2 )
    {
	return( eptr );
    }


    /*
     *  Calculating log2 n by determining the size of the binarray 
     *  representation of n.  This is done by counting the number of 
     *  shifts right if takes until n = 0.
     */

    log2n = 0;
    for ( j = n; j > 1; j >>= 1 )
    {
	log2n++;
    }

    istack = int_alloc( log2n * 2 );

    EP_CHECK_NULL( istack );

    for ( ; ; ) 
    {    

	if ( current_right - current_left < M ) 
	{
	    /*
	     *  Sort by straight insertion.
	     */

	    for ( j = current_left + 1; j <= current_right; j++ ) 
	    {
		a = e_pointer[j - 1];

		for (i = j-1; i > 0 && e_compare( e_pointer[i - 1], a ) > 0; 
			i--) 
		{
		    e_pointer[i] = e_pointer[i - 1];
		}

		e_pointer[i] = a;
	    }
	    if ( jstack == 0 ) 
	    {
		gen_free( istack );
		return( eptr );
	    }
	    current_right = istack[jstack--];
	    current_left = istack[jstack--];
	} 
	else 
	{
	    /*
	     *  Sort by optimized quick sort algorithm.
	     */

	    i = current_left;
	    j = current_right;


	    /*
	     *  Generating a random integer rand_index between current_left and 
	     *  current_right.
	     */

	    fx = ( fx * FA + FC ) % FM;
	    rand_index = current_left + 
		   ( ( current_right - current_left + 1 ) * fx ) / FM;

	    a = e_pointer[rand_index - 1];
	    e_pointer[rand_index - 1] = e_pointer[current_left - 1];

	    for ( ; ; ) 
	    {
		while ( j > 0 && e_compare( a, e_pointer[j - 1] ) < 0 )
		{
		    j--;
		}

		if ( j <= i ) 
		{
		    e_pointer[i - 1] = a;
		    break;
		}
		e_pointer[i - 1] = e_pointer[j - 1];
		i++;

		while ( i <= n && e_compare( a, e_pointer[i - 1] ) > 0 ) 
		{
		    i++;
		}

		if ( j <= i ) 
		{
		    i = j;
		    e_pointer[i - 1] = a;
		    break;
		}
		e_pointer[j - 1] = e_pointer[i - 1];
		j--;
	    }
	    if ( current_right - i >= i - current_left ) 
	    {
		istack[++jstack] = i + 1;
		istack[++jstack] = current_right;
		current_right = i - 1;
	    } 
	    else 
	    {
		istack[++jstack] = current_left;
		istack[++jstack] = i - 1;
		current_left = i + 1;
	    }
	}
    }
}
#undef M
#undef FM
#undef FA
#undef FC
