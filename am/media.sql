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
*   Name:		am/sql/media.sql
*
*   Purpose:
*       This file contains the table definitions for the media table.
*
*   Database		: dhsTestDB
*
*   Date		: February 4, 1998
*
*   SCCS data		: @(#)
*	Module Name	: media.sql
*	Version Number	: 1.6
*	Release Number	: 1
*	Last Updated	: 04/22/99
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/02/04 SEC  :	Entire library copied from VLT development tree 
*			to CADC development tree. Initial check-in.
*	98/04/07 SEC  : Modifications for readability.
*	98/09/25 SDJ  : Added precision and split level to media.
*	99/03/15 NRH  : Changed media_type to 10 characters. Removed split
*			size.
*	99/04/20 NRH  : Removed gemini specific naming.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database dhsTestDB"
use dhsTestDB
go

print "Creating new media table"
go

if exists ( select name from sysobjects 
		where name = 'media' and type = 'U' )
		begin 
		    print "Dropping existing media table"
		    drop table media
		end
go


Create table media
(
	media_request_id 	int		not null,
	media_request_name	varchar(32)	not null,
	source_id	 	varchar(32)	not null,
	date_requested	 	datetime	not null,
	date_completed	 	datetime	null,
	media_request_type	char(4)		not null,
	media_type	 	char(10)	not null,
	num_copies	 	int		not null,
	unit_name_root	 	varchar(32)	not null,
	unit_name_precision	int		not null,
	status	 		char(1)		not null 
)
go

print "Creating indices"
go

create unique index m_primary on media
(
    media_request_id
)
go

print "Granting"
go

#grant insert, update, delete, select on media to operator
#go

grant select on media to public
go


/*
 *  The integer value of the largest media_id in this file must match
 *  initial value in the keys.sql file.  The entries made below must also
 *  match the segregation information in the storageServer.config file.
 */
 
print "Inserting default archive media"
go
 
# GEMINI_N is the prefix on CDROM label
insert media values ( 1, "Gemini", "dataServer", getdate(), null, "ARCH", "CD", 2, "GEMINI_N", 4, "I" )
go

