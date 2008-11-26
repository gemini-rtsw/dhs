/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1996				(c) 1996.
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
*   Name:		ad/sql/ods.sql
*
*   Purpose:
*	This file contains the table and index definitions for
*       the ods table.
*
*   Database		: archive
*
*   Date		: Dec 3, 1996
*
*
*   Field SCCS data	: @(#)
*	Module Name	: ods.sql
*	Version Number	: 1.6
*	Release Number	: 1
*	Last Updated	: 09/23/97
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/01/29 SEC :	Added prints, added grant statements.
*       97/06/20 SEC  : Tidy up a bit.
*	97/09/23 pdd	: Changed to archive database
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database archive"
use archive
go

print "Creating new ods table"
go

Create table ods
(
	diskname	varchar(32)	not null,
	reverse		varchar(32)	not null,
	creation_date	int		not null,
	ingest_date	int		not null,
	nfiles		int		not null,
	nkbytes		int		not null,
	flag		char(1)		not null,
	shelf		char(2)		not null,
	error_count	int		not null,
	remark		varchar(70)	not null,
	accesses	smallint	not null,
	archive		char(6)		not null,
	dirsectoff	int		not null,
	dtype		varchar(8)	not null,
	diskname_orig	varchar(32)	not null 
)
go


print "Creating indices"
go

create unique clustered index ods_index1 on ods ( diskname ) 
	with ignore_dup_key 
go


print "Granting"
go

grant insert, update, delete, select on ods to operator
go

grant select on ods to public
go

