/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Module Name:	gen/src/stack.c
*
*   Purpose:
*	A set of routines to manage a stack of pointers.
*	To use this as a "normal" stack, use stack_init, stack_push,
*	stack_pop and stack_free.  Stack_item will return the item on the
*	top of the stack.  To navigate the stack, the concept of current
*	item is used.  The routines stack_bottom, stack_down, stack_up,
*	stack_top are used to change the current item.  If a pop or a push
*	is executed while the current item is not the top of the stack,
*	then ALL items above the current item in the stack are deleted.
*
*   Routines:
*	pointer	stack_bottom	: Set current item to bottom.
*	pointer	stack_down	: Set current item one down in the stack.
*	void	stack_free	: Free all memory associated with stack.
*	STACK	*stack_init	: Initalize stack.
*	pointer	stack_item	: Return the current item.
*	pointer	stack_pop	: Delete current item and return one down.
*	int	stack_position	: Where is the current item in the stack?
*	pointer	stack_push	: Add a new item above the current item.
*	pointer	stack_top	: Set current item to top item.
*	pointer	stack_up	: Set current item one up in the stack.
*
*   Date		: Nov 8, 1993
*
*   SCCS data		: @(#)
*	Module Name	: stack.c
*	Version Number	: 1.3
*	Release Number	: 1
*	Last Updated	: 02/27/97
*
*   Programmer		: Severin Gaudet
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include "gen_types.h"
#include "gen_eptr.h"
#include "gen_stack.h"

#define	STACK_CHECK_NULL(p)		{if((p)==NULL) return(NULL);}


/*+
************************************************************************
*
*   Synopsis:
*	pointer	stack_bottom( stack )
*
*   Purpose:
*	Moves the current item pointer to the bottom of the stack.
*
*   Parameters:
*	STACK	*stack		: (mod)	Pointer to stack being affected.
*
*   Values Returned:
*	pointer	item		: Pointer to the bottom item on the stack.
*	pointer	NULL		: No items in stack.
*
************************************************************************
-*/

pointer	stack_bottom(STACK *stack)
{
    pointer	item;

    if ( eptrlen( (EPTR *) stack->s_eptr ) == 0 )
    {
	return( NULL );
    }
    stack->s_current = 0;
    item = eptritem( (EPTR *) stack->s_eptr, stack->s_current );
    return( item );
}

/*+
************************************************************************
*
*   Synopsis:
*	pointer	stack_down( stack )
*
*   Purpose:
*	Moves the current item pointer down one item in the stack.
*
*   Parameters:
*	STACK	*stack		: (mod)	Pointer to stack being affected.
*
*   Values Returned:
*	pointer	item		: Pointer to the previous item on the stack.
*	pointer	NULL		: Already at the bottom of the stack.
*
************************************************************************
-*/

pointer	stack_down(STACK *stack)
{
    pointer	item;

    if ( eptrlen( (EPTR *) stack->s_eptr ) == 0 || stack->s_current == 0 )
    {
	return( NULL );
    }
    stack->s_current--;
    item = eptritem( (EPTR *) stack->s_eptr, stack->s_current );
    return( item );
}

/*+
************************************************************************
*
*   Synopsis:
*	void	stack_free( stack )
*
*   Purpose:
*	Frees all items on the stack and then frees the stack structure
*	itself.
*
*   Parameters:
*	STACK	*stack		: (mod)	Pointer to stack being freed.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	stack_free(STACK *stack)
{
    eptr_free( (EPTR *) stack->s_eptr, stack->s_free );
    gen_free( stack );
}

/*+
************************************************************************
*
*   Synopsis:
*	STACK	*stack_init( num_entries, free_function )
*
*   Purpose:
*	Initializes the stack.
*
*   Parameters:
*	int	num_entries	: (in)	Initial number of entries.
*	pfi	free_function	: (in)	Function to free stack item.
*
*   Values Returned:
*	STACK	*stack		: Pointer to stack.
*	STACK	NULL		: Memory error.
*
************************************************************************
-*/

STACK	*stack_init(int num_entries, pfv free_function)
{
    STACK	*stack;

    STACK_CHECK_NULL( stack = (STACK *) gen_alloc( sizeof( STACK ) ) );
    STACK_CHECK_NULL( stack->s_eptr = (pointer) eptr_alloc( num_entries ) );
    stack->s_current = 0;
    stack->s_free = free_function;
    return( stack );
}

/*+
************************************************************************
*
*   Synopsis:
*	pointer	stack_item( stack )
*
*   Purpose:
*	Returns a pointer to the item at the current item pointer position.
*
*   Parameters:
*	STACK	*stack		: (in)	Pointer to stack being affected.
*
*   Values Returned:
*	pointer	item		: Pointer to the current item on the stack.
*	pointer	NULL		: Stack is empty.
*
************************************************************************
-*/

pointer	stack_item(STACK *stack)
{
    pointer	item;

    item = eptritem( (EPTR *) stack->s_eptr, stack->s_current );
    return( item );
}

/*+
************************************************************************
*
*   Synopsis:
*	pointer	stack_pop( stack )
*
*   Purpose:
*	Removes all items on the stack above the current item pointer.
*	It then deletes the current item and returns the pointer to
*	the item below on the stack.
*
*   Parameters:
*	STACK	*stack		: (mod)	Pointer to stack being affected.
*
*   Values Returned:
*	pointer	item		: Pointer to item below current item
*	pointer	NULL		: Stack is empty.
*
************************************************************************
-*/

pointer	stack_pop(STACK *stack)
{
    pointer	item;
    int		num_deletions;

    if ( eptrlen( (EPTR *) stack->s_eptr ) == 0 )
    {
	return( NULL );
    }

    num_deletions = eptrlen( (EPTR *) stack->s_eptr ) - stack->s_current;
    eptr_del( (EPTR *) stack->s_eptr, stack->s_current,
	    num_deletions, stack->s_free );

    if ( stack->s_current == 0 )
    {
	return( NULL );
    }

    stack->s_current--;
    item = eptritem( (EPTR *) stack->s_eptr, stack->s_current );
    return( item );
}

/*+
************************************************************************
*
*   Synopsis:
*	int	stack_position( stack )
*
*   Purpose:
*	Determines whether the current stack position is at the top,
*	the bottom or somewhere in the middle.  Note that a single
*	item in the stack will return STACK_POS_SINGLE.
*
*   Parameters:
*	STACK	*stack		: (in)	Pointer to stack being checked.
*
*   Values Returned:
*	int	STACK_POS_BOTTOM	: At bottom of stack.
*	int	STACK_POS_EMPTY		: No items in stack.
*	int	STACK_POS_MIDDLE	: Somewhere in the middle.
*	int	STACK_POS_SINGLE	: Only one item in the stack.
*	int	STACK_POS_TOP		: At top of stack.
*
************************************************************************
-*/

int	stack_position(STACK *stack)
{
    int		position;

    if ( eptrlen( (EPTR *) stack->s_eptr ) == 0 )
    {
	position = STACK_POS_EMPTY;
    }
    else if ( eptrlen( (EPTR *) stack->s_eptr ) == 1 )
    {
	position = STACK_POS_SINGLE;
    }
    else if ( stack->s_current == 0 )
    {
	position = STACK_POS_BOTTOM;
    }
    else if ( eptrlen( (EPTR *) stack->s_eptr ) == stack->s_current + 1 )
    {
	position = STACK_POS_TOP;
    }
    else
    {
	position = STACK_POS_MIDDLE;
    }

    return( position );
}

/*+
************************************************************************
*
*   Synopsis:
*	pointer	stack_push( stack, item )
*
*   Purpose:
*	Inserts the given item on the top of the stack.  The stack is
*	expanded if necessary.  The current item pointer is set to
*	this new item.  If the current item pointer is not at the top
*	of the stack, then all items above the current item are deleted
*	and the new item added.
*
*   Parameters:
*	STACK	*stack		: (mod)	Pointer to stack being affected.
*	pointer	item		: (in)	Pointer to item being added.
*
*   Values Returned:
*	pointer	item		: Pointer to the top item on the stack.
*	pointer	NULL		: Memory error.
*
************************************************************************
-*/

pointer	stack_push(STACK *stack, pointer item)
{
    int		position;
    int		num_deletions;

    if ( eptrlen( (EPTR *) stack->s_eptr ) == 0 )
    {
	position = 0;
    }
    else if ( stack->s_current == 0 ||
	    stack->s_current < eptrlen( (EPTR *) stack->s_eptr ) - 1 )
    {
	position = stack->s_current + 1;
	num_deletions = eptrlen( (EPTR *) stack->s_eptr ) - position;
	eptr_del( (EPTR *) stack->s_eptr, position,
		num_deletions, stack->s_free );
    }
    else
    {
	position = eptrlen( (EPTR *) stack->s_eptr );
    }

    STACK_CHECK_NULL( eptr_insert( (EPTR *) stack->s_eptr, position, item ) );
    stack->s_current = position;
    return( item );
}

/*+
************************************************************************
*
*   Synopsis:
*	pointer	stack_top( stack )
*
*   Purpose:
*	Moves the current item pointer to the top of the stack.
*
*   Parameters:
*	STACK	*stack		: (mod)	Pointer to stack being affected.
*
*   Values Returned:
*	pointer	item		: Pointer to the top item on the stack.
*	pointer	NULL		: No items in stack.
*
************************************************************************
-*/

pointer	stack_top(STACK *stack)
{
    pointer	item;

    if ( eptrlen( (EPTR *) stack->s_eptr ) == 0 )
    {
	return( NULL );
    }
    stack->s_current = eptrlen( (EPTR *) stack->s_eptr ) - 1;
    item = eptritem( (EPTR *) stack->s_eptr, stack->s_current );
    return( item );
}

/*+
************************************************************************
*
*   Synopsis:
*	pointer	stack_up( stack )
*
*   Purpose:
*	Moves the current item pointer up one item in the stack.
*
*   Parameters:
*	STACK	*stack		: (mod)	Pointer to stack being affected.
*
*   Values Returned:
*	pointer	item		: Pointer to the next item up on the stack.
*	pointer	NULL		: Already at the top of the stack.
*
************************************************************************
-*/

pointer	stack_up(STACK *stack)
{
    pointer	item;

    if ( eptrlen( (EPTR *) stack->s_eptr ) == 0 ||
	    eptrlen( (EPTR *) stack->s_eptr ) == stack->s_current + 1 )
    {
	return( NULL );
    }
    stack->s_current++;
    item = eptritem( (EPTR *) stack->s_eptr, stack->s_current );
    return( item );
}
#undef STACK_CHECK_NULL
