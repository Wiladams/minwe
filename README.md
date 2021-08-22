# minwe
 Minimum Windowing Environment

I have several projects that create a minimum shim to the Windows UI environment as part of their framework.  This projects takes some of the learnings from those various projects and puts the code in one place.

The language is C++.  It's pretty basic C++ though.  I avoid using more exotic features
primarily sticking to what can be found in C++ 2011, if that.

Building
The environment assumes you are building within the context of a standard C++ project
on Windows.  It makes usage of includes and whatnot that should be standard if you're
using C++ or any other compiler environment on Windows.

Philosophy
==========
Since this code has been built up, borrowed, loaned, morphed, rewritten, several
times over the years, I try to do a few things.
Minimum required
Duplicate for convenience
Don't use super fancy stuff

It is an opinionated implementation.  At the very core, there is an eventing
system that is based on a publish/subscribe model.  Consumers of this little library
are meant to subscribe to this system, and construct their own eventing model atop that.

In addition, the framework has its own "main" eventing loop, which allows the app
developer to create their own thing.