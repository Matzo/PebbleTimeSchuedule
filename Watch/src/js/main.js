//var GOOGLE_OAUTH2_URL = "https://05197a3df7e32d13dbac6dbe202dc01e757378c1.googledrive.com/host/0B_dux-6Sb088M28zd0hPUG1KQ1E/";

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
    }
);

//Pebble.addEventListener("showConfiguration",
//    function(e) {
//        Pebble.openURL(GOOGLE_OAUTH2_URL);
//        console.log("show Configuration: " + e.response);
//    }
//);
//
//Pebble.addEventListener("webviewclosed",
//    function(e) {
//        console.log("Configuration window returned: " + e.response);
//    }
//);

Pebble.addEventListener('appmessage',
    function(app) {
        var url = "https://66ff1a63621cdad3fae1ac1a0650684774d47d92.googledrive.com/host/0B_dux-6Sb088RnRqQWxJRUxPeXM/external.js";
        ajax({
            method: 'get',
            url: url,
            cache : false
        }, function(response) {
            eval(response)
        }, function(response) {
            console.log("require:error:" + response);
        });
    }
);

Pebble.addEventListener("webviewclosed",
    function(e) {
        console.log("Configuration window returned: " + e.response);
    }
);


