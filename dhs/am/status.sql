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
*   Name:		am/sql/status.sql
*
*   Purpose:
*	This file contains the table definitions for
*       the status table.
*
*   Database		: dhsTestDB
*
*   Date		: February 4, 1998
*
*   SCCS data		: @(#)
*	Module Name	: status.sql
*	Version Number	: 1.6
*	Release Number	: 1
*	Last Updated	: 04/29/99
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/02/04 SEC  :	Entire library copied from VLT development tree 
*			to CADC development tree. Initial check-in.
*	98/04/07 SEC  : Modifications for readability.
*	98/04/29 SEC  :	Modified for new am library statusses.
*	98/01/21 SDJ  : Added unique index.
*	99/04/29 SDJ  :	status changed to char(2).  Added status
*			info. for media_actions table.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database dhsTestDB"
use dhsTestDB
go

print "Creating new status table"
go

if exists ( select name from sysobjects 
		where name = 'status' and type = 'U' )
		begin 
		    print "Dropping existing status table"
		    drop table status
		end
go



Create table status
(
	status	 		char(2)  	not null,
	column_name		varchar(32)	not null,
	table_name		varchar(32)	not null,
	short_description	varchar(45)	not null,
	description		varchar(225)	not null 
)
go
print "Granting"
go

create unique index st_primary on status
(
	status, table_name, column_name
)
go


#grant insert, update, delete, select on status to operator
#go

grant select on status to public
go

insert into status values (" ", "verified", "cds", "unverified", "long description field for unverified")
insert into status values ("V", "verified", "cds", "verified", "long description field for verified")

insert into status values (" ", "status", "cdfiles", "ok", "long description field for ok")
insert into status values ("E", "status", "cdfiles", "error", "long description field for error")

insert into status values (" ", "status", "media", "Creating", "Creating User Media Request")
insert into status values ("E", "status", "media", "Error", "Error creating User Media Request ")
insert into status values ("I", "status", "media", "In-Progress", "Media Request is In-Progress")
insert into status values ("D", "status", "media", "Done", "Processing of Media Request is Done")

insert into status values (" ", "status", "media_units", "Creating", "Creating Media Unit")
insert into status values ("Q", "status", "media_units", "Queued", "Media Unit has been Queued")
insert into status values ("P", "status", "media_units", "Prepared", "Media Unit has been Prepared for writing")
insert into status values ("W", "status", "media_units", "Written", "All copies of Media Unit have been Written")
insert into status values ("C", "status", "media_units", "Cleaned Up", "Cleanup task has been completed for Media Unit")
insert into status values ("D", "status", "media_units", "Done", "Processing of Media Unit is Done")

insert into status values (" ", "status", "media_files", "Unprocessed", "File has not yet been processed")
insert into status values ("R", "status", "media_files", "Retrieved", "File has been Retrieved")
insert into status values ("E", "status", "media_files", "Error", "Error occurred retrieving file")
insert into status values ("U", "status", "media_files", "Unavailable", "File is currently Unavailable from the Data Server")
insert into status values ("D", "status", "media_files", "Done", "Processing of this file is Done.")

insert into status values ("I", "action", "media_actions", "Ingesting", "Ingesting file information into the database.")
insert into status values ("IR", "action", "media_actions", "Uningesting", "Rolling back file ingesting into the database.")
insert into status values ("Q", "action", "media_actions", "Queuing", "Creating Media Unit.")
insert into status values ("QR", "action", "media_actions", "Unqueuing", "Removing Media Unit.")
insert into status values ("P", "action", "media_actions", "Preparing", "Preparing the Media Unit for writing.")
insert into status values ("PR", "action", "media_actions", "Unpreparing", "Rolling back the preparation proccess for the Media Unit.")
insert into status values ("W", "action", "media_actions", "Writing", "Writing  copies of the Media Unit.")
insert into status values ("WR", "action", "media_actions", "Unriting", "Decrementing the number of copies written for the Media Unit.")
insert into status values ("C", "action", "media_actions", "Cleaning Up", "Cleaning up resources used by Media Unit.")
insert into status values ("CR", "action", "media_actions", "Requeuing", "Setting unit back to the queued state after it has been cleaned up.")

go
