D3
===

The d3.js module provides integration of D3 visualization library with Backdrop.

There are several javascript charts & graphs libraries out there, googleapi and
jqplot for example. The D3 library, however, is not only a much more flexible
library, but also offers more than just charts and graphs.

Some examples include:
 * Chord Digram
 * Force-Directed Graph
 * Treemap
 * Scatterplot Matrix
 * Bubble Chart
 * Pie Charts / Donut Charts
 * Multiple Line Graphs
 * etc...

Information on d3.js
 * http://mbostock.github.com/d3/
 * https://leanpub.com/D3-Tips-and-Tricks

Related Modules
 * D3 Libraries - Placeholder hub to share D3-Drupal libraries with the community
 * Charts Graphs - Create new charts using the UI, requires Charts Graphs D3
 * D3 Sparkline - Style library for d3.js used to visualize Drupal.org issue statistics
 * D3 Sankey - API to let you create alluvial and Sankey diagrams using the D3.js module and its corresponding JavaScript library, New Relic's fork of D3js's Sankey plugin, and the d3.chart.sankey library.
 * GraphAPI - module which provides views integration to build graphs

Requirements
------------

This module requires that the following modules are also enabled:

 * [Libraries API](https://github.com/backdrop-contrib/libraries)
 * Latest version of [d3.js](http://d3js.org)

Installation <!-- This section is required. -->
------------

- Install this module using the official Backdrop CMS instructions at
  https://docs.backdropcms.org/documentation/extend-with-modules.

- Download the [D3 library](https://github.com/mbostock/d3/wiki) and place it
  into the libraries location. (@todo, remove this after the library is bundled)

- To download the library using the provided example makefile and drush, use:
  `drush make --no-core d3.make.example` (@todo, remove this after the library
  is bundled)

- If you're not familiar with the D3 API, see the d3_examples module
  for various code examples of the d3 API.


Differences from Drupal 7
-------------------------

- @todo: The d3.js library will be bundled into the module instead of requiring
  separate download and installation.

Documentation
-------------

Additional documentation is located in [the Wiki](https://github.com/backdrop-contrib/d3/wiki/Documentation).

Issues
------

Bugs and feature requests should be reported in [the Issue Queue](https://github.com/backdrop-contrib/d3/issues).

Current Maintainers
-------------------

- [Jen Lampton](https://github.com/jenlampton).
- [Joseph Flatt](https://github.com/hosef).
- Seeking additional maintainers.

Credits
-------

- Ported to Backdrop CMS by [Jen Lampton](https://github.com/jenlampton).
- Maintained for Drupal by [Alan Sherry](https://www.drupal.org/u/asherry).
- Supported for Drupal by [Aten Design Group](https://www.drupal.org/aten-design-group)
- Based on the [d3.js](http://d3js.org) project.

Licenses
--------

* This project is GPL v2 software. See the LICENSE.txt file in this directory for complete text.
* d3.js library released under [BSD license](https://opensource.org/licenses/BSD-3-Clause).


