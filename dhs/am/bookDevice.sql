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
*   Name:		am/sql/bookDevice.sql
*
*   Purpose:
*	This file contains the table definitions for
*       the bookDevice table.
*
*   Database		: dhsTestDB
*
*   Date		: August 28, 1998
*
*   SCCS data		: @(#)
*	Module Name	: bookDevice.sql
*	Version Number	: 1.6
*	Release Number	: 1
*	Last Updated	: 04/29/99
*
*   Programmer		: Jennifer Dunn
*
*   Modification History:
*	98/20/01 SDJ	: Changed timeStamp to a datetime type.
*	98/21/01 SDJ	: Changed name from book to bookDevice and
*			  added unique index.
*	99/03/14 SDJ	: Changed primary index to be clustered.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database dhsTestDB"
use dhsTestDB
go

print "Creating new bookDevice table"
go

if exists ( select name from sysobjects 
		where name = 'bookDevice' and type = 'U' )
		begin 
		    print "Dropping existing bookDevice table"
		    drop table bookDevice
		end
go


Create table bookDevice
(
	deviceName		varchar(32)	not null,
	inUse			char(1)		not null,
	processName		varchar(255)	null,
	processId		int		null,
	media_request_id	int		null,
	media_unit_name		varchar(32)	null,
	timeStamp		datetime	null,
	logicalName		varchar(32)	null
)
go

print "Creating indexes"
go

create unique clustered index db_primary on bookDevice
(
	deviceName
)
go


print "Granting"
go

#grant insert, update, delete, select on bookDevice to operator
#go

grant select on bookDevice to public
go

insert into bookDevice values
	( "/dev/dsk/c2t1d0s2", "N", NULL, NULL, NULL, NULL, NULL, NULL )
insert into bookDevice values
	( "/dev/rmt/0", "N", NULL, NULL, NULL, NULL, NULL, NULL )
go
