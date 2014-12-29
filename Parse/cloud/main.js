Parse.Cloud.define("refreshNotification", function(request, response) {
  var device = request.params.device;
  var query = new Parse.Query(Parse.Installation);
  query.equalTo('deviceToken', device);

  var ret = Parse.Push.send({
    where: query,
    data: {
      "operation":"refresh",
      "content-available":1
    }
  }, {
    success: function() {
      // Push was successful
      response.success();
    },
    error: function(error) {
      // Handle error
      console.error(error);
      response.error();
    }
  });
});
