# $Id: led.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
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
# dhsConsole/src/led.itk
#
# PURPOSE:
# Create an CLed mega-widget with a childsite class.
#
# CLASS NAME(S)
# CLed class		: A mega-widget that contains a frame (LED) and a 
#			 childsite. It inherits items from [ incr ] Tk 
#			 "Widget" class.
#
# METHOD NAME(S)
# PUBLIC
# CLed::alignleds	: Aligns the labels and LEDs with a set of LEDs.
# CLed::childsite	: Returns the window pathname of the LED's childsite.
# CLed::constructor	: Constructor for the CLed class.
# CLed::destructor	: Destructor for the CLed class.
#
# PROTECTED
# CLed::positionLed	: Positions the LED with its label.
#
# PRIVATE
# CLed::updateColor	: Gets called when the variable specified by 
#			  -ledvariable changes value.
#
#
# GLOBAL PROCEDURE(S)
# CLed		Lower cased access to the CLed class.
#
# $Log: led.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:09  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.8  1999/06/07 22:30:42  jaeger
# Fixed up the variable monitoring.  Handles "scoped" variables properly
# now.
#
# Revision 1.7  1998/06/22 17:01:21  jaeger
# Brought documentation up to Gemini standards.  Renamed class to CLed.
#
# Revision 1.6  1998/06/15 05:03:49  jaeger
# Removed code for doing help information, it was not needed.  Also got
# rid of some of the headers by moving method/option definitions into
# the class definition.
#
# Revision 1.5  1998/06/02 16:42:23  jaeger
# Added code that trace a variable and changes the colour of the
# LED based on a colour associations list.
#
# Revision 1.4  1998/05/04 18:42:26  jaeger
# Fixed switch statement, added the "--" flag.
#
# Revision 1.3  1998/04/14 18:59:56  jaeger
# Corrected "after" commands, fixed spacing around [ and ]'s
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
# CLed
#
# OPTION(S)
# background	Specifies the background color of the shell.
# clientdata	A hook that clients can use to store data with each widget.
# colorassoc	List that associates a color with a value.
# cursor	Specifies the mouse crsor to be used.
# led		Use to toggle the LED to "on" and "off".
# ledcolor      Specifies the color of the LED.
# ledmargin	Specifies the margin between the LED and the  childsite
# ledpos	Specifies the relative postion of the LED with respect
#			the childsite.
# ledvariable	Specifies a variable whose value is to be montiored.
# ledwidth	Specifies the width of the LED.
# offcolor	Specifies the color of the LED when it is off.
# oncolor	Specifies the color of the LED when it is on.
#
#
# PUBLIC METHODS: 
# alignleds	Aligns a list of widgets derived from the CLed class.
# cget		Standard cget method, returns the value of the specified
#			option.
# childsite	Returns the pathname of the childsite widget.
# component	Standard [ incr ] Tk component method.
# config	See configure
# configure	Standard Tk method, sets or returns options.
# isa		Sets the widget's class.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
#
# PURPOSE:
# Create an LED mega-widget with a childsite class.
#
#-
#***********************************************************************
#

#
# Default options.
#

option add *CLed.Led {0} widgetDefault
option add *CLed.onColor {green} widgetDefault
option add *CLed.offColor {grey} widgetDefault


#
# Usual options
#

itk::usual CLed {
    keep -background -cursor -foreground 
}


class CLed {
    inherit itk::Widget


    #
    # Option defintions.
    #

    itk_option define -colorassoc colorAssociation ColorAssociation {} 
    itk_option define -led led Led 0
    itk_option define -ledmargin ledMargin Margin 1 {
	positionLed
    }
    itk_option define -ledpos ledPos Posisiton w {
	positionLed
    }
    itk_option define -ledvariable ledVariable Variable ""
    itk_option define -offcolor offColor Background {}
    itk_option define -oncolor onColor Background {}


    #
    # Public methods and data
    #

    public method	constructor { args }
    public method	destructor {}
    public method	alignleds { args } {}    
    public method 	childsite { } {
	return $itk_component(lwchildsite)
    }


    #
    # Protected methods and data
    #

    protected method 	positionLed { {when later} }

    protected variable 	currentLedColor ""	;#current color of the LED 
    protected variable 	reposition "" 		;#non-null =>positionLed pending


    #
    # Private methods and data
    #

    private method	updateColor { name1 name2 op }

    private variable	ledVariable ""		;#variable being "traced"
    private variable	colors {}		;#list of colours 
    private variable	values {}		;#list of values assoicated
						 #with the colours.
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cLed
#
# INVOCATION:
# cLed pathName ?options?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) pathName  (windowPath)	A window path name for a nonexistent window.
# (>) options   (string)	One or more valid option/value pairs.
#
# The pathName and options are passed on to the CLed class.
#
# PROCEDURE VALUE:
# pathName - the window path name provided is returned.
#
# PURPOSE:
# To provide a lowercased access method for the CLed class.
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
# CLed
# CLed::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc ::cLed { 
    pathName 
    args 
} {
    uplevel ::CLed $pathName $args
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CLed:constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) args - Arguments to itk_initialize
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Constructor for the "CLed" class.	
#
# DESCRIPTION:
# 
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CLed::destructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body CLed::constructor {
    args
} {
    # 
    # Create the outermost frame to maintain geometry.
    #
 
   itk_component add shell {
        frame $itk_interior.shell
    } {
        keep -background -cursor
    }
    pack $itk_component(shell) -fill both -expand yes
    

    #
    # Create a frame for the childsite widget.
    #

    itk_component add lwchildsite {
        frame $itk_component(shell).lwchildsite
    } {
        keep -background -cursor
    }
    pack $itk_component(lwchildsite) -fill both -expand yes
    set itk_interior $itk_component(lwchildsite)


    #
    # Create LED.
    #

    itk_component add led {
	frame $itk_component(shell).led -height 14 -width 6 -relief sunken \
	    -borderwidth 2
    } {
	rename -background -ledcolor ledColor Background
	rename -width ledwidth ledWidth Width
    }
    pack $itk_component(led) -side left -padx 2

    
    #
    # Create margin between LED and the child site.
    #

    itk_component add ledmargin {
        frame $itk_component(shell).ledmargin
    } {
	keep -background -cursor
    }

    eval itk_initialize $args


    #
    # setup help bindings
    #

    foreach i [ $this component ] {
	bindtags [ $this component $i ] [ linsert  [ bindtags [ $this	\
	    component $i ] ] end $itk_component(hull) ]
    }

    
    # 
    # When idle, position the LED.
    #

    positionLed
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CLed::destructor
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
# Destructor for the "CLed" class.
#
# DESCRIPTION:
# Cancel the reposition of the LED if the command is pending.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CLed::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body CLed::destructor {} {
    if { $reposition != "" } { after cancel $reposition }

    if { [ uplevel #0 [list trace info variable $ledVariable] ] != "" } {
	   uplevel #0 [list trace remove variable $ledVariable {write unset} [ code $this updateColor ]]
    }
}

# ------------------------------------------------------------------
#                             OPTIONS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CLed::colorassoc
#
# INVOCATION:
# pathName configure -colorassoc ?value?
#	      OR
# cLed pathName -colorassoc value
#
# where pathName is a window path of a "CLed" widget and value a list
# of associations.  For example {{red hello} {black world}},
# indicates that when the variable specified by the -ledvariable option
# is "hello" the color of the LED is "red" and if its value is "world"
# the color of the LED is black.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# n.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies the color assocuation list.  What color is associated with
# what value
#
# For example 
#    {{red hello} {black world}},
# indicates that when the variable specified by the -ledvariable option
# is "hello" the color of the LED is "red" and if its value is "world"
# the color of the LED is black.
#
# DESCRIPTION:
# Set-up the list of colours specified and match them up with their
# associated value in the values list.  Then make sure the LED has
# the right colour.
#
# EXTERNAL VARIABLES:
# (list)   CLed::colors		list of colours 
# (list)   CLed::values		list of values associated with the colors.
# (string) CLed::ledVariable	variable whose value is being traced.
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

configbody	CLed::colorassoc {
    set colors {}
    set values {}
    foreach assoc $itk_option(-colorassoc) {
	lappend colors [ lindex $assoc 0 ]
	lappend values [ string toupper [ lindex $assoc 1 ] ]
    }

    if { $ledVariable != "" } {
	updateColor $ledVariable "" w
    } 
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CLed::led
#
# INVOCATION:
# pathName configure -led ?value?
#	      OR
# CLed pathName -led value
#
# where pathName is a window path of a "CLed" widget and value is either
# "on" or "off"
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# n.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies what state variable is in (on/off).
#
# DESCRIPTION:
# Toggles the value of the led, off or on.
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

configbody CLed::led {
    set vstate $itk_option(-led)

    if { "$vstate" == "toggle" } {
	if {"$currentLedColor" == "$itk_option(-oncolor)" } {
	    set color $itk_option(-offcolor)
	} else {
	    set color $itk_option(-oncolor)
	}	    
    } else {
	if { "$vstate" } {
	    set color $itk_option(-oncolor)
	} else {
	    set color $itk_option(-offcolor)
	}
    }

    $itk_component(led) configure -background $color
    set currentLedColor $color
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CLed::ledvariable
#
# INVOCATION:
# pathName configure -ledvariable ?value?
#	      OR
# CLed pathName -ledvariable value
# # where pathName is a window path of a "CLed" widget and value is 
# the name of any global variable
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# n.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies the variable whose value is to be monitored, together with
# the -colorassoc option these option can be used to automatically change
# the color of the LED given a particular value 
#
# DESCRIPTION:
# If we are already tracing changes in a variables value then stop
# tracing it.  Set-up a tracing on the new variable and make sure
# the LED is the right colour.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# Cled::colorassoc
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

configbody	CLed::ledvariable {

    set newVariable $itk_option(-ledvariable)
    if {  ( $newVariable == "" ) || ( $ledVariable  != $newVariable ) } {
	set traceInfo [ uplevel "#0" [list trace info variable $ledVariable] ]
	set found 0
	foreach traceItem $traceInfo {
	    if { "$this" == "[ lindex $traceItem 1 ] 2 ]" } {
		uplevel #0 [list trace remove variable $ledVariable {write unset} [ code $this updateColor ]]
		break
	    }
	}
    }

    set ledVariable $newVariable
    if { "$ledVariable" != "" } {
	set traceInfo [ uplevel #0 [list trace info variable $ledVariable] ]
	set found 0
	foreach traceItem $traceInfo {
	    if { "$this" ==  "[ lindex [ lindex $traceItem 1 ] 2 ]" } {
		set found 1
	    }
	}

	if {  [ isFalse $found ] } {
	    updateColor $ledVariable "" w
	    uplevel #0 [list trace add variable $ledVariable {write unset} [ code $this updateColor ]]
	}
    }
}

# ------------------------------------------------------------------
#                         PUBLIC METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# Cled::alignLeds
#
# INVOCATION:
# pathName alignLeds ledList
# 
# where pathName is a CLed window.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) ledList	(list)	List of widgets derived from the CLed class.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To align a list of widgets derived from the CLed class.  It adjusts
# the LED margin in order to line-up the LEDs.
#
# DESCRIPTION:
#
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
# Works best if the LED is positioned west of the childsite.  Does not
# work at all if the LED is position north, south, northwest, northeast,
# southwest or southeast of the childsite.
#-
#***********************************************************************
#

body CLed::alignleds {args} {
    update
    set maxLedWidth 0
    

    #
    # Verify that all the widgets are of type LED and determine
    # the size of the maximum length LED string.
    #

    foreach iwid $args {
        set objcmd [ find objects -isa Led *::$iwid ]
 
        if {$objcmd == ""} {
            error "$iwid is not a \"Led\""
        }
        
        set csWidth [ winfo reqwidth $iwid.shell.lwchildsite ]
        set shellWidth [ winfo reqwidth $iwid.shell ]
            
        if {[ expr $shellWidth - $csWidth ] > $maxLedWidth} {
            set maxLedWidth [ expr $shellWidth - $csWidth ]
        }

    }
    

    #
    # Adjust the margins for the LEDs such that the child sites and
    # LEDs line up.
    #

    foreach iwid $args {
        set csWidth [ winfo reqwidth $iwid.shell.lwchildsite ]
        set shellWidth [ winfo reqwidth $iwid.shell ]

        set ledWidth [ expr $shellWidth - $csWidth ]
        
        if {$maxLedWidth > $ledWidth} {
            set dist [ expr $maxLedWidth - \
                    ($ledWidth - [ winfo reqwidth $iwid.shell.labmargin ]) ]
            
            set objcmd [ find objects -isa Led *::$iwid ]
            $objcmd configure -ledmargin $dist 
        }

    }   
}

# ------------------------------------------------------------------
#                         PROTECTED METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# positionLed 
#
# INVOCATION:
# positonLed ?when?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) when 	(string)	When the reposition should be applied.
#			  	This parameter is optional.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Packs the LED and LED margin.  If "when" is "now", the change is
# applied immediately.  If it is "later" or it is not specified, then
# the change is applied later, when the application is idle.  The
# default value for "when" is "later"
#
# DESCRIPTION:
# Determine if the repositioning is to occur now or later.
# Based on the LEDs position use "pack configure" to rearrange the
#    relative positions of the LED, LED margin and the childsite.
#
# For example if the LEDs position is "w" then the LED is the
#   the west most widget, then the LED margin, and the chilsite
#   is on the east side.
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

body CLed::positionLed { {when later} } {
    #
    # Determine whe to reposition.
    #

    if {$when == "later"} {
        if {$reposition == ""} {
            set reposition [ after idle "[ code $this positionLed now ]" ]
        }
        return
    } elseif {$when != "now"} {
        error "bad option \"$when\": should be now or later"
    }
 
    
    #
    # Based on the LEDs position reposition the components
    #

    switch -- "$itk_option(-ledpos)" {
	nw {
	    pack configure $itk_component(lwchildsite) -side top
	    
	    $itk_component(ledmargin) configure -width 1 -height \
		[ winfo pixels $itk_component(ledmargin) \
                         $itk_option(-ledmargin) ]
	    pack configure $itk_component(ledmargin) -side top \
		-before $itk_component(lwchildsite)
	    pack configure $itk_component(led) -anchor w \
		-side top -before $itk_component(ledmargin)
	}
            
	n {
	    pack configure $itk_component(lwchildsite) -side top
                
	    $itk_component(ledmargin) configure -width 1 -height \
		[ winfo pixels $itk_component(ledmargin) \
		     $itk_option(-ledmargin) ]
	    pack configure $itk_component(ledmargin) -side top \
		-before $itk_component(lwchildsite)
	    pack configure $itk_component(led) -anchor center \
		-before $itk_component(ledmargin) -side top
	}
            
	ne {
	    pack configure $itk_component(lwchildsite) -side top
                
	    $itk_component(ledmargin) configure -width 1 -height \
		[ winfo pixels $itk_component(ledmargin) \
		     $itk_option(-ledmargin) ]
	    pack configure $itk_component(ledmargin) -side top \
		-before $itk_component(lwchildsite)
	    pack configure $itk_component(led) -anchor e \
		-side top -before $itk_component(ledmargin)
	}
            
	e {
	    pack configure $itk_component(lwchildsite) -side right
                
	    $itk_component(ledmargin) configure -height 1 -width \
		[ winfo pixels $itk_component(ledmargin) \
		     $itk_option(-ledmargin) ]
	    pack configure $itk_component(ledmargin) \
		-side right -before $itk_component(lwchildsite)
	    pack configure $itk_component(led) -anchor center \
		-side right -before $itk_component(ledmargin)
	}
            
	se {
	    pack configure $itk_component(lwchildsite) -side top
                
	    $itk_component(ledargin) configure -width 1 -height \
		[ winfo pixels $itk_component(ledmargin) \
		     $itk_option(-ledmargin) ]
	    pack configure $itk_component(ledmargin) \
		-side top -after $itk_component(lwchildsite)
	    pack configure $itk_component(led) -anchor e \
		-side bottom -after $itk_component(ledmargin)
	}
            
	s {
	    pack configure $itk_component(lwchildsite) -side top
	    
	    $itk_component(ledmargin) configure -width 1 -height \
		[ winfo pixels $itk_component(ledmargin) \
		     $itk_option(-ledmargin) ]
	    pack configure $itk_component(ledmargin) \
		-side top -after $itk_component(lwchildsite)
	    pack configure $itk_component(led) -anchor center \
		-side bottom -after $itk_component(ledmargin)
	}
            
	sw {
	    pack configure $itk_component(lwchildsite) -side top
                
	    $itk_component(ledmargin) configure -width 1 -height \
		[ winfo pixels $itk_component(ledmargin) \
		     $itk_option(-ledmargin) ]
	    pack configure $itk_component(ledmargin) \
		-side top -after $itk_component(lwchildsite)
	    pack configure $itk_component(led) -anchor w \
		-side bottom -after $itk_component(ledmargin)
	}
	
	w {
	    pack configure $itk_component(lwchildsite) -side right
	    $itk_component(ledmargin) configure -height 1 -width \
		[ winfo pixels $itk_component(ledmargin) \
		     $itk_option(-ledmargin) ]
	    pack configure $itk_component(ledmargin) \
		-side left -before $itk_component(lwchildsite)
	    pack configure $itk_component(led) -anchor center \
		-side left -before "$itk_component(ledmargin)"


        }
    }
 
    set reposition ""
}

# ------------------------------------------------------------------
#                         PRIVATE METHODS
# ------------------------------------------------------------------


#
#***********************************************************************
#+
# PROCEDURE NAME:
# CLed::updateColor
#
# INVOCATION:
# updateColor name1 name2 op
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name1	(string)	Name of the variable or associtive array
# (>) name2	(string)	Element of an associative array
# (>) op	(string)	Operation that was performed on the variable
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# To alter the color of the LED based on the new value in the
# variable
#
# DESCRIPTION:
# Make sure the variable that has been given is the one we are
# tracing.  If it is then use the values specified in the 
# colour/value assocaiation lists to set the colour of the LED.
#
# EXTERNAL VARIABLES:
# CLed::colors
# CLed::values
# CLed::ledVariable
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CLed::colorassoc
# CLed::textvariable
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CLed::updateColor {
    name1
    name2
    op
} {
    global ::dhsStatus ::$name1

    if { $name2 == "" } {
	set var "$name1"
    } else {
	set var "${name1}(${name2})"
    }
    if { "$var" != "$ledVariable" || [ uplevel \#0 info exists $var ] == 0 } {
	#
	# We aren't suppose to be monitoring this variable.
	# 
	return
    }


    if { [string match u* $op] } {
	#
	# The variable has been unset so we should no longer trace it
	#

	$this configure -ledvariable ""
    } elseif { [string match w* $op] } {
	#
	# Set the colour of the led according to the value of the variable.
	#

	set value [ uplevel \#0 set $var ]
	if { [ set pos [ lsearch -exact $values [ string toupper $value ] ] ] != -1 } {
	    $itk_component(led) configure -background [ lindex $colors $pos ]
	} elseif { [ set pos [ lsearch -exact $values "*" ] ] != -1 } {
	    $itk_component(led) configure -background [ lindex $colors $pos ]
	}
    }
}


