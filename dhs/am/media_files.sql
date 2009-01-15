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
*   Name:		am/sql/media_files.sql
*
*   Purpose:
*	This file contains the table definitions for
*	the media_files table.
*
*   Database		: dhsTestDB
*
*   Date		: February 4, 1998
*
*   SCCS data		: @(#)
*	Module Name	: media_files.sql
*	Version Number	: 1.6
*	Release Number	: 1
*	Last Updated	: 03/05/99
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/02/04 SEC  :	Entire library copied from VLT development tree 
*			to CADC development tree. Initial check-in.
*	98/04/07 SEC  : Modifications for readability.
*	98/05/25 SEC  : Mods to indices. 
*	99/03/05 NRH  : Added file name to index mf_primary.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database dhsTestDB"
use dhsTestDB
go

print "Creating new media_files table"
go
if exists ( select name from sysobjects 
		where name = 'media_files' and type = 'U' )
		begin 
		    print "Dropping existing media_files table"
		    drop table media_files
		end
go


Create table media_files
(
	file_id	 		varchar(45)	not null,
	filename 		varchar(223)	not null,
	media_request_id	int		not null,
	media_unit_name		varchar(32)	null,
	date_notified		datetime	not null,
	source_id		varchar(32)	not null,
	compression_flag	char(1)		null,
	filesize	 	int  		not null,
	uncomp_filesize 	int  		not null,
	status	 		char(1) 	null 
)
go

print "Creating indexes"
go

create unique clustered index mf_primary on media_files
(
    media_request_id, file_id, filename
)
go

create index mf_id_unit on media_files
(
    media_request_id, media_unit_name
)
go

create index mf_id_status_unit on media_files
(
    media_request_id, status, media_unit_name
)
go

create index mf_id_status on media_files
(
    media_request_id, status
)
go

print "Granting"
go

#grant insert, update, delete, select on media_files to operator
#go

grant select on media_files to public
go
