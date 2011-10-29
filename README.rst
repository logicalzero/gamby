gamby README
==================

This repository contains (or will contain) the Arduino libraries for use with
the GAMBY LCD/Game shield. 

The initial test sketches will be checked into the /examples/ subdirectory.
As the actual libraries are developed, code will be moved out of the sketches
and into the actual .cpp/.h code; the test sketches will shrink and
eventually disappear.


Technical Notes
===============

LCD
---

GAMBY is based around a 96x64 pixel monochrome display using an `S6B0755 <http://www.alldatasheet.com/datasheet-pdf/pdf/37866/SAMSUNG/S6B0755.html>`__ (or compatible) LCD controller, although this particular module does not support several of the controller's features. 

The display is broken into eight horizontal 8-pixel 'pages'. Drawing to the screen is done by selecting a page and/or column and writing one or more bytes. The column automatically advances with each set of eight bits received, so the column/row select commands do not need to be sent when writing to a single page.

This system of dividing the display into pages has limitation: writing a byte to the display overwrites the data in that column. While the XXXX controller supports bidirectional communication, this particular LCD module does not. Therefore, in order to get per-pixel drawing, GAMBY's graphics mode maintains an offscreen buffer. Drawing is done first to this buffer and the GAMBY library handles drawing the updated pages. The downside is that this uses nearly half of an ATMega 328's available SRAM: 768 bytes for the buffer, plus another 12 bytes for tracking the updated portions of the screen (the 'dirty bits').


Unsupported Features
''''''''''''''''''''

* The LCD module does not support bi-directional communication; it is write-only.
* The module is 96x64 pixels; the controller supports 128x64.
* The controller datasheet makes reference to a 65th display line that contains icons; this module does not have the extra row.



TO DO
=====

See source code for ``@todo:`` tags in doc comments.