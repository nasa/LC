# core Flight System (cFS) Limit Checker (LC)

## Introduction

The Limit Checker (LC) is a core Flight System (cFS) application
that is a plug in to the Core Flight Executive (cFE) component of the cFS.

The LC application monitors telemetry data points in a cFS system and compares 
the values against predefined threshold limits. When a threshold condition is 
encountered, an event message is issued and a Relative Time Sequence (RTS) 
command script may be initiated to respond/react to the threshold violation.  

The LC application is written in C and depends on the cFS Operating System
Abstraction Layer (OSAL) and cFE components.  There is additional LC application
specific configuration information contained in the application user's guide.

User's guide information can be generated using Doxygen (from top mission directory):
```
  make prep
  make -C build/docs/lc-usersguide lc-usersguide
```

## Software Required

cFS Framework (cFE, OSAL, PSP)

An integrated bundle including the cFE, OSAL, and PSP can
be obtained at https://github.com/nasa/cfs

## About cFS

The cFS is a platform and project independent reusable software framework and
set of reusable applications developed by NASA Goddard Space Flight Center.
This framework is used as the basis for the flight software for satellite data
systems and instruments, but can be used on other embedded systems.  More
information on the cFS can be found at http://cfs.gsfc.nasa.gov
