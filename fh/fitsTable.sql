/*+
*
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1997				(c) 1997
* National Research Council		Conseil national de recherches
* Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
* All rights reserved			Tous droits reserves
* 					
* NRC disclaims any warranties,	Le CNRC denie toute garantie
* expressed, implied, or statu-	enoncee, implicite ou legale,
* tory, of any kind with respect	de quelque nature que se soit,
* to the software, including		concernant le logiciel, y com-
* without limitation any war-		pris sans restriction toute
* ranty of merchantability or		garantie de valeur marchande
* fitness for a particular pur-	ou de pertinence pour un usage
* pose.  NRC shall not be liable	particulier.  Le CNRC ne
* in any event for any damages,	pourra en aucun cas etre tenu
* whether direct or indirect,		responsable de tout dommage,
* special or general, consequen-	direct ou indirect, particul-
* tial or incidental, arising		ier ou general, accessoire ou
* from the use of the software.	fortuit, resultant de l'utili-
* 					sation du logiciel.
*
************************************************************************
*
* FILENAME
* fh/sql/fitsTable.C
*
* PURPOSE:
* Create the fits table.  This contains fits header information.
*
*
*INDENT-OFF*
*
*INDENT-ON*
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
*
-*/

print "Using database dhs"
use dhs
go

print "Creating new fitsTable table"
go

Create table fitsTable
(
	datasetName	varchar(45)	null,
	instrument	varchar(72)	null,
	origin		varchar(72)	null,
	type1		varchar(72)	null,
	CRPIX1		float 		null,
	contributors	varchar(225)	null
)
go
print "Granting"
go

grant insert, update, delete, select on fitsTable to operator
go

grant select on fitsTable to public
go
