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
*   Name:		media_actions.sql
*
*   Purpose:
*	Whatever
*
*   Database		: dhsTestDB
*
*   Date		: Apr 27, 1999
*
*   SCCS data		: @(#)
*	Module Name	: media_actions.sql
*	Version Number	: 1.2
*	Release Number	: 1
*	Last Updated	: 05/03/99
*
*   Programmer		: Norm Hill
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database dhsTestDB"
use dhsTestDB
go

print "creating new media_actions table"
go

if exists ( select name from sysobjects 
                where name = 'media_actions' and type = 'U' )
                begin 
                    print "Dropping existing media_actions table"
                    drop table media_actions
                end
go


create table media_actions
(
        media_request_id        int             not null,
        media_unit_name         varchar(32)     null,
	action			char(2)		not null,
	time			datetime	not null,
	process_id		int		not null
)
go

print "Granting"
go

#grant insert, update, delete, select on media_actions to operator
#go

grant select on media_actions to public
go
