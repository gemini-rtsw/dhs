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
* dhsData/sql/process.C
*
* PURPOSE:
* Create the process table.  This contains information about active
* datasets, both temporary and permanent.  Deletions are made from 
* this table when a file has been archived or deleted.
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

print "Creating new process table"
go

Create table process
(
	datasetName	varchar(45)	not null,
	uniqueName	varchar(45)	not null,
	dateReceived	datetime	not null,
	received	char		not null,
	lifetime	char		not null,
	health		char		not null,
	oldpNotify	char		not null,
	ssNotify	char		not null,
	format		char		not null,
	contributors	varchar(225)	not null
)
go

print "Creating indexes"
go

create unique index pr_primary on process
(
    datasetName
)
go

print "Granting"
go

grant insert, update, delete, select on process to operator
go

grant select on process to public
go
