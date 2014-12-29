var GOOGLE_OAUTH2_URL = "https://05197a3df7e32d13dbac6dbe202dc01e757378c1.googledrive.com/host/0B_dux-6Sb088M28zd0hPUG1KQ1E/";

//var configurations = {
//  'parse_application_id':'LcrLTWbWLAHG0EHJnDgaWu6twaOxh6Mmtw82VwHb',
//  'parse_rest_key': 'xVlJD46HlhzJADWEPdSJR7DaZ12ydUtvz54ovwPA'
//};

//var appMessageKey {
//  iosDeviceTokenKey:9
//}

var sendRefreshNotification = function(deviceToken) {
    ajax({
        method: 'post',
        url: 'https://api.parse.com/1/functions/refreshNotification',
        type: 'json',
        headers: {
            'X-Parse-Application-Id': configurations.parse_application_id,
            'X-Parse-REST-API-Key': configurations.parse_rest_key,
            'Content-Type': 'application/json;charset=utf-8'
        },
        data: {
            device:deviceToken
        },
        cache : false
    }, function(e) {
        console.log("success:" + e);
    }, function(e) {
        console.error('Request failed with response code ' + e.status);
    });
}

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

Pebble.addEventListener('appmessage',
    function(e) {
        var deviceToken = e.payload.iosDeviceTokenKey;
        sendRefreshNotification(deviceToken);
    }
);

Pebble.addEventListener("webviewclosed",
    function(e) {
        console.log("Configuration window returned: " + e.response);
    }
);


