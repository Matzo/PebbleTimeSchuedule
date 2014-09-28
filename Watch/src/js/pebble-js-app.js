var GOOGLE_OAUTH2_URL = "https://05197a3df7e32d13dbac6dbe202dc01e757378c1.googledrive.com/host/0B_dux-6Sb088M28zd0hPUG1KQ1E/";

Pebble.addEventListener("ready",
    function(e) {
//        console.log("Hello world! - Sent from your javascript application.");
    }
);

Pebble.addEventListener("showConfiguration",
    function(e) {
        Pebble.openURL(GOOGLE_OAUTH2_URL);
        console.log("show Configuration: " + e.response);
    }
);

Pebble.addEventListener("webviewclosed",
    function(e) {
        console.log("Configuration window returned: " + e.response);
    }
);
