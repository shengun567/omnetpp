#==========================================================================
#  WIDGETS.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2003 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#



#===================================================================
#    STARTUP PROCEDURES
#===================================================================

# checkTclTkVersion --
#
# Check required Tcl/Tk version
#
proc checkTclTkVersion {} {

   global tk_version tk_patchLevel

   catch {package require Tk}
   if {$tk_version<8.0} {
      wm deiconify .
      wm title . "Bad news..."
      frame .f
      pack .f -expand 1 -fill both -padx 2 -pady 2
      label .f.l1 -text "Your version of Tcl/Tk is too old!"
      label .f.l2 -text "Tcl/Tk 8.0p1 or later required."
      button .f.b -text "OK" -command {exit}
      pack .f.l1 .f.l2 -side top -padx 5
      pack .f.b -side top -pady 5
      focus .f.b
      wm protocol . WM_DELETE_WINDOW {exit}
      tkwait variable ok
   } elseif {[string match "8.0.*" $tk_patchLevel]} {
      if {[string compare $tk_patchLevel "8.0.1"]<0} {
         tk_messageBox -title {Warning} -type ok -icon warning \
              -message {Old Tcl/Tk version. At least 8.0p1 is strongly recommended!}
      }
   } elseif {$tk_version==8.0 && [string compare $tk_patchLevel "8.0p1"]<0} {
      tk_messageBox -title {Warning} -type ok -icon warning \
           -message {Old Tcl/Tk version. At least 8.0p1 is strongly recommended!}
   }
}


# setupTkOptions --
#
# Sets up fonts and various options in Tk in order to have consistent look
# on Unix/Windows and across different Tk versions.
#
proc setupTkOptions {} {
   global fonts tcl_platform tk_version
   global tcl_wordchars tcl_nonwordchars

   catch {tcl_wordBreakAfter}; # work around Tcl bug: these vars got reset when words.tcl was autoloaded
   set tcl_wordchars {\w}
   set tcl_nonwordchars {\W}

   catch {
       # by default, undo/redo bindings are platform-specific -- change it:
       event add <<Undo>> <Control-Key-z>
       event add <<Undo>> <Control-Key-Z>
       event add <<Redo>> <Control-Key-y>
       event add <<Redo>> <Control-Key-Y>
   }
   
   #
   # fonts() array elements:
   #  normal:  menus, labels etc
   #  bold:    buttons
   #  big:     STOP button
   #  msgname: message name during animation
   #  fixed:   text windows and listboxes
   #

   if {$tcl_platform(platform) == "unix"} {
      set fonts(normal)  -Adobe-Helvetica-Medium-R-Normal-*-*-120-*-*-*-*-*-*
      set fonts(bold)    -Adobe-Helvetica-Bold-R-Normal-*-*-120-*-*-*-*-*-*
      set fonts(big)     -Adobe-Helvetica-Medium-R-Normal-*-*-180-*-*-*-*-*-*
      set fonts(msgname) -Adobe-Helvetica-Medium-R-Normal-*-*-120-*-*-*-*-*-*
      set fonts(fixed)   fixed
      set fonts(balloon) -Adobe-Helvetica-Medium-R-Normal-*-*-120-*-*-*-*-*-*
   } else {
      # Windows, Mac
      if {$tk_version<8.2} {
         set s 140
      } else {
         set s 110
      }
      font create opp_normal -family "MS Sans Serif" -size 8
      font create opp_bold   -family "MS Sans Serif" -size 8 -weight bold
      font create opp_balloon -family "MS Sans Serif" -size 8

      set fonts(normal)  opp_normal
      set fonts(bold)    opp_bold
      set fonts(big)     -Adobe-Helvetica-Medium-R-Normal-*-*-180-*-*-*-*-*-*
      set fonts(msgname) -Adobe-Helvetica-Medium-R-Normal-*-*-$s-*-*-*-*-*-*
      set fonts(fixed)   FixedSys
      set fonts(balloon) opp_balloon
   }

   if {$tcl_platform(platform) == "unix"} {
       option add *Scrollbar.width  12
       option add *Menubutton.font  $fonts(normal)
       option add *Menu.font        $fonts(normal)
       option add *Label.font       $fonts(normal)
       option add *Entry.font       $fonts(normal)
       option add *Listbox.font     $fonts(fixed)
       option add *Text.font        $fonts(fixed)
       option add *Button.font      $fonts(bold)

       # make menus look more contemporary
       menu .tmp
       set activebg [.tmp cget -activebackground]
       set activefg [.tmp cget -activeforeground]
       destroy .tmp
       option add *Menu.activeBorderWidth 0
       option add *Menu.relief raised
       option add *Menu.activeBackground #800000
       option add *Menu.activeForeground white
       option add *menubar.borderWidth 1
       option add *menubar.activeBorderWidth 1
       option add *menubar.activeBackground $activebg
       option add *menubar.activeForeground $activefg
   }
}


#===================================================================
#    UTILITY PROCEDURES
#===================================================================

# wsize --
#
# Utility to set a widget's size to exactly width x height pixels.
# Usage example:
#    button .b1 -text "OK"
#    pack [wsize .b1 40 40] -side top -expand 1 -fill both
#
proc wsize {w width height} {
  set f ${w}_f
  frame $f -width $width -height $height
  place $w -in $f -x 0 -y 0 -width $width -height $height
  raise $w
  return $f
}


#===================================================================
#    PROCEDURES FOR CREATING NEW 'WIDGET TYPES'
#===================================================================

package require combobox 1.0

proc iconbutton {w args} {
    global fonts

    if {$args=="-separator"} {
        # space between two buttons
        frame $w -height 1 -width 4
        #W2K: frame $w -height 23 -width 2 -bd 1 -relief groove
    } {
        # button
        eval button $w -bd 1 $args
        #W2K: eval button $w -bd 1 -relief flat $args
        #W2K: bind $w <Enter> [list $w config -relief raised]
        #W2K: bind $w <Leave> [list $w config -relief flat]
    }
    return $w
}

proc combo {w list {cmd {}}} {
    # implements a combo box widget (which is missing from Tk)
    # using a menubutton and a menu

    global fonts

    combobox::combobox $w
    foreach i $list {
        $w list insert end $i
    }
    catch {$w configure -value [lindex $list 0]}
    $w configure -command "$cmd ;#"
    return $w
}

proc comboconfig {w list {cmd {}}} {
    # reconfigures a combo box widget

    $w list delete 0 end
    foreach i $list {
        $w list insert end $i
    }
    if {[lsearch $list [$w cget -value]] == -1} {
        catch {$w configure -value [lindex $list 0]}
    }
    $w configure -command "$cmd ;#"
    return $w
}

proc combo-onchange {w cmd} {
    # Tk sucks: no event is triggered when entry content changes.
    # entry -validate is no better than bind <Key>,
    # vtrace is too error-prone.
    bind $w.entry <1> $cmd
    bind $w.entry <Key> $cmd
    bind $w.entry <FocusIn> $cmd
    bind $w.entry <FocusOut> $cmd
}

proc label-entry {w label {text {}}} {
    # utility function: create a frame with a label+entry
    frame $w
    label $w.l -anchor w -width 16 -text $label
    entry $w.e -highlightthickness 0
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e insert 0 $text
}

proc label-entry-chooser {w label text chooserproc} {
    # utility function: create a frame with a label+entry+button
    # the button is expected to call a dialog where the user can select
    # a value for the entry
    frame $w
    label $w.l -anchor w -width 16 -text $label
    entry $w.e -highlightthickness 0
    button $w.c -text " ... " -command [list chooser:choose $w.e $chooserproc]
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack [wsize $w.c 20 20] -anchor center -expand 0 -fill none -padx 2 -pady 2 -side right
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side left
    $w.e insert 0 $text
}

# private proc for label-entry-chooser
proc chooser:choose {w chooserproc} {
    set current [$w get]
    set new [eval $chooserproc \"$current\"]
    if {$new!=""} {
       $w delete 0 end
       $w insert end $new
    }
}

proc label-sunkenlabel {w label {text {}}} {
    # utility function: create a frame with a label+"readonly entry"
    frame $w
    label $w.l -anchor w -width 16 -text $label
    label $w.e -relief groove -justify left -anchor w
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e config -text $text
}

proc label-combo {w label list {text {}} {cmd {}}} {
    # utility function: create a frame with a label+combo
    frame $w
    label $w.l -anchor w -width 16 -text $label
    combo $w.e $list $cmd
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    if {$text != ""} {
         $w.e configure -value $text
    } else {
         $w.e configure -value [lindex $list 0]
    }
}

proc label-combo2 {w label list {text {}} {cmd {}}} {
    # start with empty combo box
    label-combo $w $label $list $text $cmd
    $w.e delete 0 end
}

proc label-text {w label height {text {}}} {
    # utility function: create a frame with a label+text
    frame $w
    label $w.l -anchor w -width 16 -text $label
    text $w.t -highlightthickness 0 -height $height -width 40
    catch {$w.t config -undo true}
    pack $w.l -anchor n -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.t -anchor center -expand 1 -fill both -padx 2 -pady 2 -side right
    $w.t insert 1.0 $text
}

proc label-scale {w label} {
    # utility function: create a frame with a label+scale
    frame $w
    label $w.l -anchor w -width 16 -text $label
    scale $w.e -orient horizontal
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
}

proc label-button {w label {text {}}} {
    # utility function: create a frame with label+button
    frame $w
    label $w.l -anchor w -width 16 -text $label
    button $w.e
    pack $w.l -anchor center -expand 0 -fill none -padx 2 -pady 2 -side left
    pack $w.e -anchor center -expand 1 -fill x -padx 2 -pady 2 -side right
    $w.e config -text $text
}

proc label-check {w label first var} {
    # utility function: create a frame with a label+radiobutton for choose
    global gned

    frame $w
    label $w.l -anchor w -width 16 -text $label
    frame $w.f
    checkbutton $w.f.r1 -text $first -variable $var

    pack $w.l -anchor w -expand 0 -fill none -side left
    pack $w.f -anchor w -expand 0 -side left -fill x
    pack $w.f.r1 -anchor w -expand 0 -side left
}

proc commentlabel {w text} {
    # utility function: create a frame with a message widget
    frame $w
    message $w.e -justify left -text $text -aspect 1000
    pack $w.e -anchor center -expand 0 -fill x -padx 2 -pady 2 -side left
}


# noteboook --
#
# Create 'tabbed notebook' widget
#
# Usage example:
#  notebook .x bottom
#  notebook_addpage .x p1 Egy
#  notebook_addpage .x p2 Ketto
#  notebook_addpage .x p3 Harom
#  pack .x -expand 1 -fill both
#  label .x.p1.e -text "One"
#  pack .x.p1.e
#
proc notebook {w {side top}} {
    #  utility function: create an empty notebook widget
    global nb
    set nb($w) ""

    frame $w
    frame $w.tabs
    pack $w.tabs -side $side -fill x
}

proc notebook_addpage {w name label} {
    #  utility function: add page to notebook widget
    set tab $w.tabs.$name
    set page $w.$name

    frame $page -border 2 -relief raised
    button $tab -text $label -command "notebook_showpage $w $name" -relief flat
    pack $tab -anchor n -expand 0 -fill none -side left

    global nb
    if {$nb($w)==""} {notebook_showpage $w $name}
}

proc notebook_showpage {w name} {
    #  notebook internal function
    global nb

    if {$nb($w)==$name} return

    pack $w.$name -expand 1 -fill both
    $w.tabs.$name config -relief raised

    if {$nb($w)!=""} {
       pack forget $w.$nb($w)
       $w.tabs.$nb($w) config -relief flat
    }
    set nb($w) $name
}


# vertResizeBar --
#
# Vertical 'resize bar' (divider)
#
proc vertResizeBar {w wToBeResized} {
    # create widget
    frame $w -width 5 -relief raised -borderwidth 1
    if [catch {$w config -cursor size_we}] {
      if [catch {$w config -cursor sb_h_double_arrow}] {
        catch {$w config -cursor sizing}
      }
    }

    # create bindings
    bind $w <Button-1> "vertResizeBar:buttonDown $w %X"
    bind $w <B1-Motion> "vertResizeBar:buttonMove %X"
    bind $w <ButtonRelease-1> "vertResizeBar:buttonRelease %X $wToBeResized"
    bind $w <Button-2> "catch {destroy .resizeBar}"
    bind $w <Button-3> "catch {destroy .resizeBar}"
}

proc vertResizeBar:buttonDown {w x} {
    global mouse
    set mouse(origx) $x

    catch {destroy .resizeBar}
    toplevel .resizeBar -relief flat -bg #606060
    wm overrideredirect .resizeBar true
    wm positionfrom .resizeBar program
    set geom "[winfo width $w]x[winfo height $w]+[winfo rootx $w]+[winfo rooty $w]"
    wm geometry .resizeBar $geom
}

proc vertResizeBar:buttonMove {x} {
    catch {wm geometry .resizeBar "+$x+[winfo rooty .resizeBar]"}
}

proc vertResizeBar:buttonRelease {x wToBeResized} {
    global mouse
    set dx [expr $x-$mouse(origx)]

    set width [$wToBeResized cget -width]
    set width [expr $width+$dx]
    $wToBeResized config -width $width

    catch {destroy .resizeBar}
}

# tableEdit --
#
# Create a "tableEdit" widget
#
# one $columnlist entry:
#   {title column-name command-to-create-widget-in-cell}
#
#  the command should use two variables:
#    $e - widget must be created with name stored in $e
#    $v - widget must be bound to variable whose name is in $v
#
# Example:
# tableEdit .t 20 {
#   {Name    name    {entry $e -textvariable $v -width 8 -bd 1 -relief sunken}}
#   {Value   value   {entry $e -textvariable $v -width 12 -bd 1 -relief sunken}}
#   {Comment comment {entry $e -textvariable $v -width 20 -bd 1 -relief sunken}}
# }
# pack .t -expand 1 -fill both
#
proc tableEdit {w numlines columnlist} {

    # clean up variables from earlier table instances with same name $w
    global tablePriv
    foreach i [array names tablePriv "$w,*"] {
        unset tablePriv($i)
    }

    # create widgets
    frame $w; # -bg green
    frame $w.tb -height 16
    canvas $w.c -yscrollcommand "$w.vsb set" -height 150 -bd 0
    scrollbar $w.vsb -command "$w.c yview"

    grid rowconfig $w 1 -weight 1 -minsize 0

    grid $w.tb -in $w -row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
    grid $w.c   -in $w -row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news
    grid $w.vsb -in $w -row 1 -column 1 -rowspan 1 -columnspan 1 -sticky news

    frame $w.c.f -bd 0
    $w.c create window 0 0 -anchor nw -window $w.c.f

    set tb $w.tb
    set f $w.c.f

    for {set li 0} {$li<$numlines} {incr li} {
       set col 0
       foreach entry $columnlist {
           # get fields from entry
           set title   [lindex $entry 0]
           set attr    [lindex $entry 1]
           set wcmd    [lindex $entry 2]

           # add table entry
           set e $f.li$li-$attr
           set v tablePriv($w,$li,$attr)
           eval $wcmd
           grid $e -in $f -row $li -column $col -rowspan 1 -columnspan 1 -sticky news

           # next column
           incr col
       }
    }

    update idletasks

    # create title labels
    set dx 2
    foreach entry $columnlist {
        # get fields from entry
        set title   [lindex $entry 0]
        set attr    [lindex $entry 1]

        set e $f.li0-$attr
        label $tb.$attr -bd 1 -relief raised -text $title

        # add title bar
        set width [expr [winfo width $e]]
        place $tb.$attr -in $tb -x $dx -y 0 -width $width -height [winfo height $tb]
        set dx [expr $dx + $width]
    }

    # adjust canvas width to frame width
    $w.c config -width [winfo width $f]
    $w.c config -scrollregion "0 0 0 [winfo height $f]"

    #focus $w.l0c0

}


# center --
#
# utility function: centers a dialog on the screen
#
proc center {w} {

    global tcl_platform

    # preliminary placement...
    if {[winfo reqwidth $w]!=0} {
       set pre_x [expr ([winfo screenwidth $w]-[winfo reqwidth $w])/2-[winfo vrootx [winfo parent $w]]]
       set pre_y [expr ([winfo screenheight $w]-[winfo reqheight $w])/2-[winfo vrooty [winfo parent $w]]]
       wm geom $w +$pre_x+$pre_y
    }

    # withdraw the window, then update all the geometry information
    # so we know how big it wants to be, then center the window in the
    # display and de-iconify it.
    if {$tcl_platform(platform) != "windows"} {
        wm withdraw $w
    }
    update idletasks
    set x [expr [winfo screenwidth $w]/2 - [winfo width $w]/2  - [winfo vrootx [winfo parent $w]]]
    set y [expr [winfo screenheight $w]/2 - [winfo height $w]/2  - [winfo vrooty [winfo parent $w]]]
    wm geom $w +$x+$y
    if {$tcl_platform(platform) != "windows"} {
        wm deiconify $w
    }
    focus $w
}


# createOkCancelDialog --
#
# creates dialog with OK and Cancel buttons
# user's widgets can go into frame $w.f
#
proc createOkCancelDialog {w title} {
    global tk_version tcl_platform

    catch {destroy $w}
    toplevel $w -class Toplevel
    if {$tk_version<8.2 || $tcl_platform(platform)!="windows"} {
        wm transient $w [winfo toplevel [winfo parent $w]]
    }
    wm title $w $title
    wm iconname $w Dialog
    wm focusmodel $w passive
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm deiconify $w
    wm protocol $w WM_DELETE_WINDOW { }

    # preliminary placement (assumes 350x250 dialog)...
    set pre_x [expr ([winfo screenwidth $w]-350)/2-[winfo vrootx [winfo parent $w]]]
    set pre_y [expr ([winfo screenheight $w]-250)/2-[winfo vrooty [winfo parent $w]]]
    wm geom $w +$pre_x+$pre_y

    frame $w.f
    frame $w.buttons
    button $w.buttons.okbutton  -text {  OK  }
    button $w.buttons.cancelbutton  -text {Cancel}

    pack $w.buttons -expand 0 -fill x -padx 5 -pady 5 -side bottom
    pack $w.f -expand 1 -fill both -padx 5 -pady 5 -side top
    pack $w.buttons.cancelbutton  -anchor n -side right -padx 2
    pack $w.buttons.okbutton  -anchor n -side right -padx 2
}


# execOkCancelDialog --
#
# Executes the dialog.
# Optional validating proc may check if fields are correctly
# filled in -- it should return 1 if dialog contents is valid,
# 0 if there are invalid fields and OK button should not be
# accepted.
#
# Returns 1 if Ok was pressed, 0 if Cancel was pressed.
#
proc execOkCancelDialog {w {validating_proc {}}} {

    global opp

    $w.buttons.okbutton configure -command "set opp($w) 1"
    catch {$w.buttons.cancelbutton configure -command "set opp($w) 0"}

    bind $w <Return> "if {\[winfo class \[focus\]\]!=\"Text\"} {set opp($w) 1}"
    bind $w <Escape> "set opp($w) 0"

    wm protocol $w WM_DELETE_WINDOW "set opp($w) 0"

    # next line mysteriously solves "lost focus" problem of popup dialogs...
    after 1 "wm deiconify $w"

    center $w

    set oldGrab [grab current $w]
    if {$oldGrab != ""} {
        set grabStatus [grab status $oldGrab]
    }
    grab $w

    # Wait for the user to respond, then restore the focus and
    # return the index of the selected button.  Restore the focus
    # before deleting the window, since otherwise the window manager
    # may take the focus away so we can't redirect it.  Finally,
    # restore any grab that was in effect.

    if {$validating_proc==""} {
    tkwait variable opp($w)
    } else {
        tkwait variable opp($w)
        while {$opp($w)==1 && ![eval $validating_proc $w]} {
            tkwait variable opp($w)
        }
    }

    if {$oldGrab != ""} {
        if {$grabStatus == "global"} {
            grab -global $oldGrab
        } else {
            grab $oldGrab
        }
    }
    return $opp($w)
}

# createCloseDialog --
#
# Creates dialog with a Close button.
# User's widgets can go into frame $w.f, and extra buttons can go into frame $w.buttons.
#
proc createCloseDialog {w title} {
    global tk_version tcl_platform

    catch {destroy $w}
    toplevel $w -class Toplevel
    if {$tk_version<8.2 || $tcl_platform(platform)!="windows"} {
        wm transient $w [winfo toplevel [winfo parent $w]]
    }
    wm title $w $title
    wm iconname $w Dialog
    wm focusmodel $w passive
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm deiconify $w
    wm protocol $w WM_DELETE_WINDOW { }

    # preliminary placement (assumes 350x250 dialog)...
    set pre_x [expr ([winfo screenwidth $w]-350)/2-[winfo vrootx [winfo parent $w]]]
    set pre_y [expr ([winfo screenheight $w]-250)/2-[winfo vrooty [winfo parent $w]]]
    wm geom $w +$pre_x+$pre_y

    frame $w.f
    frame $w.buttons
    button $w.buttons.closebutton  -text {Close}

    pack $w.buttons -expand 0 -fill x -padx 5 -pady 5 -side bottom
    pack $w.f -expand 1 -fill both -padx 5 -pady 5 -side top
    pack $w.buttons.closebutton  -anchor n -side right -padx 2
}


# execCloseDialog --
#
# Executes the dialog.
#
proc execCloseDialog w {

    global opp

    $w.buttons.closebutton configure -command "set opp($w) 1"

    #bind $w <Return> "if {\[winfo class \[focus\]\]!=\"Text\"} {set opp($w) 1}"
    bind $w <Escape> "set opp($w) 0"

    wm protocol $w WM_DELETE_WINDOW "set opp($w) 0"

    # next line mysteriously solves "lost focus" problem of popup dialogs...
    after 1 "wm deiconify $w"

    center $w

    set oldGrab [grab current $w]
    if {$oldGrab != ""} {
        set grabStatus [grab status $oldGrab]
    }
    grab $w

    # Wait for the user to respond, then restore the focus and
    # return the index of the selected button.  Restore the focus
    # before deleting the window, since otherwise the window manager
    # may take the focus away so we can't redirect it.  Finally,
    # restore any grab that was in effect.

    tkwait variable opp($w)

    if {$oldGrab != ""} {
        if {$grabStatus == "global"} {
            grab -global $oldGrab
        } else {
            grab $oldGrab
        }
    }
    return $opp($w)
}


# aboutDialog --
#
# Displays an About dialog.
#
proc aboutDialog {title contents} {
    catch {destroy .about}
    createOkCancelDialog .about $title
    label .about.f.l -text $contents
    pack .about.f.l -expand 1 -fill both
    destroy .about.buttons.cancelbutton
    execOkCancelDialog .about
    destroy .about
}


