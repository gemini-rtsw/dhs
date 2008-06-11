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
*   Name:		am/sql/media_units.sql
*
*   Purpose:
*	This file contains the table definitions for
*       the media_units table.
*
*   Database		: dhsTestDB
*
*   Date		: February 4, 1998
*
*   SCCS data		: @(#)
*	Module Name	: media_units.sql
*	Version Number	: 1.7
*	Release Number	: 1
*	Last Updated	: 04/28/99
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/02/04 SEC  :	Entire library copied from VLT development tree 
*			to CADC development tree. Initial check-in.
*	98/04/07 SEC  : Modifications for readability.
*	98/05/25 SEC  : Mods to indices. 
*	98/21/01 SDJ  : Changed the indexes on the table.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database dhsTestDB"
use dhsTestDB
go

print "Creating new media_units table"
go

if exists ( select name from sysobjects 
		where name = 'media_units' and type = 'U' )
		begin 
		    print "Dropping existing media_units table"
		    drop table media_units
		end
go


Create table media_units
(
	media_request_id	int		not null,
	media_unit_name	 	varchar(32)	not null,
	total_size	 	int		null,
	uncomp_size	 	int		null,
	copies_written	 	int		not null,
	date_completed	 	datetime	null,
	status		 	char(1)		null
)
go

print "Creating indexes"
go

create unique clustered index mu_primary on media_units
(
    media_request_id, media_unit_name
)
go

create index mu_id on media_units
(
    media_request_id
)
go

create index mu_id_status on media_units
(
    media_request_id, status
)
go

print "Granting"
go

#grant insert, update, delete, select on media_units to operator
#go

grant select on media_units to public
go

