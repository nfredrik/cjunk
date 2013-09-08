#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
#                     GLOBAL VARIABLE TO TRANSLATE PROBE NAMES
#
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# FileName      : TranslateProbe.pm
# Author        : Fredrik Svärd
# Date          : 15 Nov 2006
# Version       : 1.0
# Subjet        :
#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Netcool/Proviso                                        Copyright(c) Telia
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package TranslateProbe;

use strict;
use warnings;
use Exporter;
use base 'Exporter';
our @EXPORT =qw(%TeliaSonera @Num_2_String %Digit_2_String);

#===============================================================================
# Global variables
#===============================================================================

    our %TeliaSonera = (
        'TSS-fre-mp3'  => 'Stockholm',
        'TSS-m-sp-mp2'    => 'Malmo',
        'TSS-blg-mp2'  => 'Borlange',
        'TSS-vs-a-mp2' => 'Vasteras',
        'TSS-sv-g-mp2' => 'Sundsvall',
        'TSS-n-mp2'    => 'Norrkoping',
        'TSS-j-mp2'    => 'Jonkoping',
        'TSS-ld-c-mp2'   => 'Lund',
        'TSS-th-c-mp2'   => 'Trollhattan',
        'TSS-smy-mp2'  => 'Kalmar',
        'TSS-ks-mp2'   => 'Karlstad',
        'TSS-vrr-mp2'  => 'Goteborg',
        'TSS-dro-mp2'  => 'Helsingborg',
        'TSS-ll-mp2'   => 'Lulea',

        'TSF-olucorebrix1' => 'Oulu',
        'TSF-jklcorebrix1' => 'Jyvaskyla',
        'TSF-trepnkcorebrix1' => 'Tampere',
        'TSF-kvlcorebrix1' => 'Kouvola',
        'TSF-tkucorebrix1'  => 'Turku(Abo)',
        'TSF-hkihkpcorebrix1' => 'Helsinki(Kapyla)',
	'TSF-hkitmcorebrix1' => 'Helsinki(Tahtitorninmaki)',

        'TSD-dk-ejb-brix1' => "Copenhagen (Ejby)",
	'TSD-dk-osd-brix1' => "Copenhagen (Orestad)",
        'TSD-dk-kol-brix1' => "Kolding",
        'TSD-dk-hc-brix1' => "Hasselager"
    );

    our @Num_2_String = (
        "test completed succesfully",             # 0
        "DNS name not resolved",                  # 1
        "Responder unavailable",                  # 2
        "Responder unreachable",                  # 3
        "No test instance",                       # 4
        "Connection refused",                     # 5
        "Invalid test version",                   # 6
        "No UDP port available",                  # 7
        "Responder Verifier stopped the test",    # 8
        "Invalid SrcIPAddr parameter"             # 9
    );

my %Digit_2_String = (
        0  => "test completed succesfully",             
        1  => "DNS name not resolved",                  
        2  => "Responder unavailable",                  
        3  => "Responder unreachable",                  
        4  => "No test instance",                       
        5  => "Connection refused",                     
        6  => "Invalid test version",                   
        7  => "No UDP port available",                  
        8  => "Responder Verifier stopped the test",    
        9  => "Invalid SrcIPAddr parameter",             
	7173  => "Internal network error"               
    );
