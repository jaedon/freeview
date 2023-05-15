Settop Api tests.

Test    		Platform      Description
----------------+----------+---------------
boot            | All      | Initialization test for the settop Api,
                |          | when test completed, 'Init Done' message
                |          | shall be displayed
----------------+----------+---------------
colorkey        | 97038    | Tests video/graphics blending and colorkeying
----------------+----------+---------------
dualdisplay     | 97038    | Dual decode/dual graphics/dual display
----------------+----------+---------------
dualdisplay-hd  | 97038    | Dual decode (HD stream)/dual graphics/dual display
----------------+----------+---------------
dualpvr			| 97038    | Dual record (HD stream)/decode/dual display
----------------+----------+---------------
dualgraphics	| 97038    | dual graphics/dual display 
----------------+----------+---------------
encode_93740    | 93740    | Tests for the encoder start/stop conditions
----------------+----------+---------------
format_change   | 97038    | Single decode/cycling thru NTSC formats
----------------+----------+---------------
graphics_test1  | 97038	   | -"-	
----------------+----------+---------------
graphics_test2  | 97038	   | -"-	
----------------+----------+---------------
graphics_bandwidth | 97038 | perfomance test
----------------+----------+---------------
hdsdsimul		| 97038    | single decode HD/SD simul mode
----------------+----------+---------------
iodecode        | All      | Tests asyncrhonous user I/O and video decode
----------------+----------+---------------
perfomance_pvr  | 97038    | dual(triple) all pass record/dual playback(all
pass)/dual display/dual decode
----------------+----------+---------------
pip				| 97038    | dual decode/single graphics/single display
----------------+----------+---------------
sds_retune		| 97038	   | -"-
----------------+----------+---------------
spdif 			| 97038	   | -"-
----------------+----------+---------------
video_transition| 97038    | Tests for video transitions, show/hide,
                |          | create/destroy, analog/digital
----------------+----------+---------------

