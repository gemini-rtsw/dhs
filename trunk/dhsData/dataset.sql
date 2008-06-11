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
* dhsData/sql/dataset.C
*
* PURPOSE:
* Create the dataset table.  This table contains persistant information
* about datasets whose lifetime is permanent.
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

print "Creating new dataset table"
go

Create table dataset
(
	datasetName	varchar(45)	not null,
	health		char		not null,
	format		char		not null,
	contributors	varchar(225)	not null
)
go

print "Creating indexes"
go

create unique index ds_primary on dataset
(
    datasetName
)
go

print "Granting"
go

grant insert, update, delete, select on dataset to operator
go

grant select on dataset to public
go
