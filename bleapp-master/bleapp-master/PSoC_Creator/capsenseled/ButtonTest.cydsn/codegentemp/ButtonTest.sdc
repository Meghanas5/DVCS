# THIS FILE IS AUTOMATICALLY GENERATED
# Project: C:\Users\Sarah\Documents\DVCS\bleapp-master\bleapp-master\PSoC_Creator\capsenseled\ButtonTest.cydsn\ButtonTest.cyprj
# Date: Tue, 25 Apr 2017 18:21:20 GMT
#set_units -time ns
create_clock -name {CyRouted1} -period 20.833333333333332 -waveform {0 10.4166666666667} [list [get_pins {ClockBlock/dsi_in_0}]]
create_clock -name {CyWCO} -period 30517.578125 -waveform {0 15258.7890625} [list [get_pins {ClockBlock/wco}]]
create_clock -name {CyLFCLK} -period 30517.578125 -waveform {0 15258.7890625} [list [get_pins {ClockBlock/lfclk}]]
create_clock -name {CyILO} -period 31250 -waveform {0 15625} [list [get_pins {ClockBlock/ilo}]]
create_clock -name {CyECO} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/eco}]]
create_clock -name {CyIMO} -period 20.833333333333332 -waveform {0 10.4166666666667} [list [get_pins {ClockBlock/imo}]]
create_clock -name {CyHFCLK} -period 20.833333333333332 -waveform {0 10.4166666666667} [list [get_pins {ClockBlock/hfclk}]]
create_clock -name {CySYSCLK} -period 20.833333333333332 -waveform {0 10.4166666666667} [list [get_pins {ClockBlock/sysclk}]]
create_generated_clock -name {Clock_1} -source [get_pins {ClockBlock/hfclk}] -edges {1 48001 96001} [list [get_pins {ClockBlock/udb_div_0}]]


# Component constraints for C:\Users\Sarah\Documents\DVCS\bleapp-master\bleapp-master\PSoC_Creator\capsenseled\ButtonTest.cydsn\TopDesign\TopDesign.cysch
# Project: C:\Users\Sarah\Documents\DVCS\bleapp-master\bleapp-master\PSoC_Creator\capsenseled\ButtonTest.cydsn\ButtonTest.cyprj
# Date: Tue, 25 Apr 2017 18:21:17 GMT
