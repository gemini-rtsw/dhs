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
*   Name:		am/sql/devices.sql
*
*   Purpose:
*	This file contains the table definitions for
*       the devices table.
*
*   Database		: dhsTestDB
*
*   Date		: August 28, 1998
*
*   SCCS data		: @(#)
*	Module Name	: devices.sql
*	Version Number	: 1.5
*	Release Number	: 1
*	Last Updated	: 03/15/99
*
*   Programmer		: Jennifer Dunn
*
*   Modification History:
*	98/10/21 SDJ	: Made Threshold and capacity values consistent
*			  with the configuration file.
*	98/11/23 JSD	: Moved capacity, threshold to mediaTypes table,
*			  and changed type to mediaType, changed media_type
*			  to be only 3 characters long.
*	98/20/01 SDJ	: Changed table name to devices, corrected initial
*			  inserts into the table.
*	99/03/15 NRH	: Changed media_type to 10 characters. Added 
*			  insertions.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database dhsTestDB"
use dhsTestDB
go

print "Creating new devices table"
go

if exists ( select name from sysobjects 
		where name = 'devices' and type = 'U' )
		begin 
		    print "Dropping existing devices table"
		    drop table devices
		end
go


Create table devices
(
	deviceName		varchar(32)	not null,
	media_type		char(10)	not null,
	driverName		varchar(32)	null,
	available		char(1)		not null,
	rSpeed			int		null,
	wSpeed			int		null,
	deviceInfo 		varchar(223)	null
)
go

print "Creating indexes"
go

create unique index dev_primary on devices
(
    deviceName, media_type
)
go

print "Granting"
go

#grant insert, update, delete, select on devices to operator
#go

grant select on devices to public
go

insert into devices values 
("/dev/dsk/c2t1d0s2", "CD", "CD1", "Y", 6, 4, "2,1,0" )
insert into devices values 
("/dev/rmt/0", "EXA-l-90", NULL, "Y", 1, 1, "l" )
insert into devices values 
("/dev/rmt/0", "EXA-m-90", NULL, "Y", 1, 1, "m" )
insert into devices values 
("/dev/rmt/0", "EXA-h-90", NULL, "Y", 1, 1, "h" )
insert into devices values 
("/dev/rmt/0", "EXA-l-120", NULL, "Y", 1, 1, "l" )
insert into devices values 
("/dev/rmt/0", "EXA-m-120", NULL, "Y", 1, 1, "m" )
insert into devices values 
("/dev/rmt/0", "EXA-h-120", NULL, "Y", 1, 1, "h" )
go
