Serelepe
========

Serelepe, short Ser, is a project which aims to use JerryScript for scripting.

Unlike iot.js, Serelepe will try to expose more internals to the JavaScript
side and will try to avoid the use the event loop for now. I want to see whether
JerryScript could be made to work with FFI and to drive a forking HTTP server
handler.

About JerryScript
=================

JerryScript is not exactly fast when compared to JIT VMs like V8. JerryScript
is optimized to use less memory. While the JerryScript project does not advise
to use it on the server or on the desktop where plenty of resources are
available, it uses so little resources and tries hard to be cross-platform for
the Internet of Things, so who knows what it could be used for.

JavaScript is a programming language that is getting a lot of support from
tools, which could be shared by JerryScript.

JerryScript is written in C, whereas a lot of JavaScript implementations are
written in C++ instead. That difference may make JerryScript easier to embed
and to play with.

We'll include the JerryScript source code into the repository for general peace
of mind.

Name
====

The name Serelepe is from a Portuguese word that means agile, playful, and to
me it resembled the spirit of the name Jerry of JerryScript which was named
after a cartoon mouse from Tom and Jerry.

Since it's a long word, I may adopt the Ser shorter name from time to time which
itself is another Portuguese word that means a being and to exist. The idea for
this shorter version of the name is from the Internet of Things which
JerryScript aims to serve.
