FRACTAL OBSERVER
---------------------------------------------------------------------------------------------------------------------------

EXECUTE OPTIONS
/dev/ttyACM0   set serial port location   ( /dev/ttyACM0 by default )
--set-(param)  set parameters at the beginning
--put-(param)  'number(s)' to set parameters with execution (if param type is double, end with '-ok')
--help         to see all usable keys

examples:  ./Fractal-observer /dev/ttyACM1 --set              (specify serial link for Nucleo and set all parameters)
           ./Fractal-observer                                 (default settings)
		   ./Fractal-observer --help                          (display whole help)

list of key options:


MAIN OPTIONS
'a'   -    abort
's'   -    set computation parameters
'1'   -    start computing on nucleo
'b'   -    change baud rate 
'c'   -    compute fractal on pc
'p'   -    force gui update
'l'   -    clear canvas
'm'   -    animate
'i'   -    capture screenshot
'r'   -    reset chunk id
'h'   -    help
'+'   -    increase size of window
'-'   -    decrease size of window
'o'   -    enter parameters set menu
'q'   -    quit

to zoom scroll with mouse (slowly, event queue takes every little scroll)
------------------------
you can increase size of window even with mous, but when fractal computed 
it takes a bit of time for it to compute new one when resized this way
------------------------

PARAMETERS MENU OPTIONS:
'c'   -    change complex number C 
'r'   -    change range
'b'   -    go back to main

COMPLEX AND RANGE CHANGE MENU OPTIONS:
COMPLEX:
'+'   -    increment real part of C
'1'   -    increment imaginary part of C
'-'   -    decrement real part of C
'0'   -    decrement imaginary part of C
RANGE:
'+'   -    zoom in
'-'   -    zoom out
BOTH:
'w'   -    move up
'a'   -    move left
's'   -    move down
'd'   -    move right
'v'   -    display actual C and range values
'o'   -    save changes and go back

you can display all of those options when you click 'h' or
when you execute application with --help you can even see all executing parameters options
------------------------------------------------------------------------------------
All rights reserved 
Fractal observer™ ©2021


