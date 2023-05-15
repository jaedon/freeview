var menuItemImage = "http://cdn-0.nflximg.com/us/ffe/htmltvui/plus/720/mdp/mdp_menuItem_focused.png";

var verticalLayoutMarkup = {
    scene: {
        id: "root",
        width: "100%",
        height: "100%",
        color: "#f00",
        layout: {
            layout: "vertical",
        },
        children: [
            {
                id: "one",
                text: "One",
                color: "#ddd",
            },
            {
                id: "two",
                text: "Two",
                color: "#ddd",
            },
            {
                stretch: 1,
                id: "three",
                text: "Three",
                color: "#ddd",
                layout: {
                    layout: "horizontal",
                },
                children: [
                    { id: "header", stretch: 10 },
                    {
                        id: "box1",
                        image: "http://cdn-9.nflximg.com/en_us/boxshots/150/70140429.jpg",
                    },
                    {
                        id: "box2",
                        image: "http://cdn-9.nflximg.com/en_us/boxshots/150/70140429.jpg",
                    },
                    {
                        id: "box3",
                        image: "http://cdn-9.nflximg.com/en_us/boxshots/150/70140429.jpg",
                    },
                    {
                        id: "box4",
                        image: "http://cdn-9.nflximg.com/en_us/boxshots/150/70140429.jpg",
                    },
                    {
                        id: "box5",
                        image: "http://cdn-9.nflximg.com/en_us/boxshots/150/70140429.jpg",
                    },
                    {
                        id: "box6",
                        image: "http://cdn-9.nflximg.com/en_us/boxshots/150/70140429.jpg",
                    },
                    { id: "footer", stretch: 10 },
                ],
            },
            {
                stretch: 1,
                id: "four",
                text: "Four",
                color: "#ddd",
            },
            {
                id: "five",
                text: "Five",
                color: "#ddd",
            },
        ],
    },
};

function startScene() {
    gibbon.scene.start(verticalLayoutMarkup);
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

