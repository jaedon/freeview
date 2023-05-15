python ssu_fvp5000t_next.py 
./tool/insertsec DSI.sec 13
./tool/insertsec DII-1.sec 14
./ssu-update.sh datacarousel 321 0
tscbrmuxer b:7520 firstpat.ts b:7520 firstpmt.ts b:150 firstnit.ts b:150 firstsdt.ts c:1000000 datacarousel.ts o:1048576 null.ts > output/4_ota_FVP5000T_single_UKTFAE_next.ts

#./tool/ExtractPSI 321 output/4_ota_FVP5000T_single_UKSFAA_next.ts output/4_ota_FVP5000T_single_UKTFAE_next.sec

./clean.sh
