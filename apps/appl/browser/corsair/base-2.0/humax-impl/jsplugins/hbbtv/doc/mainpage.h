/**
  @mainpage Opera OIPF Framework

  @section intro Introduction

<p> The OOIF is the Opera solution for implementing the JavaScript
extensions defined in the OIPF and HbbTV specifications, and mapping
them down to the native functionality of the device.</p>

<p> Technically the OOIF is a JavaScript plug-in. This is a plug-in
technology used in the Opera Devices SDK which allows integrators to
extend the JavaScript environment with custom functionality which maps
down to code running in the native platform. More information about
this can be found in the SDK documentation.</p>

<p> The OOIF is a thin wrapper layer between the Opera browser engine
and the platform. It implements the JavaScript interfaces of the OIPF
and HbbTV specifications, but very little beyond that. In most cases,
it does not contain any logic or complicated semantics. These are
instead left to the layer beneath the OOIF - the <b>Native OOIF
Integration Layer</b> - to implement.</p>

  \image html ooif.arch.png

<p> The Native OOIF Integration Layer is the component that lies
between the OOIF and the direct platform capabilities. The Native OOIF
Integration Layer is responsible for implementing the functionality in
the OIPF/HbbTV specifications and connecting the OOIF plug-in to the
platform.</p>

<p> The Native OOIF Integration Layer is connected to the OOIF using
native interfaces. The functions in these interfaces are often a
direct one-to-one mapping to parameters and methods in the OIPF/HbbTV
specification. See more about this, and about what exceptions exist,
in the The native interface section in the SDK documentation. Examples
of call sequences can be found in the Example call sequences section
in the SDK documentation. </p>

<p> Part of the integration layer is related to management of
applications, which map closely to browser windows. To implement this
it is necessary to work directly with the GOGI APIs. Typically, this
functionality will be implemented in the integration layer as a
component somewhat separate from the rest of the integration layer
code, and being called something like the Application
Manager.</p>

*/
