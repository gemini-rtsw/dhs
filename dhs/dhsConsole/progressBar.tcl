# $Id: progressBar.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
#
#********************************************************************** 
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
# dhsConsole/src/progressBar.itk
#
# PURPOSE:
# To create a progress bar widget, another name for this widget
# would be thermometer.
#
# CLASS NAME(S)
# CProgressBar			: Displays a bar that can grdually "progress"
#				  or act like a thermometer widget.
#
# METHOD NAME(S)
# PUBLIC
# CProgressBar::constructor	: Constructor for the CProgressBar class.
# CProgressBar::destructor	: Destructor for the CProgressBar class.
# CProgressBar::doSomething	: Makes the progress bar look like something
#				  is happening, graually increases its value.
# CProgressBar::lookBusy	: Turns the graually increasing off and on.
# CProgressBar::reset		: Resets the bar back to "0" or empty.
#
# PROTECTED
# CProgressBar::repackValues	: Realigns the position of the values with
#				  respect to the slider.
#
# GLOBAL PROCEDURE(S)
# cProgressBar			: lower case access method to the
#				  CProgressBar class.
#
#
# $Log: progressBar.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:09  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.9  1998/07/16 18:54:16  jaeger
# Properly cancelled the "after" script.
#
# Revision 1.8  1998/07/16 18:15:10  jaeger
# Made inheritance fully specified.
#
# Revision 1.7  1998/06/22 17:40:22  jaeger
# Updated code documentation to bring it up to Gemini standards.  Moved
# some of the option and method definitions into the class definition
# since they were trivial.
#
# Revision 1.6  1998/06/22 17:34:07  jaeger
# Updated code do
#
# Revision 1.5  1998/05/15 16:49:42  jaeger
# Changed "update" to "update idletasks"
#
# Revision 1.4  1998/04/14 19:19:19  jaeger
# Fixed after command.
#
# Revision 1.3  1998/03/06 17:49:02  jaeger
# Added idlecolor and busycolor configbodies
#
# Revision 1.2  1997/11/13 23:53:19  jaeger
# Checked in for install
#
#
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#***********************************************************************
#

#
#***********************************************************************
#+
# CLASS NAME:
# CProgressBar
#
# OPTION(S)
# background	Specifies the background color of the shell.
# busycolor	Specifies slider color when busy.
# clientdata	A hook that clients can use to store data with each widget.
# cursor	Specifies the mouse cursor to be used.
# digits	Specifies the number of significant digits.
# font		Specifies the text font to use.
# foreground    Specifies the normal foreground color.
# from		Specifies the starting value.
# idlecolor	Specifies slider color when idle.
# labelbitmap	Specifies the label's bitmap.
# labelfont     Specifies the label's font.
# labelimage.   Specifies the label's image.
# labelmargin	Specifies the margin between the label and the  childsite
# labelpos	Specifies the relative postion of the label with respect
#			the childsite.
# labeltext     Specifies the text that appears in the label.
# labelvariable	Specifies the text variable of the label around the childsite.
# length	Specifies the desired long dimension of the progress bar.
# orient	Put progress bar in either horizont or vertical position.
# showvalue	Specifies a boolean value indicating whether the 
#		value are shown.			
# sliderlength  Specfies the size of the slider, measured in screen units.
# speed		Specifies the rate in which the value is increased
#		     when busy.
# to		Specifies the real end value .
# value		Specifies the slider position or the current value.
# valueforeground
#		Specifies the normal foreground color of the value.
# variable	Specifies the name of a global variable to
#		   link with the progress bar, the value changes
#		   whenever the variables value changes.
# width		Specifies the desired narrow dimention of the trough.
# 
#
# PUBLIC METHODS: 
# cget		Standard cget method, returns the value of the specified
#			option.
# component	Standard [incr] Tk component method.
# config	See configure
# configure	Standard Tk method, sets or returns options.
# doSomething	Make the progress bar do something when we are
#			looking busy.
# isa		Sets the widget's class.
# lookBusy	Make the progess bar portray activity
# reset		Set the value back to "to" and change the label's text.
# 
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# This class creates a progress bar widget, displays a message
# and a scale indicating that work is in progress.
#
# This widget can also be used as a thermometer.
#
#-
#***********************************************************************
#


#
# Default option values.
#

option add *CProgressBar.busyColor blue widgetDefaul
option add *CProgressBar.from 0 widgetDefault
option add *CProgressBar.idleColor grey widgetDefaul
option add *CProgressBar.labelMargin 2 widgetDefault
option add *CProgressBar.orient horizontal widgetDefault
option add *CProgressBar.state busy widgetDefault
option add *CProgressBar.to 1 widgetDefault
option add *CProgressBar.value 0 widgetDefault
option add *CProgressBar.width 15 widgetDefault


#
# Ususal options
#

itk::usual CProgressBar {
    keep -background 							\
	-borderwidth							\
	-cursor 							\
	-foreground -highlightcolor 					\
	-highlightthickness 
}


#
# The class definition.
#

class CProgressBar {
    inherit iwidgets::Labeledwidget

    #
    # Option definitions.
    #

    itk_option define -busycolor busyColor Color blue  {
	if { $itk_option(-state) == "busy" } {
	    $itk_component(canvas) itemconfig $bar -fill $itk_option(-busycolor)
	}
    }

    itk_option define -from from From 0 {
      $itk_component(from) configure 				\
          -text "[ lindex [ split $itk_option(-from) . ] 0 ]"
      repackValues
    }

    itk_option define -idlecolor idleColor Color grey80 {
      if { $itk_option(-state) == "idle" } {
          $itk_component(canvas) itemconfig $bar -fill $itk_option(-idlecolor)
      }
   }

    itk_option define -length length Length 1

    itk_option define -orient orient Orient horizontal {
      repackValues
    }

    itk_option define -showvalue showValue ShowValue 1 {
	if { $itk_option(-showvalue) } {
	    pack $itk_component(valueFrame) -side bottom -fill x -expand yes
	} else {
	    pack forget $itk_component(valueFrame)
	}
    }


    itk_option define -state state State busy

    itk_option define -to to To 1 {
	$itk_component(to) configure 				\
	    -text "[ lindex [ split $itk_option(-to) . ] 0  ]"
	repackValues
    }

    itk_option define -value value Value 0 {
       repackValues
    }
    itk_option define -width width Width 15
    itk_option define -variable variable Variable ""
    #
    # speed in ms/tick when looking busy
    #

    itk_option define -speed speed Speed 250


    #
    # Public methods and data
    #

    public method constructor {args} 
    public method destructor {} {
	#
	# Cancel the updating of the slider.
	#
	
	if { $doingSomething != "" } {
	    after cancel $doingSomething
	}
    }
    public method doSomething {}
    public method lookBusy { { bool 1 } }


    #
    # Reset the bar so it is "empty".
    #

    public method reset { { text "" } } {
	lookBusy 0
	$this config -labeltext "$text"
    }


    #
    # Protected methods and data
    #

    protected method repackValues {}

    protected variable color {grey80}	;# current bar color
    protected variable lookingBusy 0	;# true if currently looking busy
    protected variable inc 1    	;# controls direction for animation


    #
    # Private methods and data
    #

    private variable doingSomething ""	;# Result of the "doSomething"
					 # after script.
   private variable bar
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CProgressBar::constructor
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
# Constructor for the "CProgressBar" class.
#
# DESCRIPTION:
# ???Description of the algorithm.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CProgressBar::destructor
# CProgressBar
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CProgressBar::constructor { 
    args 
} {
    component hull configure -borderwidth 0

    
    #
    # Create the scale portion.
    #

    set childsite [ $this childsite ]
   itk_component add canvas {
      canvas $childsite.canvas  \
         -borderwidth 1         \
         -relief sunken
   }

    pack $itk_component(canvas) -expand yes -fill x  	\
	-padx 2 -pady 2
   set bar [ $itk_component(canvas) create \
      rectangle 0 0 [winfo width $itk_component(canvas)] [winfo height $itk_component(canvas)] -fill blue \
   ]

    #
    # Create a frame for displaying the value, below the scale widget.
    # This show the "to", "from" and current value all below the scale
    # widget.
    #

    itk_component add valueFrame {
	frame $childsite.valueFrame
    } {
	keep -background -cursor
    }
    pack $itk_component(valueFrame) -side bottom -fill x -expand yes


    #
    # The from value.
    #

    itk_component add from {
	label $itk_component(valueFrame).from 				\
	    -text ""
    } {
	keep -background -cursor -foreground
    }
    pack $itk_component(from) -anchor w -side left

    
    #
    # Padding between the "from" value and the current value.
    #

    itk_component add fvFrame {
	frame $itk_component(valueFrame).fvFrame -width 0 -height 0
    } {
	keep -background -cursor
    }
    pack $itk_component(fvFrame) -anchor w -side left


    #
    # The current value.
    # 

    itk_component add value {
	label $itk_component(valueFrame).value				\
	    -text ""
    } {
	keep -background -cursor 
	rename -foreground -valueforeground valueForeground Foreground
    }
    $itk_component(value) configure -foreground green
    pack $itk_component(value) -side left


    #
    # The "to" value.
    # 

    itk_component add to {
	label $itk_component(valueFrame).to 				\
	    -text ""
    } {
	keep -background -cursor -foreground
    }
    pack $itk_component(to) -side right -anchor e 

    #
    # Padding between the current value and "to" value.
    # 

    itk_component add vtFrame {
	frame $itk_component(valueFrame).vtFrame -width 0 -height 0
    } {
	keep -background -cursor
    }
    pack $itk_component(vtFrame) -side right -anchor e


    #
    # Make sure all bindings applied to this widget are applied
    # to all of its components.
    #

    foreach i [$this component] {
	bindtags [$this component $i] [ linsert  [ bindtags [ $this	\
		component $i ] ] end $itk_component(hull) ]
    }

    eval itk_initialize $args
   
   bind $itk_component(canvas)  <Configure> [itcl::code $this repackValues]
}



#
#***********************************************************************
#+
# PROCEDURE NAME:
# cProgressBar
#
# INVOCATION:
# cProgressBar pathName ?options?
#
# (>) pathName  (windowPath)    A window path name for a nonexistent window.
# (>) options   (string)        One or more valid option/value pairs.
#
# The pathName and options are passed on to the LED class.
#
# PROCEDURE VALUE:
# pathName - the window path name provided is returned.
#
# PURPOSE:
# To provide a lower cased access method for the Led class.
#
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# None.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CProgressBar::constructor
# CProgressBar::destructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc 	cProgressBar {
     pathName 
     args
} {
    uplevel CProgressBar $pathName $args
}

# ------------------------------------------------------------------
#                             OPTIONS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CProgressBar::length
#
# INVOCATION:
# pathName configure -length ?value?
#             OR
# CProgressBar pathName -length value
#
# where pathName is a window path of a "CProgressBar" widget and value 
# is a pixel value.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# n.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies the length of the progress bar.
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

configbody 	CProgressBar::length {
    if { $itk_option(-length) < 0 } {
	$this config -length 0
    } else {
       if { $itk_option(-orient) eq "horizontal" } {
      	$itk_component(canvas) config -width $itk_option(-length)
      	$itk_component(valueFrame) config -width $itk_option(-length)
       } else  {
         $itk_component(canvas) config -height $itk_option(-length)
         $itk_component(valueFrame) config -height $itk_option(-length)
       }
       repackValues
    }  
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CProgressBar::state
#
# INVOCATION:
#  pathName configure -state ?value?
#             OR
# CProgressBar pathName -state value
#
# where pathName is a window path of a "CProgressBar" widget and value 
# is either "busy" or "idle"
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# n.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies whether the progress bar is "busy" or "idle", the trough
# color of the scale is set to "busycolor" and "idlecolor" respectively.
#
# DESCRIPTION:
# Change the color of the slider depending on the state value given.
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

configbody	CProgressBar::state {
   if { $itk_option(-state) == "busy" } {
      $itk_component(canvas) itemconfig $bar -fill $itk_option(-busycolor)
    } elseif { $itk_option(-state) == "idle" } {
      $itk_component(canvas) itemconfig $bar -fill $itk_option(-idlecolor)
    } else {
   	error "Unknown state \"$itk_option(-state)\": should be one of\
   		\"busy\" or \"idle\"."
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CProgressBar::value
#
# INVOCATION:
# pathName configure -value ?value?
#             OR
# CProgressBar pathName -value value
#
# where pathName is a window path of a "CProgressBar" widget and value 
# is a numeric value.
# 
# PARAMETERS: (">" input, "!" modified, "<" output)
# n.a.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# Specifies the current value of the progress bar.
#
# DESCRIPTION:
# Make sure the new value is between the "from" and "to" values.  If it
# is set the scale widgets value to the new value.  Otherwise use the
# "from" or "to" value, which ever is appropriate.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CProgressBar::from
# ProgressBar::showvalue
# CProgressBar::to
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

configbody 	CProgressBar::value {
    if { ( $itk_option(-to) > $itk_option(-from) ) &&			\
	     ( $itk_option(-value) > $itk_option(-to) ) } {
	#
	# value is never more than "to so set it to the "to" value" 
	#

	$this config -value $itk_option(-to)
    } elseif { ( $itk_option(-to) <= $itk_option(-from) ) && 		\
		   ( $itk_option(-value) < $itk_option(-to) ) } {
	#
	# value is never more than "to" so set it to the "to" value.
	#

	$this config -value $itk_option(-to)
    } elseif { ( $itk_option(-from) > $itk_option(-to) ) &&		\
	     ( $itk_option(-value) > $itk_option(-from) ) } {
	#
	# value is never less than "from" so set it to the "from" value.
	#

	$this config -value $itk_option(-from)
    } elseif { ( $itk_option(-from) <= $itk_option(-to) ) && 		\
		   ( $itk_option(-value) < $itk_option(-from) ) } {
	#
	# value is never less than "from: so set it to the "from" value.
	#

	$this config -value $itk_option(-from)
    } else {
   	$itk_component(value) config -text $itk_option(-value)
   
   	repackValues
    }
}

# ------------------------------------------------------------------
#                          PUBLIC METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CProgressBar::doSomething
#
# INVOCATION:
# pathName doSomething
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To do something in order to look busy.
#
# DESCRIPTION:
# If the "to value" is less than the "from value" then decrement the 
# "to value", otherwise increment the "to value".  Then call
# "doSomething" again.
#
# EXTERNAL VARIABLES:
# CProgressBar::lookingBusy 
# CProgressBar::inc
# 
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CProgressBar::lookBusy 
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body CProgressBar::doSomething {
} {
    if { $lookingBusy } {
	if { $itk_option(-from) <= $itk_option(-to)  
	     && $itk_option(-value) < $itk_option(-to) } {
	    set inc 1;
	} elseif { $itk_option(-from) > $itk_option(-to)  
	     && $itk_option(-value) > $itk_option(-to) } {
	    set inc -1;
	} else {
	    set inc 0;
	}
	$this configure -value [ incr itk_option(-value) $inc ]
	update idletasks
	set doingSomething [ after $itk_option(-speed) 			\
				 "[ code $this doSomething ]" ]
    } 
    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CProgressBar::lookBusy
#
# INVOCATION:
# pathName lookBusy ?booleanValue?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) boolValue (boolean) If true then the slider length is greater than
#	0, otherwise it is 0
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Start (or stop) some animation inorder to indicate that work is (not)
# in progress.  This will continue until the next reset or until
# -to is set with config.
#
# DESCRIPTION:
# If we are to look busy then set the color of the bar to the busy color
# and call "doSomething" to gradually increase the bar.  Conversely if
# we are to look idle then change the color to the idle color and 
# stop the bar from increasing.
#
# EXTERNAL VARIABLES:
# CProgressBar::lookingBusy
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

body 	CProgressBar::lookBusy {
    { bool 1 }
} {
#    $itk_component(scale) set [ set itk_option(-value) 0 ]
#    $itk_component(scale) config 					\
#	-from $itk_option(-from)					\
#	-to $itk_option(-to)						\
#	-troughcolor $itk_option(-idlecolor)

    if { [ set lookingBusy $bool ] } {
	set itk_option(-value) 0
#	$itk_component(scale) config \
#	    -background $itk_option(-busycolor)
	doSomething
    } else {
#	$itk_component(scale) config \
#	    -background $itk_option(-idlecolor)
	if { $itk_option(-value) != $itk_option(-to) && 
	     $doingSomething != "" } {
	    after cancel $doingSomething
	}
    }
}

# ------------------------------------------------------------------
#                       PROTECTED METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CProgressBar::repackValues
#
# INVOCATION:
# repackValues
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To repack the "to", current value and "from" values so that they
# are all visible, and the position of the current value matches the
# position of the progress bar.
#
# DESCRIPTION:
# If orientation is "horizontal" then make sure the values appear to
# below the slider.  Conversly if the orentation is "vertical" then
# make sure the values apper to the right of the slider.
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

body CProgressBar::repackValues {} {
    if { ![ winfo viewable $itk_component(hull) ] } {
	update idletasks
    }

   if { $itk_option(-orient) == "horizontal" } {
      $itk_component(canvas) configure -height 15
      if { $itk_option(-to) != $itk_option(-from) } {
         set extension [ expr "[winfo width $itk_component(canvas)] * ( double($itk_option(-value)) - double($itk_option(-from)) ) / ( double($itk_option(-to)) - double($itk_option(-from)) )" ]
      } else {
         set extension [ winfo width $itk_component(canvas) ]
      }
      $itk_component(canvas) coords $bar 0 0 $extension [winfo height $itk_component(canvas)]
   } else {
      $itk_component(canvas) configure -width 15
      if { $itk_option(-to) != $itk_option(-from) } {
         set extension [ expr "[winfo height $itk_component(canvas)] * ( double($itk_option(-value)) - double($itk_option(-from)) ) / ( double($itk_option(-to)) - double($itk_option(-from)) )" ]
      } else {
         set extension [ winfo height $itk_component(canvas) ]
      }
      $itk_component(canvas) coords $bar 0 0  \
                                       [winfo width $itk_component(canvas)] $extension
   }
   if { ! $itk_option(-showvalue) } { 
      return
   }


    if { $itk_option(-orient) == "horizontal" } {
	#
	# Make sure the scale and values are position horizontally
	#
   
   
	pack configure $itk_component(canvas) -side top -anchor n -fill x
	pack configure $itk_component(valueFrame) -side top -anchor n	\
	    -after $itk_component(canvas) -fill x

	
	#
	# Find the width of the scale widget and the various bits
	# of the value frame.
	#

	set totalWidth [ winfo reqwidth $itk_component(canvas) ]
	set valueWidth [ winfo width $itk_component(value) ]
	set fromWidth [ winfo width $itk_component(from) ]
	set toWidth [ winfo width $itk_component(to) ]


	#
	# Find the relative position of the scales slider
	# with respect to the windows size.
	#

	set range [ expr abs( $itk_option(-to) - $itk_option(-from) ) ]
	if { $range == 0 } {
	    set location 0
	} else {
	    set location [ expr ( $totalWidth * 			\
	            $itk_option(-value) ) / $range ]
	} 


	#
	#  Set the width of the fromValue frame so that the value
	# is positioned below the slider.
	#

	set fvWidth [ expr $location - ( $valueWidth / 2 ) - $fromWidth ]
	if { $fvWidth < 0 } {
	    set vtWidth [ expr $totalWidth - $toWidth - $valueWidth 	\
			      - $fromWidth ]
	} else {
	    set vtWidth [ expr $totalWidth - $fvWidth - $toWidth 	\
			      - $valueWidth - $fromWidth ]
	}

	
	#
	#  Position all the values.
	#

	pack configure $itk_component(from) -side left -anchor w	
	pack configure $itk_component(fvFrame) -side left  -anchor w	\
	    -after $itk_component(from)
	pack configure $itk_component(to) -side right -anchor e
	if { $fvWidth <= 0 } {
	    $itk_component(fvFrame) config -width 2p -height 0
	    pack configure $itk_component(value) -side left -anchor w 	\
		-after $itk_component(fvFrame)
	} elseif { $vtWidth <= 0 } {
	    $itk_component(vtFrame) config -width 2p -height 0
	    pack configure $itk_component(value) -side right -anchor e 	\
		-after $itk_component(vtFrame)
	} else {
	    pack configure $itk_component(value) -side left -anchor w 	\
		-after $itk_component(fvFrame)
	    $itk_component(fvFrame) config -width "${fvWidth}p"
	}
    } elseif { $itk_option(-orient) == "vertical" } {
	#
	# Make sure the scale and values are situated in 
	# a vertical manner
	#

	pack configure $itk_component(canvas) -side right -anchor w -fill y
	pack configure $itk_component(valueFrame) -side right -anchor w	\
	    -after $itk_component(canvas) -fill y

	
	#
	# Find the height of the scale and all of the values
	#

	set totalHeight [ winfo reqheight $itk_component(canvas) ]
	set valueHeight [ winfo height $itk_component(value) ]
	set fromHeight [ winfo height $itk_component(from) ]
	set toHeight [ winfo height $itk_component(to) ]

	
	#
	# Find the relative place for the value, should be
	# the same place as the "slider" position
	# 

	set range [ expr abs( $itk_option(-from) -  $itk_option(-to)  ) ]
	if { $range == 0 } {
	    set location 0
	} else {
	    set location [ expr ( $totalHeight * 			\
	            $itk_option(-value) ) / $range ]
	} 

	#
	# Caluculate the relative height of the frameValue frame
	# this will make the "value" move.
	#

	set fvHeight [ expr $location - ( $valueHeight / 2 ) - 		\
		       $fromHeight ]
	if { $fvHeight < 0 } {
	    set vtHeight [ expr $totalHeight - $toHeight - 		\
			   $valueHeight  - $fromHeight ]
	} else {
	    set vtHeight [ expr $totalHeight - $fvHeight - $toHeight 	\
			      - $valueHeight - $fromHeight ]
	}


	#
	# Pack the various widget appropriately and change the
	# height of the vfFrame.
	#

	pack configure $itk_component(from) -side top -anchor n	
	pack configure $itk_component(fvFrame) -side top -anchor n	\
	    -after $itk_component(from)
	pack configure $itk_component(to) -side bottom -anchor s

	if { $fvHeight <= 0 } {
	    $itk_component(fvFrame) config -height 2p -width 0
	    pack configure $itk_component(value) -side top -anchor n 	\
		-after $itk_component(fvFrame)
	} elseif { $vtHeight <= 0 } {
	    $itk_component(vtFrame) config -height 2p -width 0
	    pack configure $itk_component(value) -side bottom -anchor s \
		-after $itk_component(vtFrame)
	} else {
	    pack configure $itk_component(value) -side top -anchor n 	\
		-after $itk_component(fvFrame)
	    $itk_component(fvFrame) config -height "${fvHeight}p" 	\
		-width 0
	}
    } else {
	error "Option \"-orient\" has a bad value: $itk_option(-orient)"
    }
}