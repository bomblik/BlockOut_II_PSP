Compilation
===========

In order to compile BlockOut II for Sony PSP, please install psptoolchain 
and set development environment.

To compile project invoke

cd ImageLib/src
make -f Makefile.psp

cd ../../BlockOut
make -f Makefile.psp

To prepare data for installation invoke

make -f Makefile.psp release

That would create all required data in BlockOut_II directory

Installation
============

Copy BlockOut_II directory to Sony PSP device
(e.g. to ms0:/psp/game directory on Memory Stick).

Key mappings
============

CROSS                    - confirm / drop block
RIGHT TRIGGER            - cancel / exit game

D-PAD                    - move block / select character (hall of fame)

SQUARE                   - rotate block around X axis counterclockwise
SQUARE   + LEFT TRIGGER  - rotate block around X axis clockwise
TRIANGLE                 - rotate block around Y axis counterclockwise
TRIANGLE + LEFT TRIGGER  - rotate block around Y axis clockwise
CIRCLE                   - rotate block around Z axis counterclockwise
CIRCLE   + LEFT TRIGGER  - rotate block around Z axis clockwise

START                    - pause / resume game

Online score database
=====================

You can play against / compare your score to online score database, using wifi connection
