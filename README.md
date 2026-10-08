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

A demonstration bundle of the Core Flight System including the cFE, OSAL, and PSP can be obtained at https://github.com/nasa/cfs

For information about a mission ready cFS bundle, see: https://github.com/nasa/cFS#cfs-gov-mission-ready-version

## Known issues

See all [open issues](https://github.com/nasa/LC/issues) and closed to milestones later than this version.

## Getting Help

For best results, submit issues:questions or issues:help wanted requests at <https://github.com/nasa/cFS>.

Official cFS page: <http://cfs.gsfc.nasa.gov>
