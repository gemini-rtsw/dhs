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
*   Name:		am/sql/logicalDevices.sql
*
*   Purpose:
*	This file contains the table definitions for
*       the logicalDevices table.
*
*   Database		: dhsTestDB
*
*   Date		: August 28, 1998
*
*   SCCS data		: @(#)
*	Module Name	: logicalDevices.sql
*	Version Number	: 1.7
*	Release Number	: 1
*	Last Updated	: 03/15/99
*
*   Programmer		: Jennifer Dunn
*
*   Modification History:
*	98/10/20 SDJ	: Fixed up bugs, wrong table name given in grants.
*	98/20/01 SDJ	: Changes logical_devices to logicalDevices.
*	98/21/01 SDJ	: Changed unique into to clustered and to be on
*			  deviceName, logicalName.
*	99/03/15 NRH	: Added an exabyte logical device.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database dhsTestDB"
use dhsTestDB
go

if exists ( select name from sysobjects 
		where name = 'logicalDevices' and type = 'U' )
		begin 
		    print "Dropping existing logicalDevices table"
		    drop table logicalDevices
		end
go

print "Creating new logicalDevices table"
go

Create table logicalDevices
(
	logicalName		varchar(32)	not null,
	deviceName		varchar(32)	not null
)
go

print "Creating indexes"
go

create unique clustered index ld_primary on logicalDevices
(
    logicalName, deviceName
)
go

print "Granting"
go

#grant insert, update, delete, select on logicalDevices to operator
#go

grant select on logicalDevices to public
go

insert logicalDevices values ("cd1", "/dev/dsk/c2t1d0s2" )
#insert logicalDevices values ("cd2", "/dev/rdsk/c1t4d0s0" )
#insert logicalDevices values ("cdPair1", "/dev/rdsk/c5t5d0s0" )
#insert logicalDevices values ("cdPair1", "/dev/rdsk/c1t4d0s0" )
insert logicalDevices values ("exa1", "/dev/rmt/0" )
go
