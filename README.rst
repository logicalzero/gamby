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

GAMBY is based around a 96x64 pixel monochrome display manufactured by Samsung for Motorola, internal part number UG09B63-FLGT4AA. It uses an `S6B0755 <http://www.alldatasheet.com/datasheet-pdf/pdf/37866/SAMSUNG/S6B0755.html>`__ (or compatible) LCD controller, although this particular module does not support several of the controller's features (documented below).

The display is broken into eight horizontal 'pages', 8 pixels high and 96 pixels wide. Drawing to the screen is done by selecting a page and/or column and writing one or more bytes. The column automatically advances with each set of eight bits received, so the column/row select commands do not need to be sent when writing to a single page.

.. note:: The controller datasheet makes reference to a 65th display line, corresponding to a row of permanent icons (e.g. cell phone signal strength). This LCD module does not have such icons. The LCD is also designed to support a 128x64 pixel display, whereas this module only provides 96 columns. The extra 32 columns exist in the LCD's memory but aren't displayed. More about peculiarities relating to this later.

This system of dividing the display into pages has limitation: writing a byte to the display overwrites the data in that column. While the S6B0755 controller supports bidirectional communication, this particular LCD module does not. Therefore, in order to get per-pixel drawing, GAMBY's graphics mode maintains an offscreen buffer. Drawing is done first to this buffer and the GAMBY library handles drawing the updated pages. The downside is that this uses nearly half of an ATMega 328's available SRAM: 768 bytes for the buffer, plus another 12 bytes for tracking the updated portions of the screen (the 'dirty bits').


Flipped Screen
''''''''''''''
Strictly speaking, the LCD module on GAMBY is installed upside down; this was the best way to attach it to the shield. However, the controller has a feature -- ``SHL_SELECT``, which reverses the direction in which its memory is scanned -- to allow it to function normally this way. The only trick is that since the controller supports a 128 column display, the first 32 columns (rather than the last 32) are off the edge of the screen, so the addresses of the visible columns must be offset by 32. The library's positioning methods take this into account; the constant ``SET_COLUMN_ADDR_1`` (the first byte of the two-byte column address selection command) has this offset factored into it.



TO DO
=====

See source code for ``@todo:`` tags in doc comments.