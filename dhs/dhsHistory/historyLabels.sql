/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1997.				(c) 1997.
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
*   Name:		historyLabels.sql
*
*   Purpose:
*	Creation of the historyLabels table and its indices and permissions. 
*
*   Database		: ???
*
*   Date		: Sep 15, 1997
*
*   SCCS data		: 
*	Module Name	: 
*	Version Number	: 
*	Release Number	: 
*	Last Updated	: 
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database cadctemp"
use cadctemp
go

if exists ( select name from sysobjects
		where name = 'historyLabels' and type = 'U' )
		begin
		    print "Dropping existing historyLabels table"
		    drop table historyLabels
		end
go

print "Creating new historyLabels table"
go

create table historyLabels  
(
    dhsLabel	varchar(40)	not null,
    startDate	datetime	not null,
    startNsec	int		not null,
    endDate	datetime	not null,
    endNsec	int		not null
)
go

print "Creating indices on historyLabels table"
go

create unique index hf_unique on historyLabels 
    ( dhsLabel )
go

print "Granting privileges on historyLabels table"
go

grant insert, update, delete, select on historyLabels to operator
go

grant select on historyLabels to public
go

