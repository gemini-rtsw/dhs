/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 2000				(c) 2000
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
*   Name:		ad/sql/mfs.sql
*
*   Purpose:
*	This file contains the table and index definitions for
*       the mfs table.
*
*   Database		: archive
*
*   Date		: Mar 09, 2000
*
*   Programmer		: Shannon Jaeger
*
*   SCCS data		: @(#)
*	Module Name	: mfs.sql
*	Version Number	: 1.5
*	Release Number	: 1
*	Last Updated	: 03/26/01
*
*   Modification History:
*    2000/03/09	SDJ	: Copied from cdfiles.sql
*    2000/04/05 SDJ	: Removed volume_orig, and mounted from mfs table.
*			  Allowed mount_point to be null.  Added hostname
*			  to mfs table.
*    2000/04/10 SDJ	: Switch crc from int to binary data type.
*			  Made mfs_volume and mfs_crc unique indexes.
*    2001/02/05	SDJ	: Changed type to media_type, and format to
*			  file_system.  Added "class" column.  Changed
*			  verified to a datetime field and changed the
*			  column name to verify_date.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database dhsDB_NB"
use dhsDB_NB
go

print "Creating new mfs table"
go

Create table mfs
(
    crc			binary(4)	not null,
    archive		char(6)		not null,
    volume		varchar(32)	not null,
    creation_date	datetime	not null,
    ingest_date		datetime	not null,
    hostname		varchar(32)     null,
    mount_point		varchar(255)	null,
    nfiles		int		not null,
    nkbytes		int		not null,
    media_type		char(10)	not null,
    file_system		char(10)	not null,
    class		char(1)		not null,
    verify_date		datetime	null 
)
go

print "Creating indices"
go

create unique clustered index mfs_volume
	on mfs ( volume, archive, media_type, creation_date ) 
create unique index mfs_crc 
	on mfs ( crc )
go

print "Granting"
go

grant select on mfs to public
go

