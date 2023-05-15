python ssu_fvp5000t.py 
./tool/insertsec DSI.sec 13
./tool/insertsec DII-1.sec 14
./ssu-update.sh datacarousel 321 0
tscbrmuxer b:7520 firstpat.ts b:7520 firstpmt.ts b:150 firstnit.ts b:150 firstsdt.ts c:1000000 datacarousel.ts o:1048576 null.ts > output/3_ota_FVP5000T_single_UKTFAE_.ts

./tool/ExtractPSI 321 output/3_ota_FVP5000T_single_UKTFAE_.ts output/2_ota_FVP5000T_single_UKTFAE_.sec

./clean.sh
