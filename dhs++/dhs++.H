//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1997				(c) 1997
// National Research Council		Conseil national de recherches
// Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
// All rights reserved			Tous droits reserves
// 					
// NRC disclaims any warranties,	Le CNRC denie toute garantie
// expressed, implied, or statu-	enoncee, implicite ou legale,
// tory, of any kind with respect	de quelque nature que se soit,
// to the software, including		concernant le logiciel, y com-
// without limitation any war-		pris sans restriction toute
// ranty of merchantability or		garantie de valeur marchande
// fitness for a particular pur-	ou de pertinence pour un usage
// pose.  NRC shall not be liable	particulier.  Le CNRC ne
// in any event for any damages,	pourra en aucun cas etre tenu
// whether direct or indirect,		responsable de tout dommage,
// special or general, consequen-	direct ou indirect, particul-
// tial or incidental, arising		ier ou general, accessoire ou
// from the use of the software.	fortuit, resultant de l'utili-
// 					sation du logiciel.
//
//***********************************************************************
//
// FILENAME
// dhs++/dhs++.H
//
// PURPOSE:
// Main interface routine to the DHS C++ library.
//
// CLASSES:
// dhs - Main class for the dhs library.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.8  2001/03/12 20:50:46  nhill
// Changed to work with workshop 5.0 compilers.
//
// Revision 1.7  1998/04/23 22:32:04  nhill
// Changed to include dhsGenStatus.H.
//
// Revision 1.6  1998/04/02 19:33:33  nhill
// Removed fil;e dhs++Command.H from those included.
//
// Revision 1.5  1997/08/19 19:57:15  nhill
// Added stdlib.h to the files included.
//
// Revision 1.4  1997/04/15 20:06:35  nhill
// Added undefs so the selective include stuff can be used when the dhs++
// header is included more than once.
//
// Revision 1.3  1997/03/06 19:16:30  nhill
// Cleaned up the header file dependancies.
//
// Revision 1.2  1997/02/28 21:16:55  nhill
// Changed the order of the stl includes.
//
// Revision 1.1  1997/02/21 21:46:47  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <string.h>
#include <stdlib.h>

#include <pthread.h>

#include "dhsGenStatus.H"
extern "C"
{
#include "dhs.h"
}

#ifdef STD_NAMESPACE
using namespace STD_NAMESPACE;
#endif // STD_NAMESPACE

#include "dhs++Dhs.H"

#ifndef	DHS_NO_CLIENT
#include "dhs++Client.H"
#ifndef	DHS_NO_COMMAND
#include "dhs++ClientCommand.H"
#endif	// DHS_NO_COMMAND
#endif	// DHS_NO_CLIENT

#ifndef DHS_NO_SERVER
#ifndef DHS_NO_COMMAND
#include "dhs++ServerCommand.H"
#endif	// DHS_NO_COMMAND
#endif	// DHS_NO_SERVER


#ifndef DHS_NO_BULK
#include "dhs++Dataset.H"

#ifndef DHS_NO_SERVER
#ifndef DHS_NO_PUT
#include "dhs++PutRequest.H"
#endif	// DHS_NO_PUT

#ifndef DHS_NO_GET
#include "dhs++GetRequest.H"
#endif	// DHS_NO_GET
#endif	// DHS_NO_SERVER

#ifndef DHS_NO_CLIENT
#include "dhs++ClientDataset.H"

#ifndef DHS_NO_PUT
#include "dhs++PutDataset.H"
#endif	// DHS_NO_PUT

#ifndef DHS_NO_GET
#include "dhs++GetDataset.H"
#endif	// DHS_NO_GET
#endif	// DHS_NO_CLIENT
#endif  // DHS_NO_BULK

#undef DHS_NO_CLIENT
#undef DHS_NO_COMMAND
#undef DHS_NO_BULK
#undef DHS_NO_CLIENT
#undef DHS_NO_COMMAND
#undef DHS_NO_GET
#undef DHS_NO_PUT
#undef DHS_NO_SERVER
