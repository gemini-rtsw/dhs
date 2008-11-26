/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1998.				(c) 1998.
* National Research Council		Conseil national de recherches
* Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
* All rights reserved			Tous droits reserves
* 					
* NRC disclaims any warranties,		Le CNRC denie toute garantie
* expressed, implied, or statu-		enoncee, implicite ou legale,
* tory, of any kind with respect	de quelque nature que se soit,
* to the software, including		concernant le logiciel, y com-
* without limitation any war-		pris sans restriction toute
* ranty of merchantability or		garantie de valeur marchande
* fitness for a particular pur-		ou de pertinence pour un usage
* pose.  NRC shall not be liable	particulier.  Le CNRC ne
* in any event for any damages,		pourra en aucun cas etre tenu
* whether direct or indirect,		responsable de tout dommage,
* special or general, consequen-	direct ou indirect, particul-
* tial or incidental, arising		ier ou general, accessoire ou
* from the use of the software.		fortuit, resultant de l'utili-
* 					sation du logiciel.
*
************************************************************************
*
*   Name:		am/sql/mediaTypes.sql
*
*   Purpose:
*	This file contains the table definitions for
*	the mediaTypes table.
*
*   Database		: dhsTestDB
*
*   Date		: Novebmer 24, 1998
*
*   SCCS data		: @(#)
*	Module Name	: mediaTypes.sql
*	Version Number	: 1.2
*	Release Number	: 1
*
*   Programmer		: Jennifer Dunn
*
*   Modification History:
*	99/03/15 NRH  : Changed media_type to 10 characters. Added insertion
*			of mediatypes.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database dhsTestDB"
use dhsTestDB
go

print "Creating new mediaTypes table"
go
if exists ( select name from sysobjects 
		where name = 'mediaTypes' and type = 'U' )
		begin 
		    print "Dropping existing mediaTypes table"
		    drop table mediaTypes
		end
go


Create table mediaTypes
(
	media_type		char(10)	not null,
	capacity		int		not null,
	threshold		float		not null,
	splitSize		int		not null,
	blockSize		int		not null,
	fileOverhead		int		not null,
	directoryOverhead	int		not null,
	description		char(50)	not null
)
go

print "Creating indexes"
go

create unique index mt_primary on mediaTypes
(
    media_type
)
go


print "Granting"
go

#grant insert, update, delete, select on mediaTypes to operator
#go

grant select on mediaTypes to public
go

insert mediaTypes values ("CD", 665600, 98.5, 332800, 512, 4, 4, "CD-ROM" )
insert mediaTypes 
	values ("EXA-l-90", 2000000, 98.5, 1000000, 8192, 1, 1, "Low Density Exabyte, 90m tapes" )
insert mediaTypes 
	values ("EXA-m-90", 4000000, 98.5, 2000000, 8192, 1, 1, "Medium Density Exabyte, 90m tapes" )
insert mediaTypes 
	values ("EXA-h-90", 8000000, 98.5, 4000000, 8192, 1, 1, "High Density Exabyte, 90m tapes" )
insert mediaTypes 
	values ("EXA-l-120", 3500000, 98.5, 1750000, 8192, 1, 1, "Low Density Exabyte, 120m tapes" )
insert mediaTypes 
	values ("EXA-m-120", 7000000, 98.5, 3500000, 8192, 1, 1, "Medium Density Exabyte, 120m tapes" )
insert mediaTypes 
	values ("EXA-h-120", 14000000, 98.5, 7000000, 8192, 1, 1, "High Density Exabyte, 120m tapes" )
go
