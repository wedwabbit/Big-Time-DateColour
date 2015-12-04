Pebble.addEventListener('ready', function() {
	console.log('PebbleKit JS ready!');
});

Pebble.addEventListener('showConfiguration', function() {
	var url = 'http://localhost:8080';
    console.log('url:' + url);
	Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
    var configData = JSON.parse(decodeURIComponent(e.response));

    console.log('Configuration page returned: ' + JSON.stringify(configData));

    if (configData.timebackground) {
        Pebble.sendAppMessage({
          timebackground: parseInt(configData.timebackground, 16),
          timeforeground: parseInt(configData.timeforeground, 16),
          datebackground: parseInt(configData.datebackground, 16),
          dateforeground: parseInt(configData.dateforeground, 16),
          datetimeout: parseInt(configData.datetimeout, 16)
        }, function() {
          console.log('Send successful!');
        }, function() {
          console.log('Send failed!');
        });
    }
});
