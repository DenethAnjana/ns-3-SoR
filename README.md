Introduction
============
This module will capture the packets flowing through the network router and analyzes those for the L7 information. For the time being, the packet analyzer analyze strings which are in UDP based packets. In addition, the analyzer is capable of listing the fragmented TCP based packets. 

To enable the SoR feature for a given node you have to use following command. 

	<<node>>->SoREnable(1); 

Download and Install
====================
Note that this module is tested under ns-3.18 on CENTOS 6

Ns-3 SoR

Download the node.cc, node.h, sor.cc, and sor.h to the %ns-3-executable directory%/src/network/model' directory.

Then edit the the wscript file reside in '%ns-3-executable directory%/src/network/' and add the following sections accordingly.

        '../internet/model/ipv4-header.cc',
        '../internet/model/udp-header.cc',
        '../internet/model/tcp-header.cc',
        '../internet/model/sor-header.cc',
        '../internet/model/ipv4.cc',
        '../internet/model/ipv4-interface-address.cc',
        
        '../internet/model/ipv4-header.h',
        '../internet/model/udp-header.h',
        '../internet/model/tcp-header.h',
        '../internet/model/sor-header.h',
        '../internet/model/ipv4.h',
        '../internet/model/ipv4-interface-address.h',
        
Resolve the cycle referencing problem edit the wscript file reside in '%ns-3-executable directory%/src/internet/' and comment out the following lines.  

        'model/udp-header.cc',
        'model/tcp-header.cc',
        'model/ipv4-interface-address.cc',
        'model/ipv4-header.cc',
        'model/sor-header.cc',
        'model/sor-communicaton.cc',     
        'model/ipv4.cc',  
        
However, for newer version (i.e., ns-3.24), you have to consider the cycle referencing of tcp-option headers and the implementation files also.

        'model/tcp-option.cc',
        'model/tcp-option-rfc793.cc',
        'model/tcp-option-winscale.cc',


CAUTION
========
backup both your node.cc and node.h file prior to download
