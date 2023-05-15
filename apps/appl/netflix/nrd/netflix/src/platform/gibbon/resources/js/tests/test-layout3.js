var textStyles = {
    bobTitleText: { size: 24, weight: "plain", align: "left top", color: "#bfbfbf" },
    bobSynopsisText: { size: 19, lineHeight: 24, weight: "plain", align: "left top", color: "#bfbfbf" },
    // bobRatingText: { size: 19, weight: "plain", align: "left center", color: "#bfbfbf" },
    bobDurationText: { size: 19, weight: "plain", align: "right center-vertical", color: "#bfbfbf" },
    bobDefaultText:  { size: 19, weight: "plain", align: "left", color: "#bfbfbf" },
};

var boxColor = "#171717";

var markup = {
    scene: {
        id: "root",
        width: "100%",
        height: "100%",
        color: "#aaa",

        children: [{
            id: "bob",
            width: 282,
            height: "100%",
            color: "#f00",
            layout: {
                layout: "vertical",
            },
            children: [
                {
                    id: "title",
                    color: boxColor,
                    text: "This is two lines of title text",
                    textStyle: textStyles.bobTitleText,
                    maxHeight: 60,
                },
                {
                    id: "details",
                    color: boxColor,
                    layout: {
                        layout: "horizontal",
                        spacing: 5,
                    },
                    children: [
                        {
                            text: "2009",
                            textStyle: textStyles.bobSynopsisText,
                        },
                        {
                            text: "NR",
                            textStyle: textStyles.bobSynopsisText,
                        },
                        {  stretch: 90  },
                        {
                            text: "2h 26m",
                            textStyle: textStyles.bobSynopsisText,
                        },
                    ],
                },
                {
                    id: "rating-audio-wrapper",
                    color: boxColor,
                    layout: {
                        layout: "horizontal",
                        spacing: 20,
                    },
                    children: [
                        {
                            image: "http://cdn-0.nflximg.com/us/ffe/htmltvui/plus/720/ratings/star_yellow_all.png",
                        },
                        {
                            hidden: true,
                            image: "http://cdn-0.nflximg.com/us/ffe/htmltvui/plus/720/icons/hd_icon.png",
                        },
                        {
                            image: "http://cdn-0.nflximg.com/us/ffe/htmltvui/plus/720/icons/5.1_icon.png",
                        },
                    ],
                },
                {
                    id: "synopsis",
                    text: "Thirty-something ex-Web designer Damian Hess parlays his dork-tastic rhyming skills into a performing career as MC Frontalot, whose decidedly non-gangsta lyrics make him an Internet sensation and a leading voice in the hip-hop form known as nerdcore. Join Frontalot on his first national tour as he and his bandmates G Minor 7 and Blak Lotus entertain their unique fan base, made up of gamers, bloggers and heavy users of Second Life.",
                    textStyle: textStyles.bobSynopsisText,
                    color: boxColor,
                },
                { spacing: 20 },
                {
                    id: "cast",
                    text: "Weird Al Yankovic, Damian Hess, Brian Posehn",
                    textStyle: textStyles.bobDefaultText,
                    color: boxColor,
                },
                {
                    id: "categories",
                    text: "Documentaries",
                    textStyle: textStyles.bobDefaultText,
                    color: boxColor,
                },
                {
                    id: "directors",
                    text: "Director: Negin Farsad",
                    textStyle: textStyles.bobDefaultText,
                    color: boxColor,
                },
                { stretch: 90 }
            ],
        }],
    },
};

function startScene() {
    gibbon.scene.start(markup);
}

function load_nGn() {
    gibbon.loadScript({ url: "https://tenfootui.netflix.com/htmltvui/projects/debug/Gibbon/latest/native/nGn-native.js" }, startScene);
}

function checkStartup() {
    if (typeof gibbon !== "undefined" && typeof gibbon.init === "function") {
        // if nGn-native is already loaded, use it
        gibbon.init(startScene);
    } else {
        // it's not loaded, try to load it
        if (nrdp.isReady) {
            load_nGn();
        } else {
            nrdp.addEventListener("init", load_nGn);
            nrdp.init();
        }
    }
}

if (typeof nrdp === "undefined") {
    startScene();
} else {
    checkStartup();
}
