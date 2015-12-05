Pebble.addEventListener('ready', function() {
});

Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://wedwabbit.github.io/index-bigtimedatecolour.html';
	Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
    var configData = JSON.parse(decodeURIComponent(e.response));
    if (configData.timebackground) {
        Pebble.sendAppMessage({
          timebackground: parseInt(configData.timebackground, 16),
          timeforeground: parseInt(configData.timeforeground, 16),
          datebackground: parseInt(configData.datebackground, 16),
          dateforeground: parseInt(configData.dateforeground, 16),
          datetimeout: parseInt(configData.datetimeout, 16)
        }, function() {
        }, function() {
        });
    }
});
