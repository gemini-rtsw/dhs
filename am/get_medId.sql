/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1998.                             (c) 1998.
* National Research Council             Conseil national de recherches
* Ottawa, Canada, K1A 0R6               Ottawa, Canada, K1A 0R6
* All rights reserved                   Tous droits reserves
* 
* NRC disclaims any warranties,         Le CNRC denie toute garantie
* expressed, implied, or statu-         enoncee, implicite ou legale,
* tory, of any kind with respect        de quelque nature que se soit,
* to the software, including            concernant le logiciel, y com-
* without limitation any war-           pris sans restriction toute
* ranty of merchantability or           garantie de valeur marchande
* fitness for a particular pur-         ou de pertinence pour un usage
* pose.  NRC shall not be liable        particulier.  Le CNRC ne
* in any event for any damages,         pourra en aucun cas etre tenu
* whether direct or indirect,           responsable de tout dommage,
* special or general, consequen-        direct ou indirect, particul-
* tial or incidental, arising           ier ou general, accessoire ou
* from the use of the software.         fortuit, resultant de l'utili-
*                                       sation du logiciel.
*
************************************************************************
*
*   Name:		am/sql/get_medId.sql
*
*   Purpose:
*	Define the sybase stored procedure used to get a new media ID.
*
*   Database		: dhsTestDB
*
*   Date		: February 4, 1998
*
*   SCCS data		: @(#)
*	Module Name	: get_medId.sql
*	Version Number	: 1.3
*	Release Number	: 1
*	Last Updated	: 04/22/99
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/02/04 SEC  :	Entire library copied from VLT development tree 
*			to CADC development tree. Initial check-in.
*	98/04/07 SEC  :	Modifications for readability.
*	99/04/22 NRH  : Changed the key name to "mediaId" from "GEMINI"
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database dhsTestDB"
use dhsTestDB
go

if exists ( select name from sysobjects
    where name = 'get_medId' and type = 'P')
    drop procedure get_medId 
go

create procedure get_medId
    @result int output
    with recompile
    as

    declare  @mediaId  int


    /*
     *  First reserve the media id number.
     */

    begin transaction
        update  keys
                set   keyValue = keyValue + 1
                where keyName = "mediaId"
 
	/*
	 *  Return the media id.
	 */

        select @result = keyValue from keys where keyName = "mediaId"
    commit transaction

go
go

#grant execute on get_medId to operator
#go
