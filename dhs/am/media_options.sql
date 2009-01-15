/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 2000.				(c) 2000.
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
*   Name:		am/sql/media_options.sql
*
*   Purpose:
*	Whatever
*
*   Database		: ASTO
*
*   Date		: Jan 18, 2000
*
*   SCCS data		: @(#)
*	Module Name	: media_options.sql
*	Version Number	: 1.1
*	Release Number	: 1
*	Last Updated	: 01/18/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database dhsDB_NB"
use dhsDB_NB
go

print "creanting new media_options table"
go

if exists( select name from sysobjects 
		where name = 'media_options' and type = 'U' )
		begin
		    print "Dropping existing media_options table"
		    drop table media_options
	 	end
go


create table media_options
(
	media_request_id	int		not null,
	processName		varchar(255)	not null,
	options			varchar(255) 	not null
)
go



print "Creating indices"
go

create unique clustered index mo_primary on media_options
(
	media_request_id, processName
)
go



print "Granting"

grant insert, update, delete, select on media_options to operator
go

grant select on media_options to public
go




