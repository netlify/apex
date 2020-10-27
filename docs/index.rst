Overview
========

Netlify's Apex library is the core of nearly everything we do regarding C++.
It provides features such as a small shim of missing C++ standard library
features that may not be available for our minimum compiler target, provides
implementations of proposed C++ standard library features, a low-cost sqlite3
wrapper with a focus on readability and ease of use over high level operations,
journald logging (in progress), and additional types that are found in various
states in the wild of the C++ ecosystem, but might not be able to be
implemented in the C++ standard due to platform exclusitivity, or an inability
to target the C++ abstract machine.

.. toctree::
   prelude
   concepts
   mixins
   monads
   cpo
