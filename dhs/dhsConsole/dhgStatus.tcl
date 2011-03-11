# $Id: dhgStatus.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
#
#***********************************************************************
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#
# (c) 1997				(c) 1997
# National Research Council		Conseil national de recherches
# Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
# All rights reserved			Tous droits reserves
# 					
# NRC disclaims any warranties,		Le CNRC denie toute garantie
# expressed, implied, or statu-		enoncee, implicite ou legale,
# tory, of any kind with respect	de quelque nature que se soit,
# to the software, including		concernant le logiciel, y com-
# without limitation any war-		pris sans restriction toute
# ranty of merchantability or		garantie de valeur marchande
# fitness for a particular pur-		ou de pertinence pour un usage
# pose.  NRC shall not be liable	particulier.  Le CNRC ne
# in any event for any damages,		pourra en aucun cas etre tenu
# whether direct or indirect,		responsable de tout dommage,
# special or general, consequen-	direct ou indirect, particul-
# tial or incidental, arising		ier ou general, accessoire ou
# from the use of the software.		fortuit, resultant de l'utili-
# 					sation du logiciel.
#
#***********************************************************************
#
# FILENAME
# /dhsConsole/src/dhgStatus.itk
#
# PURPOSE:
# Set-up either the EPICS service or the DHS service  for monitoring 
# DHS status information.
#
# CLASS NAME(S)
# CDhgStatus			: sets-up the EPICS service in ocswish
#			  	  for monitoring DHS values.
#
# METHOD NAME(S)
# PUBLIC
# CDhgStatus::constructor	: Constructor for the CDhgStatus class.
# CDhgStatus::destructor	: Destructor for the CDhgStatus class.
# CDhgStatus::disable		: Turns off the monitoring of status records.
# CDhgStatus::enable		: Turns on the monitoring of status records.
# CDhgStatus::link		: Links a variable with a status record.  
#				  The value of the variable is the value of
#				  the status.
# CDhgStatus::proc		: Links a method or procedure with a
#				  status record.  Everytime the status record
#				  gets changed the procedure or method
#				  get called.
# CDhgStatus::unlink		: Removes an association, created with then
#				  link method,  btween a variable and a
#				  status record.
# CDhgStatus::unproc		: Removes an association, created with the
#				  proc method, between a method or
#				  procedure and a status record.
# CDhgStatus::value		: Returns the current value of a status
#				  record.
#
# PRIVATE
# CDhgStatus::channelConfig	: Finds all of the channel, status record,
#				  information in the configuration file.
# CDhgStatus::doUpdates		: Sets all "linked" variables and calls
#				  all "proced" methods/procedures associated
#				  with status records that have changed.
# CDhgStatus::myUpdate		: Routine that gets notified of changes
#				  in status records.
# CDhgStatus::overflow		: Handles overflow of records.
# CDhgStatus::routerConfig	: Determines the status router (DHS Status)
#				  information for the DHS Service.
# CDhgStatus::setChannel	: Sets the channel information and adds
#				  the channed (status record) to the DHS
#				  service.
#
# GLOBAL PROCEDURE(S)
# cDhgStatus			: Lowercased access method to the CDhgClass.
#
#
# $Log: dhgStatus.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:09  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.23  1999/06/21 21:49:24  jaeger
# Fixed overflow monitoring initialization.
#
# Revision 1.22  1999/06/17 20:28:37  jaeger
# Added monitoring of overflow records.  Warnings are displayed if an
# array record overflows.  Added verification of arraySizes with the
# Status Server and within the dhsConsole.config file.  Added to the code
# documentation. Made sure all methods have a return statement.  Renamed
# some of the class variables so that they are named more appropriately.
# Fixed bugs in the handling of the StatusQueue; potentially a value could
# sit in the queue with out being updated.
#
# Revision 1.21  1999/05/12 19:19:44  jaeger
# Added support for proper overflow handling.
#
# Revision 1.20  1998/08/14 21:50:47  jaeger
# Processing of new status information is delayed while a window has
# "grabbed" all X-events.  If this is not done status information is
# lost.
#
# Revision 1.19  1998/08/05 23:05:42  jaeger
# Improved output when an error is encounted executing a command associated
# with a status record.
#
# Revision 1.18  1998/06/25 20:15:34  jaeger
# Added a ping and flush command that is sent to the DHS status server
# on initialization.  This ensures we have all of the initial values.
# Changed the enable/disable methods.  Instead of disabled being a
# on/off value it now is a value greater than -1, where 0 means updating
# is enabled and anything greater than 0 means it is disabled.  Fixed bug
# where some of the status information was getting lost.  It was do to
# doUpdates setting the updateWaitQueue prematurely.
#
# Revision 1.17  1998/06/19 22:47:59  jaeger
# Brought code documentation up to Gemini standards. Fixed a bug where
# potentially update messages would be lost.  Allows applications to
# queue status record changes until they are ready for the information.
# In the DHS Service a check is done to make sure the DHS Status Server
# is running, and it is asked to flush all of its information.
# DHS status records are now configurable.
#
# Revision 1.16  1998/06/15 19:22:14  jaeger
# Changed fullName to fullname
#
# Revision 1.15  1998/06/15 05:15:25  jaeger
# Added resource "fullName" record.
#
# Revision 1.14  1998/05/22 17:01:57  jaeger
# Added code to determine whether to use SUMMIT or BASE records is determined
# from the configuration file for the dhs status acceptor only.
#
# Revision 1.13  1998/05/20 21:11:51  jaeger
# dhs service no longer reads CA_CONFIG file.  Instead information from
# the config file is used to set up the status items, should change to
# be more like the status server's configuration file.
# Reverted back to a single status acceptor "dhsSAD".
#
# Revision 1.12  1998/05/20 19:23:41  jaeger
# Added code to read CA_CONFIG file and to use the status router
# information in the dhsConfigFile.
#
# Revision 1.11  1998/05/15 18:14:26  jaeger
# Made the statusRouter informastion configurable.
#
# Revision 1.10  1998/05/15 17:48:40  jaeger
# Major change, now uses the -epics command line argument to determine
# whether to use epics or the dhs service for status monitoring.  It
# uses one or the other but not both.
#
# Revision 1.9  1998/05/04 20:43:53  jaeger
# No longer "link" or "proc" status items initially, instead this is done
# when they are needed.  The status records were broken into four
# lists, changes were made to accomidate this in this class.  Update
# Resource procedure moved to CDhgMain class.
#
# Revision 1.8  1998/04/15 22:38:29  jaeger
# Removed init method, added support for new config class, added support
# for a CA_CONFIG file broken into three status acceptors (dhsSAD, resourceSAD,
# and stoSAD).
#
# Revision 1.7  1998/04/14 19:18:47  jaeger
# Made sure string variables and [ code ] were double quoted.
#
# Revision 1.6  1998/04/08 20:57:34  jaeger
# Changed back to old way of handling EPICS initialization.
#
# Revision 1.5  1998/04/08 20:49:59  jaeger
# Removed updates.
#
# Revision 1.4  1998/03/06 18:00:34  jaeger
# Removed dbs, dhg, stg, dpg subsystems and added resource status info.
#
# Revision 1.3  1997/12/03 20:57:15  jaeger
# final revisiion, checked in for install
#
# Revision 1.2  1997/11/13 23:53:19  jaeger
# Checked in for install
#
# Revision 1.1  1997/10/30 19:24:39  jaeger
# Initial revision
#
#
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#***********************************************************************
#


#
#***********************************************************************
#+
# CLASS NAME:
# CDhgStatus
#
# OPTION(S)
# None.
#
# PUBLIC METHOD(S) 
# cget		: Standard cget method, returns the value of the specified
#                       option.
# configure	: Standard Tk method, sets or returns options.
# disable	: Temporarily turns off the monitoring of status records.
# enable	: Turn the monitoring of status records.
# isa		: Sets the widget's class.
# link 		: Link a status record with a variable.
# proc 		: Associate a command with a status record.
# unlink 	: Unink a status record with a variable.
# unproc 	: Disassociate a command with a status record.
# value 	: Find the value of a status record.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# Set-up EPICS or the DHS service for monitoring DHS status information.
# The service used depends on the value of the global "epics" variable.
#
#-
#***********************************************************************
#

itcl::class 	CDhgStatus {
    #
    # Public methods and data
    #

    public method constructor { args }
    public method destructor {} 

    public method enable { } { 
	if { $enabled > 0 } {
	   incr enabled -1
	}
	if { $updatePending == "" && $enabled == 0 } {
	    set updatePending [ after idle "[ code $this doUpdates ]" ]
	}
    }
    public method disable { } { 
	incr enabled
    }
    public method link { status variable }
    public method proc { status command }
    public method unlink { status variable }
    public method unproc { status command }
    public method value { status }

    
    #
    # Protected methods and data -- None
    #


    #
    # Private methods and data
    #

    private variable 	activeSubsystems    {}    ;# Susbsystems to be monitored
    private common	aliasArray	          ;# status record aliases
    private common	arraySizes		  ;# Sizes of record arrays.
    private common	configPath 	    ""    ;# Path to config. file.
    private common	dhsSAD 		    false ;# 
    private variable	enabled 	    0	  ;# TRUE means update values.
    private common	hostname 	    ""    ;# hostname for dhsStatus
    private common 	linkArray		  ;# Array of variables assoc.
    						   # with a status record.
    private common	overflowArray 		  ;# Keeps track of record
						   # arrays that have overflowed
    private common 	procArray		  ;# Array of procedures assoc.
    						   # with a status record.
    private common	prefix 		    ""    ;# prefix of all stat. records
    private variable	serviceName	    ""    ;# EPICS or DHS service
    private common	statusRouter 	    ""	  ;# Name of dhsStatus.
    private common	statusArray		  ;# Array with values of all
						   # status records.
    private common	updateLock 	    false ;# TRUE when update records
    private common	updatePending 	    ""    ;# TRUE when plan to update
    private common	updateQueue 	    {}    ;# Queue of records being
						   # updated
    private common 	updateWaitQueue     {}    ;# Queue of records waiting
						   # to be updated.


    private method	channelConfig { numTokens tokens }
    private method 	doUpdates {}
    private method	myUpdate { name alarm time valueList }
    private method	overflow { name alarm time valueList }
    private method	routerConfig { numTokens tokens }
    private method	setChannel { subsystem name alias max }
};

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cDhgStatus
#
# INVOCATION:
# cDhgStatus objectName ?args?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) objectName  	(string)	name of the object.
# (>) args		(string) 	List of subsystems.
#
# PROCEDURE VALUE:
# objectName (string) - the object name provided is returned.
#
# PURPOSE:
# To provide a lowercased acces method to the CDhgStatus class.
#
# DESCRIPTION:
# Trivial 
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc cDhgStatus { 
    name
    args
} {
    uplevel ::CDhgStatus "$name" "$args"
};

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgStatus::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) ?subsystemList?  (list)	List of subsystem that will be monitored.
#
# PROCEDURE VALUE:
# status - list of global status variables.
#
# PURPOSE:
# Set-up EPICS or DHS service for monitoring DHS status information.
#
# DESCRIPTION:
# 1. Store the subsystem list.
# 2. Create the EPICS or DHS service
# 3. Set-up variable links with the various status records that need
#	to be monitored by the DHS Console.
#
# EXTERNAL VARIABLES:
# CDhgStatus::activeSubsystems	(list)	  List of subsystems being monitored.
# CDhgStatus::aliasArray	(array)	  Contains status records aliases
# CDhgStatus::arraySizes	(array)	  Status record array sizes.
# CDhgStatus::configPath	(string)  Dir. path to the config. file.
# CDhgStatus::dhsSAD		(boolean) TRUE if DHS SAD has been set-up.
# CDhgStatus::hostname		(string)  Name of the host machine where
#					  the DHS Status Server is running.
# CDhgStatus::overflowArray	(array)	  Keeps track of the overflow records
# CDhgStatus::serviceName	(string)  EPICS or DHS service.
# CDhgStatus::statusArray	(array)	  Contains status records and
#					  there channel names.
# CDhgStatus::statusRouter	(string)  Specifes the name of the DHS status
#					  server and host it is running on.
# ::epics			(boolean) TRUE if epics is to be used.
#
# PRIOR REQUIREMENTS:
# The CA_CONFIG file has been set up.
#
# SEE ALSO:
# CDhgStatus::destructor
#
# DEFICIENCIES:
# Many of the record "aliases" are hard-coded in this class, including
# in this method.  It would be nice to be able to get the values from
# the CA_CONFIG file.
#
# The hard-coded overflow, and arraysize values must match those used
# by the DHS Storage, Status, On-line Data Processing, and Synchronous
# Data Processing servers.
#-
#***********************************************************************
#

body 	CDhgStatus::constructor {
    args
} {
    global ::epics

    disable

    set activeSubsystems [ join "$args" ]


    #
    # Verify Input and set class variables
    #

    if { [ isTrue $epics ] } {
	set serviceName epics
    } else {
	if { $statusRouter == "" } {
	    #
	    # Need to get status router information from
	    # the configuration file.
	    #
	    
	    set configPath [ file dirname [ cConfigGen _statusConfig ] ]
	    set result [ _statusConfig configOpen dhsConsole ]
	    if { [ lindex $result 0 ] == "ERROR" } {
		error "[ lindex $result 1 ]"
	    } 

	    if { [ _statusConfig configGet statusRouter 		\
		       [ code $this routerConfig ] ] != "SUCCESS" } {
		error "Unable to read Status Router infromation from\
 configuration file."
	    }
	    appData add statusRouter ${hostname}:$statusRouter
	}

	set serviceName dhs
    }



    #
    # Set up the status acceptor
    #

   service $serviceName


    #
    # If using the DHS Service we can verify if the status server is
    # running.
    #

    if { [ isFalse $epics ] } {
	dhs cs ping
	cs ping set destName ${hostname}:$statusRouter
	cs ping add subsystem -value STA
	set result [ cs ping postWait ]
	if { [ lindex $result 0 ] != "idle" } {
	    puts "ERROR: Unable to connect to the DHS Status Server; EXITING"
	    exit 2
	}
    }


    #
    # set up the DHS Status Alarm Database
    #

    if { ! [ simMode ] && [ isFalse $dhsSAD ] } {
	$serviceName sa dhsSAD
	set dhsSAD true
	if { [ isFalse $epics ] } {
	    if { [ _statusConfig configGet channel [ code $this 	\
			 channelConfig ] ] != "SUCCESS" } {
		error "Unable to read Channel information from configuration file."
	    }

	    foreach item [ array names statusArray ] {
		sa dhsSAD add $statusArray($item)
	    }
	}
    } else {
	#
	# Set up the arraySize and overflow arrays for the EPICS service.
	#

	set arraySizes(resArraySize)	12

	if { [ lsearch -exact $activeSubsystems sto ] != -1 } { 
	    set arraySizes(stoUnitArraySize) 	20
	    set arraySizes(stoStageArraySize) 	10
	    set arraySizes(stoActionArraySize) 	15
	    set arraySizes(stoDeviceArraySize) 	20
	    set arraySizes(stoReqArraySize) 	20

	    set overflowArray(stoUnitOverflow)          0
            set overflowArray(stoUnitOverflow,num)      stoNumUnits
	    set statusArray(stoNumUnit)			0
            set overflowArray(stoUnitOverflow,size)     stoUnitArraySize
	    set statusArray(stoUnitArraySize)		0
            set aliasArray(DHSS:STO:unitOverflow) 	stoUnitOverflow

            set overflowArray(stoActionOverflow)        0
            set overflowArray(stoActionOverflow,num)    stoNumActions
	    set statusArray(stoNumActions)		0
            set overflowArray(stoActionOverflow,size)   stoActionArraySize
	    set statusArray(stoActionArraySize)		0
            set aliasArray(DHSS:STO:actionOverflow) 	stoActionOverflow

            set overflowArray(stoReqOverflow)           0
            set overflowArray(stoReqOverflow,num)       stoNumReqs
	    set statusArray(stoNumReqs)			0
            set overflowArray(stoReqOverflow,size)      stoReqArraySize
	    set statusArray(stoReqArraySize)		0
            set aliasArray(DHSS:STO:reqOverflow) 	stoReqOverflow
	}
	
	if { [ lsearch -exact $activeSubsystems sdp ] != -1 } { 
            set overflowArray(sdpJobOverflow)   	0
            set overflowArray(sdpJobOverflow,num)	sdpNumJobs	
	    set statusArray(sdpNumJobs)			0
            set overflowArray(sdpJobOverflow,size)	sdpJobArraySize	
	    set statusArray(sdpJobArraySize)		0
            set aliasArray(DHSS:SDP:overflow)  		sdpJobOverflow
	}

	if { [ lsearch -exact $activeSubsystems olp ] != -1 } { 
	    set overflowArray(olpJobOverflow)           0
            set overflowArray(olpJobOverflow,num)	olpNumJobs	
	    set statusArray(olpNumJobs)			0
            set overflowArray(olpJobOverflow,size)	olpJobArraySize
	    set statusArray(olpJobArraySize)		0
            set aliasArray(DHSS:OLP:overflow) olpOverflow
	}
    }


    #
    # Close the configuration file, and delete the object if the object
    # exists.
    #

    if { [ find objects _statusConfig ] != "" } {
	_statusConfig configClose
	delete object _statusConfig
    }

	
    #
    # Set-up monitoring of "overflow flags.
    #

    if { ! [ simMode ] } {
	if { [ lsearch -exact $activeSubsystems olp ] != -1 } {
	    sa dhsSAD proc $statusArray(olpJobOverflow) "[ code $this overflow ]"
	    set overflowArray(olpJobOverflow)		0
	    set overflowArray(olpJobOverflow,num)	 olpNumJobs
	    set overflowArray(olpJobOverflow,size)	 olpJobArraySize
	    set aliasArray($statusArray(olpJobOverflow)) olpJobOverflow
	}

	if { [ lsearch -exact $activeSubsystems sdp ] != -1 } {
	    sa dhsSAD proc $statusArray(sdpJobOverflow) "[ code $this overflow ]"
	    set overflowArray(sdpJobOverflow)		  0
	    set overflowArray(sdpJobOverflow,num)	  sdpNumJobs
	    set overflowArray(sdpJobOverflow,size)	  sdpJobArraySize
	    set aliasArray($statusArray(sdpJobOverflow))  sdpJobOverflow

	}

	if { [ lsearch -exact $activeSubsystems sto ] != -1 } {
	    sa dhsSAD proc $statusArray(stoUnitOverflow) \
    		    "[ code $this overflow ]"    
	    set overflowArray(stoUnitOverflow)		0
	    set overflowArray(stoUnitOverflow,num)	stoNumUnits
	    set overflowArray(stoUnitOverflow,size)	stoUnitArraySize
	    set aliasArray($statusArray(stoUnitOverflow)) stoUnitOverflow


	    sa dhsSAD proc $statusArray(stoActionOverflow) \
		    "[ code $this overflow ]"
	    set overflowArray(stoActionOverflow) 	0
	    set overflowArray(stoActionOverflow,num) 	stoNumActions
	    set overflowArray(stoActionOverflow,size) 	stoActionArraySize
	    set aliasArray($statusArray(stoActionOverflow)) stoActionOverflow


	    sa dhsSAD proc $statusArray(stoReqOverflow) \
		    "[ code $this overflow ]"
	    set overflowArray(stoReqOverflow)		0
	    set overflowArray(stoReqOverflow,num)	stoNumReqs
	    set overflowArray(stoReqOverflow,size)	stoReqArraySize
	    set aliasArray($statusArray(stoReqOverflow)) stoReqOverflow
	}
    }


    #
    # Flush out all the status information.
    #

    if { [ isFalse $epics ] } {
	dhs cs flush
	cs flush set destName ${hostname}:$statusRouter
	cs flush post
    }
    enable

    return "$this"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgStatus::destructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Destructor for the CDhgStatus class.
#
# DESCRIPTION:
# "unproc" and "unlink" all of the status records being monitored.
#
# EXTERNAL VARIABLES:
# CDhgStatus::procArray		(array)	  Association array between methods
#				          (procedures) and status records.
# CDhgStatus::linkArray		(array)	  Association array between variables
#				          and status records.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhgStatus::constructor.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhgStatus::destructor {
} {
    #
    # Do unproc for all the commands/procedures
    #

    foreach status [ array names procArray ] {
	sa dhsSAD unproc $status "[ code $this myUpdate ]"
    }


    #
    # Do unlink for all the commands/procedures
    #

    foreach status [ array names linkArray ] {
	sa dhsSAD unproc $status "[ code $this myUpdate ]"
    }

    return
}

# ------------------------------------------------------------------
#                         PUBLIC METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgStatus::link
#
# INVOCATION:
# objectName link status variable
#
# where "objectName" is an instance of the CDhgStatus class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) status	(string) The status record the varible is to be linked too.
# (!) variable 	(string) The variable that will contain the status value.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To link a particular status record with the given variable; its value
# is the same as the value of the status record.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CDhgStatus::aliasArray	(array)	  Contains status records aliases
# CDhgStatus::linkArray		(array)	  Association array between variables
#				          and status records.
# CDhgStatus::serviceName	(string)  epics or dhs service.
# CDhgStatus::statusArray	(array)	  Contains status records and
#					  there channel names.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhgStatus::unlink
#
# DEFICIENCIES:
# The "link" is done at the topmost level so this could cause the
# name space to be cluttered with variables.
#-
#***********************************************************************
#

body 	CDhgStatus::link {
    status
    variable
} {

    #
    # Need to find the channel name if using the DHS service
    #

    if { "$serviceName" == "dhs" } {
	set aliasArray($statusArray($status)) $status
	set status $statusArray($status)
    }


    if { ! [ simMode ] } {
	if { ! [ info exists linkArray($status) ] } {
	    #
	    # Don't have this status record yet, so we need to "proc"
	    # our method with this status record.
	    #

	    sa dhsSAD proc $status "[ code $this myUpdate ]"
	    set linkArray($status) [ list "$variable" ]
	    uplevel #0 [list set $variable [ sa dhsSAD get $status ]]
	} elseif { [ lsearch $linkArray($status) $variable ] == -1 } {
	    lappend linkArray($status) "$variable"
	    uplevel #0 [list set $variable [ sa dhsSAD get $status ]]
	}
    } else {
	uplevel #0 [list set $variable "Simulation Mode"]
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgStatus::proc
#
# INVOCATION:
# objectName proc status command
#
# where "objectName" is an instance of the CDhgStatus class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) status	(string) 	The status record that is to be associated with
#				the command..
# (>) command 	(Tcl_command) 	The command that will be executed when
#				the status value changes.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To link a particular status record with the given procedure or
# class method..
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CDhgStatus::aliasArray	(array)	  Contains status records aliases
# CDhgStatus::procArray		(array)	  Association array between methods
#				          (procedures) and status records.
# CDhgStatus::serviceName	(string)  EPICS or DHS service.
# CDhgStatus::statusArray	(array)	  Contains status records and
#					  there channel names.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhgStatus::unproc
#
# DEFICIENCIES:
# The "proc" command is performed at the topmost level so the command
# must exist at the topmost level.
#-
#***********************************************************************
#

body 	CDhgStatus::proc {
    status
    command
} {
    if { "$serviceName" == "dhs" } {
	set SIR $statusArray($status)
	set aliasArray($SIR) $status
	set status $SIR
    }

    if { ! [ simMode ] } {
	if { ! [ info exists procArray($status) ] } {
	    #
	    # Don't have this status record yet so we need to make
	    # sure "myUpdate" gets called for this record.
	    #

	    sa dhsSAD proc $status "[ code $this myUpdate ]"
	    set procArray($status) [ list "$command" ]
	} elseif { [ lsearch $procArray($status) "$command" ] == -1 } {
	    lappend procArray($status) "$command"
	}

    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgStatus::unlink
#
# INVOCATION:
# objectName unlink status variable
#
# where "objectName" is an instance of the CDhgStatus class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) status	(string) The status record the varible is to be unlinked with.
# (!) variable 	(string) The variable that will contain the status value.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To unlink a particular record status record with the given variable.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CDhgStatus::aliasArray	(array)	  Contains status records aliases
# CDhgStatus::linkArray		(array)	  Association array between variables
#				          and status records.
# CDhgStatus::procArray		(array)	  Association array between methods
#				          (procedures) and status records.
# CDhgStatus::serviceName	(string)  EPICS or DHS service.
# CDhgStatus::statusArray	(array)	  Contains status records and
#					  there channel names.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhgStatus::link
#
# DEFICIENCIES:
# The "unlink" is done at the topmost level so this could cause the
# name space to be cluttered with variables.
#-
#***********************************************************************
#

body 	CDhgStatus::unlink {
    status
    variable
} {
    if { ! [ simMode ] } {
	if { "$serviceName" == "dhs" } {
	    set aliasArray($statusArray($status)) $status
	    set status $statusArray($status)
	}

	if { [ info exists linkArray($status) ] } {
	    if { [ set pos [ lsearch $linkArray($status) $status ] ] != -1 } {
		set linkArray($status) [ lreplace $linkArray($status)	\
					    $pos $pos ]
	    }
	    if { [ llength linkArray($status) ] < 1 } {
		unset linkArray($status)
		if { [ llength procArray($status) ] < 1 } {
		    sa dhsSAD unproc $status "[ code $this myUpdate ]"
		}
	    }
	}
    } else {
	unset "$variable"
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgStatus::unproc
#
# INVOCATION:
# objectName unproc status command
#
# where "objectName" is an instance of the CDhgStatus class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) status	(string) 	The status record whose association
#				the command is to be terminated.
# (>) command 	(Tcl_command) 	The command that will be executed when
#				the status value changes.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To unlink a particular status record with the given command.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CDhgStatus::aliasArray	(array)	  Contains status records aliases
# CDhgStatus::linkArray		(array)	  Association array between variables
#				          and status records.
# CDhgStatus::procArray		(array)	  Association array between methods
#				          (procedures) and status records.
# CDhgStatus::serviceName	(string)  EPICS or DHS service.
# CDhgStatus::statusArray	(array)	  Contains status records and
#					  there channel names.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhgStatus::proc
#
# DEFICIENCIES:
# The "unproc" command is performed at the topmost level so the command
# must exist at the topmost level.
#-
#***********************************************************************
#

body CDhgStatus::unproc {
    status
    command
} {
    if { "$serviceName" == "dhs" } {
	set aliasArray($statusArray($status)) $status
	set status $statusArray($status)
    }

    if { ! [ simMode ] } {
	#
	# Find the status record
	#

	if { [ info exists procArray($status) ] } {
	    if { [ set pos [ lsearch $procArray($status) $status ] ] != -1 } {
		set procArray($status) [ lreplace $procArray($status)	\
					    $pos $pos ]
	    }
	    if { [ llength procArray($status) ] < 1 } {
		unset procArray($status)
		if { [ llength linkArray($status) ] < 1 } {
		    #
		    # No longer need to monitor the status record
		    #

		    sa dhsSAD unproc $status "[ code $this myUpdate ]"
		}
	    }
	}
    } 

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgStatus::value
#
# INVOCATION:
# objectName value status
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) status	(string)    The status record whose value is desired.
#
# PROCEDURE VALUE:
# retValue	(string)    : The value found for the status record
#			      in EPICS or from the DHS Status Server.
# PURPOSE:
# Return the value of a particular status record.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CDhgStatus::aliasArray	(array)	  Contains status records aliases
# CDhgStatus::arraySizes	(array)   Contains the size of a number of
#					  status record arrays.
# CDhgStatus::statusArray	(array)	  Contains status records and
#					  there channel names.
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhgStatus::value { 
    status
} {
    if { "$serviceName" == "dhs" } {
	set alias $status
	set aliasArray($statusArray($status)) $status
	set status $statusArray($status)
    }

    set retValue "[ sa dhsSAD get $status value ]"
    if { [ info exists arraySizes($alias) ] } {
	if { ! [ info exists arraySizes(${alias},warn) ] ||
	     [ isFalse $arraySizes(${alias},warn) ] } {
	    
	    if { ! [ isNumber $retValue ]  || $retValue < 0 } { 
		warningDialog "Value of $status is not a positive numerical \
value: \"${retValue}\""
		set arraySizes(${alias},warn) 1
	    } elseif { $retValue !=  $arraySizes($alias) } {
		warningDialog "Value of $status (${retValue}) does not \
match configuration file value: \"$arraySizes($alias)\""
		set arraySizes(${alias},warn) 1
	    }
	}
	set retValue $arraySizes($alias)
    }


    return "$retValue"
}

# ------------------------------------------------------------------
#                        PRIVATE METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgStatus::routerConfig
#
# INVOCATION:
# routerConfig numTokens tokens
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) numTokens (int)   Number of tokens to parse.
# (>) tokens    (list)  List containing the tokens found on a line in
#                               the configuration file. 
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To find the configuration information for the status Router.  This
# is used with the dhs status acceptor only (epics doesn't need this.)
#
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# CDhgStatus::activeSubsystems	(list)	  List of subsystems being monitored.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# None
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhgStatus::channelConfig {
    numTokens
    tokens
} {
    if { [ lindex $tokens 0 ] != "channel" } {
	error "Expected configuration information for the channel\
 not [ lindex $tokens 0 ]"
    } elseif { $numTokens != 5 } {
	error "Configuration file error, expected 5 tokens for the\
 channel information but got $numTokens"
   }

    set system [ lindex $tokens 1 ] 
    set name [ lindex $tokens 2 ] 
    set alias [ lindex $tokens 3 ]
    set max [ lindex $tokens 4 ]
    
    if { $system == "*" } {
	foreach subsystem $activeSubsystems {
	    setChannel $subsystem "$name" "$alias" "$max"
	}
    } else {
	setChannel "$system" "$name" "$alias" "$max"
    }


    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsStatus::doUpdates
#
# INVOCATION:
# CDhsStatus::doUpdates
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To update the value of all variables associated with a status record
# and to call all procedure associated with a status record
#
# DESCRIPTION:
# Foreach known variable associated with the the status record set the
# variable to the new value.
#
# Foreach procedure/method associated with the status record invoke the
# procedure/method with the appropriate arguments.
#
# EXTERNAL VARIABLES:
# CDhgStatus::procArray		(array)	  Association array between methods
#				          (procedures) and status records.
# CDhgStatus::linkArray		(array)	  Association array between variables
#				          and status records.
# CDhgStatus::updateQueue 	(queue)	  Queue of status records that need
#					  updating.
# CDhgStatus::updateLock	(boolean) Lock for the update Queue.
# CDhgStatus::updatePending	(integer) Indicates if an status updates are
#					  going to occur.
# CDhgStatus::serviceName	(string)  EPICS or DHS service.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhgStatus::myUpdate
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhgStatus::doUpdates {
} {
    #
    # If some window has the "grab" then delay updates, otherwise
    # they won't be done.  Also if we are currently doing an update
    # then wait until it completes to perform the new update.
    #

    if { [ grab current ] != "" || [ isTrue $updateLock ] } {
	if { "$updatePending" !=  "" } {
	    after cancel $updatePending
	}
	set updatePending  [ after 500 "[ code $this doUpdates ]" ]
	return
    }

    
    #
    # Set the lock so no more items are added to the updateQueue 
    #

    set updateLock true
    set updatePending ""

    foreach record $updateQueue {
	set status [ lindex $record 0 ]
	set name [ lindex [ split $status . ] 1 ]
	if { "$serviceName" == "dhs" } {
	    #
	    # If we are using the dhs service we need to alter the
	    # name of the record.  The Consoles use the aliases not
	    # the full record name.
	    #

	    set SAD [ lindex [ split $status . ] 0 ]
	    set status [ lindex [ split $status . ] 1 ]
	    if { [ info exists aliasArray($status) ] } {
		set status $SAD.$aliasArray($status)
	    }
	}
	set alarm [ lindex $record 1 ]
	set time [ lindex $record 2 ]
	set valueList [ lindex $record 3 ]



	#
	# Invoke all known procedure/methods associated with the
	# record.
	#

	if { [ array names procArray $name ] != "" } {
	    foreach cmd $procArray($name) {
		if { [ catch "[ subst $cmd ] [ list $status ] [ list $alarm ] [ list $time ] [ list $valueList ]" msg ] } {
		    puts "ERROR EXECUTING CMD: \"$cmd\" \n\tfor status record\
 $name\n\tMSG:$msg"
		} 
	    }	    
	}


	#
	# Set all known variables associated with the record to the
	# new value.
	#

	if { [ array names linkArray $name ] != "" } {
	    foreach variable $linkArray($name) {
		uplevel #0 "set $variable \"$valueList\""
	    }	    
	}
    }

    set updateQueue {}
    set updateLock false
    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgStatus::myUpdate
#
# INVOCATION:
# CDhgStatus::myUpdate name alarm time valueList
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name      (string)        Name of the status record being updated.
# (>) alarm     (string)        Alarm status, OK, Warning or Bad
# (>) time      (time)          Time of the update.
# (>) valueList (list)          The new value.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To respond to a change in an EPICS value.
#
# DESCRIPTION:
# If the lock is on for the update queue add the new status record
# information to the wait queue.  If monitoring is turned on, and
# if we aren't already doing updates then update.
#
# EXTERNAL VARIABLES:
# CDhgStatus::enabled		(boolean) TRUE means report status changes.
# CDhgStatus::procArray		(array)	  Association array between methods
#				          (procedures) and status records.
# CDhgStatus::linkArray		(array)	  Association array between variables
#				          and status records.
# CDhgStatus::updateQueue 	(queue)	  Queue of status records that need
#					  updating.
# CDhgStatus::updateWaitQueue 	(queue)	  Queue of status records that need
#					  updating, this queue is created
#					  while the Console is waiting for
#					  myUpdate to finish.
# CDhgStatus::updateLock	(boolean) Lock for the update Queue.
# CDhgStatus::updatePending	(integer) Indicates if an status updates are
#					  going to occur.
# ::debug	(NONE, MIN, FULL) - Specifies the DHS Console debugging level.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhgStatus::myUpdate
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhgStatus::myUpdate {
    name
    alarm
    time
    valueList
} {
    global ::debug

    set status [ lindex [ split $name . ] 1 ]

    if { $debug == "FULL" } {
	puts [ format "STATUS: %-25s New Value: %s" $status "$valueList" ]
    }


    if { ( ! [ info exists linkArray($status) ] ||
	   ! [ info exists procArray($status) ] ) &&
	 $debug == "MIN" } {
	puts [ format "STATUS: %-25s New Value: %s" $status "$valueList" ]
    } 

    
    if { [ isTrue $updateLock ] } {
	lappend updateWaitQueue [ list "$name" "$alarm" "$time" "$valueList" ]
    } else {
	if { $updateQueue == {} } {
	    set updateQueue "$updateWaitQueue"
	    set updateWaitQueue {}
	}
	lappend updateQueue [ list "$name" "$alarm" "$time" "$valueList" ]
    }

    if { $updatePending == "" && $enabled == 0 } {
	set updatePending [ after 250 "[ code $this doUpdates ]" ]
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgStatus::overflow
#
# INVOCATION:
# CDhgStatus::overflow name alarm time valueList
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name      (string)        Name of the status record being updated.
# (>) alarm     (string)        Alarm status, OK, Warning or Bad
# (>) time      (time)          Time of the update.
# (>) valueList (list)          The new value.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To respond to a change in a status overflow value and display a message
# warning the user of the overflow.
#
# DESCRIPTION:
# If the overflow value has changed then display a message.
#
# EXTERNAL VARIABLES:
# CDhgStatus::aliasArray	(array)	  Contains status records aliases
# ::debug	(NONE, MIN, FULL) - Specifies the DHS Console debugging level.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhgStatus::overflow {
    name
    alarm
    time
    valueList
} {
    global ::debug 

    set status [ lindex [ split $name . ] 1 ]
    set alias $aliasArray($status)


    if { $debug == "FULL" } {
	puts [ format "STATUS: %-25s New Value: %s" $status "$valueList" ]
    }
    if {  [ array names overflowArray $alias ] != "" &&
	  ( ( [ isTrue $overflowArray($alias) ] &&  [ isTrue $valueList ] ) ||
	  ( [ isFalse $overflowArray($alias) ] &&  [ isFalse $valueList ] ) ||
	  ( $overflowArray($alias) == -1 && [ isFalse $valueList ] ) ) } {
	#
	# No change in overflow value. Nothing to report.
	#

	return
    }


    #
    # Find the number of records in the array and the size of
    # the array
    #

    if { [ array names overflowArray "${alias},num" ] != "" } {
	set value [ $this value $overflowArray(${alias},num) ]
	set numRecs " (${value}) "
    } else {
	set numRecs " "
    }

    if { [ array names overflowArray "${alias},size" ] != "" } {
	set value [ $this value $overflowArray(${alias},size) ]
	set arraySize " (${value})"
    } else {
	set arraySize ""
    }

	
    set msg ""
    if { [ isTrue $valueList ] } { 
	#
	# Warn operator that there are items that are not being displed.
	#

	switch -exact -- "$alias" {
	    olpOver {
		set formatStr "The number of On-line Data Processing jobs%shas\
exceeds the number of jobs that can be displayed%s."
	    }
	    sdpOver {
		set formatStr "The number of Synchronous Data Processing \
jobs%shas exceeds the number of jobs that can be displayed."
	    }
	    stoUnitOverflow {
		set formatStr "The number of media units%sin the Storage \
Server's  media unit list has exceeded the number of units that can be \
displayed%s."
	    }
	    stoActionOverflow {
		set formatStr "The number of actions%sin the Storage Server's\
 action list has exceeded the number of actions that can be displayed%s."
	    }
	    stoReqOverflow {
		set formatStr "The number of Storage Server media request has%s\
exceeded the number of requests that can be displayed%son the Storage Server's\
 window."
	    }
	    default {
		if { $debug == "FULL" || $debug == "MIN" } {
		    puts "ERROR: Undefined overflow status $alias."
		}
	    }
	}
	
	set msg [ format "$formatStr" $numRecs $arraySize ]
	set overflowArray($alias) 1
	if { $msg != "" } {
	    if { $debug == "FULL" || $debug == "MIN" } {
		puts "WARNING: $msg"
	    }
	    warningDialog "$msg"
	}

    } elseif { [ isFalse $valueList ] } {
	#
	# Inform operator that display is now complete
	#

	switch -exact -- "$alias" {
	    olpOver {
		set formatStr "All on On-line Data Processing jobs%sare now\
 being displayed."
	    }
	    sdpOver {
		set formatStr "All Synchronous Data Processing jobs%sare \
now being displayed."
	    }
	    stoUnitOverflow {
		set formatStr "All of the Storage Server's media units%s\
are now being displayed."
	    }
	    stoActionOverflow {
		set formatStr "All of the Storage Server's actions%sare \
now being displayed."

	    }
	    stoReqOverflow {
		set formatStr "All of the Storage Server's media request%s\
are now being displayed."
	    }
	    default {
		if { $debug == "FULL" || $debug == "MIN" } {
		    puts "ERROR: Undefined overflow status $alias."
		}
	    }
	}
	set overflowArray($alias) 0
	set msg [ format "$formatStr" $numRecs ]

	if { $msg != "" } {
	    if { $debug == "FULL" || $debug == "MIN" } {
		puts "INFO: $msg"
	    }
	    infoDialog "$msg"
	}
    } else {
	#
	# Non boolean value given.  Report error.
	#

	if { $debug == "FULL" || $debug == "MIN" } {
	    puts "ERROR: Non-boolean value given for Status record $status"
	}
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgStatus::routerConfig
#
# INVOCATION:
# routerConfig numTokens tokens
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) numTokens (int)   Number of tokens to parse.
# (>) tokens    (list)  List containing the tokens found on a line in
#                               the configuration file. 
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To find the configuration information for the status Router.  This
# is used with the dhs status acceptor only (epics doesn't need this.)
#
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# CDhgStatus::hostname		(string)  Name of the host machine where
#					  the DHS Status Server is running.
# CDhgStatus::prefix		(string)  Prefix for the status record names.
# CDhgStatus::statusRouter	(string)  Specifes the name of the DHS status
#					  server and host it is running on.#
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# None
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhgStatus::routerConfig {
    numTokens
    tokens
} {
    if { [ lindex $tokens 0 ] != "statusRouter" } {
	error "Expected configuration information for the statusRouter\
 not [ lindex $tokens 0 ]"
    } elseif { $numTokens != 5 } {
	error "Configuration file error, expected 5 tokens for the\
 status Router information but got $numTokens"
     } else {
        set statusRouter [ lindex $tokens 1 ] 
        set hostname [ lindex $tokens 2 ] 
	 set prefix "DH[ lindex $tokens 3 ][ lindex $tokens 4 ]"
    }
    
    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgStatus::setChannel
#
# INVOCATION:
# setChannel subsystem name alias max
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) subsystem	(string)	"*" = all subsystems
#				"-" = no subsystems
#				"XXX" = some subsystem identifier, such as STO
# (>) name	(string)	Name of the EPICS channel
# (>) alias	(string)	Alias associatied with the EPICS channel
# (>) max	(integer)	Number of channels
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To initialize the DHS Service with the appropirate channels, based on
# the information in the configuration file.
#
# DESCRIPTION:
# If the channel name has a "%d" in it substitute this with a numerical
# value ( 0 - max).  
# Store the channel name in an associative array, indexed by the aliases.
#
# Note: this routine should not be called if the EPICS service is used.
#
# EXTERNAL VARIABLES:
# CDhgStatus::aliasArray	(array)	  Contains status records aliases
# CDhgStatus::arraySizes	(array)   Contains the size of a number of
#					  status record arrays.
# CDhgStatus::overflowArray	(array)	  Keeps track of the overflow records
# CDhgStatus::prefix		(string)  Prefix for the status record names.
# CDhgStatus::statusArray	(array)	  Contains status records and
#					  there channel names.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhgStatus::channelConfig
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhgStatus::setChannel {
    subsystem
    name
    alias
    max
} { 
    #
    # Get a lower and upper case subsystem identifier.
    #

    if { "$subsystem" == "-" } {
	set lowerSub ""
	set upperSub ""
    } else {
	set lowerSub [ string tolower $subsystem ]
	set upperSub "[ string toupper $subsystem ]:"
	set alias "[ string toupper [ string range $alias 0 0 ] ][ string\
	        range $alias 1 end ]"
    }

    if { [ string match "*%d*" "$name" ] } {
	#
	# String requires substitution, number for "%d"
	#

	if { ! [ isNumber $max ] } {
	    error "Channel has had non-numeric maximum value for channel $name"
	}


	#
	# Remember the maximum value
	#

	set end [ expr [ string first "%d" "$name" ] - 1 ]
	set formalName "[ string toupper [ string range $name 0 0 ] ][ string range $name 1 $end ]"
	if { $subsystem != "-" } {
	    set maxRecord "[ string tolower $subsystem ]${formalName}ArraySize"
	} else {
	    set maxRecord "resArraySize"
	}

	if { [ array names arraySizes $maxRecord ] == "" } {
	    set arraySizes($maxRecord) $max
	} elseif { $arraySizes($maxRecord) != $max } {
	    error "Max value for channel $name should be $arraySizes($maxRecord)"
	}


	#
	# Determine the aliases and record names.
	#	

	for {set i 0} {$i < $max} {incr i} {
	    set tmpName [ format "$name" $i ]
	    set tmpAlias [ format "$alias" $i ]
	    set statusArray(${lowerSub}$tmpAlias) 			\
		"${prefix}:${upperSub}$tmpName"
	}
    } else {
	#
	# Set up the alias array.
	#

	set alias ${lowerSub}$alias
	set statusArray($alias) "${prefix}:${upperSub}$name"


	#
	# Check for overflow values.
	#

	if { [ regexp -nocase "overflow" "$name" ] } {
	    set overflowArray(${lowerSub}${alias})	0
	    set aliasArray($statusArray(${alias}))	$alias
	}


	if { [ string match "Num" "$name" ] } {
	    #
	    # Find the array size that corresonds to this record.
	    #

	    set last [ expr [ string length $name ] - 2 ]
	    set arrayType [ string range 3 $last $name ]
	    set arrayUsed(${arrayType}ArraySize) $name
	}
    }

    return
}
