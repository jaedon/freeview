function startSpecial() {
    //nrdp.gibbon.log("Gibbon lives! "  + nrdp.gibbon.scene.width + " " + nrdp.gibbon.scene.height);
    w = nrdp.gibbon.makeWidget();
    {
        nrdp.gibbon.scene.widget = w;
        w.x = 10;
        w.y = 0;
        w.width = 1000;
        w.height = 1000;
        w.opacity = 1.0;
        w.color = { a: 255, r: 255, g: 0, b: 0 };
        //w.image.url = "http://www.google.com/intl/en_com/images/srpr/logo3w.png";
    }

    right = nrdp.gibbon.makeWidget()
    {
        right.parent = w;
        right.x = 50;
        right.y = 0;
        right.width = 300;
        right.height = 300;
        right.opacity = 1.0;
        right.color = { a: 255, r: 255, g: 255, b: 255 };
        right.text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec sit amet eros a arcu posuere vestibulum. Cras non nunc erat. Sed quis nulla felis, et sollicitudin mi. Donec in diam non urna dapibus ullamcorper. Praesent interdum ante in ligula varius non tincidunt ligula dapibus. Quisque sollicitudin metus sollicitudin eros dictum id sollicitudin ligula rutrum. Duis a sollicitudin augue. In id lacus eu velit lacinia vestibulum. In hac habitasse platea dictumst. In tortor metus, luctus a ultricies et, tincidunt et turpis. Integer at diam nulla. Quisque augue felis, congue eu gravida eget, rhoncus vel velit. Donec non velit a augue faucibus placerat.\nEtiam gravida, urna in aliquet facilisis, risus lacus suscipit odio, ut consectetur tellus neque tempor elit. Vestibulum non dictum nisi. Curabitur accumsan pulvinar augue, id laoreet justo dignissim ut. Nulla euismod, eros id gravida sollicitudin, lacus turpis ornare felis, eu auctor sem dolor nec dui. Maecenas a mauris quis justo dignissim facilisis. Morbi aliquam fermentum ligula sit amet mollis. Sed risus sem, tincidunt sit amet euismod quis, blandit eget ligula. Sed laoreet quam iaculis leo varius consequat. Suspendisse fermentum elit vulputate sem dictum sollicitudin. In hac habitasse platea dictumst. Quisque purus neque, congue ac molestie quis, egestas eget elit. Proin pretium ultrices sem, at scelerisque turpis consectetur quis. Phasellus elementum pretium commodo. Nunc elementum lobortis magna, ut faucibus felis hendrerit sodales. Aenean eu leo ac nunc vehicula ultricies quis id felis. Integer arcu ipsum, vestibulum sed pharetra id, lacinia quis ante.";
		//right.text="Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec sit amet eros a arcu posuere vestibulum. Cras non nunc erat."
		//right.text="hola, ¿qué tal estás? entrañable amigo murciélago. ¿Dónde tienes el ñu que compraste en Güímar?";

        right.textStyle = {
        	"size": 20,
        	"weight": "normal",
        	"variant" : "normal",
        	"align": "left",
        	"wrap": "true",
        	"shadow": {
        		"offsetX": 0,
        		"offsetY": 0,
        		"color": { "a": 192, "r": 192, "g": 192, "b": 192 }
        	},
		    "truncation": {
		        "position": "end",
		        "ellipsis": "..."
		    }
        };

    }
  
/*  
    right = nrdp.gibbon.makeWidget()
    {
        right.parent = w;
        right.x = 50;
        right.y = 300;
        right.width = 300;
        right.height = 300;
        right.opacity = 1.0;
        right.color = { a: 255, r: 255, g: 255, b: 255 };
//        right.text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec sit amet eros a arcu posuere vestibulum. Cras non nunc erat. Sed quis nulla felis, et sollicitudin mi. Donec in diam non urna dapibus ullamcorper. Praesent interdum ante in ligula varius non tincidunt ligula dapibus. Quisque sollicitudin metus sollicitudin eros dictum id sollicitudin ligula rutrum. Duis a sollicitudin augue. In id lacus eu velit lacinia vestibulum. In hac habitasse platea dictumst. In tortor metus, luctus a ultricies et, tincidunt et turpis. Integer at diam nulla. Quisque augue felis, congue eu gravida eget, rhoncus vel velit. Donec non velit a augue faucibus placerat.\nEtiam gravida, urna in aliquet facilisis, risus lacus suscipit odio, ut consectetur tellus neque tempor elit. Vestibulum non dictum nisi. Curabitur accumsan pulvinar augue, id laoreet justo dignissim ut. Nulla euismod, eros id gravida sollicitudin, lacus turpis ornare felis, eu auctor sem dolor nec dui. Maecenas a mauris quis justo dignissim facilisis. Morbi aliquam fermentum ligula sit amet mollis. Sed risus sem, tincidunt sit amet euismod quis, blandit eget ligula. Sed laoreet quam iaculis leo varius consequat. Suspendisse fermentum elit vulputate sem dictum sollicitudin. In hac habitasse platea dictumst. Quisque purus neque, congue ac molestie quis, egestas eget elit. Proin pretium ultrices sem, at scelerisque turpis consectetur quis. Phasellus elementum pretium commodo. Nunc elementum lobortis magna, ut faucibus felis hendrerit sodales. Aenean eu leo ac nunc vehicula ultricies quis id felis. Integer arcu ipsum, vestibulum sed pharetra id, lacinia quis ante.";
		right.text="AlaLorem ipsum dolor sit amet, consectetur adipiscing elit. Donec sit amet eros a arcu posuere vestibulum. Cras non nunc erat."
		//right.text="hola, ¿qué tal estás?";

        right.textStyle = {
        	"size": 20,
        	"weight": "normal",
        	"variant" : "normal",
        	"align": "center-horizontal",
        	"wrap": "true",
        	"shadow": {
        		"offsetX": 0,
        		"offsetY": 0,
        		"color": { "a": 192, "r": 192, "g": 192, "b": 192 }
        	},
		    "truncation": {
		        "position": "none",
		        "ellipsis": "..."
		    }
        };

    }
*/    

/*
  "text": "",
  "textStyle":  {
     "align":  [
        "right"
     ],
     "color":  {
        "a": 255,
        "b": 0,
        "g": 0,
        "r": 0
     },
     "family": "Arial_for_Netflix",
     "size": 30,
     "weight": "bold"
   },
*/


}
nrdp.addEventListener("init", startSpecial);
nrdp.init();


