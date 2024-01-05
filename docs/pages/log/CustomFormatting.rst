.. _pages_log_formatting::

.. role:: underline
    :class: underline

.. role:: red
    :class: red

.. role:: red-background
    :class: red-background

Custom formatting
=================

Flags
-----

The following table outlines the formatting flags that can be used, along with their corresponding descriptions and examples.
They allows customization of log message formatting. Each formatting flag represents a specific aspect of a log entry, such as log level, timestamp, source file information, and more.
The associated examples showcase how the log entries would look with the respective formatting applied.

Flags in the pattern are represented as ´%flag´.

.. list-table:: Formatting Flags Overview
   :widths: 10 45 45
   :header-rows: 1

   * - Flag
     - Description
     - Example
   * - 'l'
     - Log Level
     - info | warn | error
   * - 'v'
     - Log message
     - Hello World!
   * - 't'
     - Thread ID
     - 1234
   * - 'n'
     - Logger name
     - my_logger
   * - 'P'
     - Process ID
     - 5678
   * - 'Q<c>'
     - ANSI color (foreground) where <c> is a letter from :ref:`this table<ansi-colors-table>`
     - %Qc: :red:`error`
   * - 'q<c>'
     - ANSI color (background) where <c> is a letter from :ref:`this table<ansi-colors-table>`
     - %qr: :red-background:`critical`
   * - '$'
     - ANSI reset
     -
   * - '@'
     - ANSI log level color
     - :red:`error`
   * - '_'
     - ANSI underline
     - :underline:`Underline`
   * - '*'
     - ANSI bold
     - *Bold*
   * - 's'
     - Source file name
     - /path/to/example.cpp
   * - 'g'
     - Source file name (short)
     - example.cpp
   * - '#'
     - Source line number
     - 42
   * - '@'
     - Source column number
     - 12
   * - '!'
     - Source function name
     - int main()
   * - 'A'
     - Weekday name
     - Monday
   * - 'a'
     - Weekday short name
     - Mon
   * - 'B'
     - Month name
     - January
   * - 'b'
     - Month short name
     - Jan
   * - 'c'
     - Date and time
     - 01/05/2022 13:45:30
   * - 'C'
     - Short year
     - 22
   * - 'Y'
     - Year
     - 2022
   * - 'D'
     - Short date
     - 01/05/22
   * - 'x'
     - Local date
     - 01/05/22
   * - 'm'
     - Month
     - 01
   * - 'd'
     - Day
     - 05
   * - 'H'
     - Hour (24-hour format)
     - 13
   * - 'I'
     - Hour (12-hour format)
     - 01
   * - 'M'
     - Minute
     - 45
   * - 'S'
     - Second
     - 30
   * - 'e'
     - Millisecond
     - 31.743
   * - 'f'
     - Microsecond
     - 31.743154
   * - 'F'
     - Nanosecond
     - 31.987654321
   * - 'p'
     - AM or PM
     - PM
   * - 'r'
     - Time (12-hour format)
     - 01:45:30 PM
   * - 'R'
     - Time (24-hour format)
     - 13:45
   * - 'T'
     - ISO 8601 time
     - 13:45:30
   * - 'z'
     - ISO 8601 timezone
     - +00:00
   * - 'E'
     - Seconds since epoch
     - 1704437905

.. note::
    Please note that ANSI escape codes are used for certain formatting flags to represent colors and styles, and these might not render correctly in all environments.

Colors
------

The table below provides a quick reference for ANSI colors, their associated escape codes, and corresponding color flag letters.

.. _ansi-colors-table:

.. list-table:: Color flags
   :widths: 10 45 45
   :header-rows: 1

   * - Color
     - Code
     - Letter
   * - Black
     - 30/40
     - k
   * - Red
     - 31/41
     - r
   * - Green
     - 32/42
     - g
   * - Yellow
     - 33/43
     - y
   * - Blue
     - 34/44
     - b
   * - Magenta
     - 35/45
     - m
   * - Cyan
     - 36/46
     - c
   * - White
     - 37/47
     - w


