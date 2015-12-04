(function() {
  loadOptions();
  submitHandler();
})();

function submitHandler() {
    var $submitButton = $('#submitButton');

    $submitButton.on('click', function() {
      var return_to = getQueryParam('return_to', 'pebblejs://close#');
      document.location = return_to + encodeURIComponent(JSON.stringify(getAndStoreConfigData()));
    });
}

function loadOptions() {
  var $timebackground = $('#timebackground');
  var $timeforeground = $('#timeforeground');
  var $datebackground = $('#datebackground');
  var $dateforeground = $('#dateforeground');
  var $datetimeout = $('#datetimeout');

  if (localStorage.timebackground) {
    $timebackground[0].value = localStorage.timebackground;
    $timeforeground[0].value = localStorage.timeforeground;
    $datebackground[0].value = localStorage.datebackground;
    $dateforeground[0].value = localStorage.dateforeground;
    $datetimeout[0].value = localStorage.datetimeout;
  }
}

function getAndStoreConfigData() {
  var $timebackground = $('#timebackground');
  var $timeforeground = $('#timeforeground');
  var $datebackground = $('#datebackground');
  var $dateforeground = $('#dateforeground');
  var $datetimeout = $('#datetimeout');

  var options = {
    timebackground: $timebackground.val(),
    timeforeground: $timeforeground.val(),
    datebackground: $datebackground.val(),
    dateforeground: $dateforeground.val(),
    datetimeout: $datetimeout.val()
  };

  localStorage.timebackground = options.timebackground;
  localStorage.timeforeground = options.timeforeground;
  localStorage.datebackground = options.datebackground;
  localStorage.dateforeground = options.dateforeground;
  localStorage.datetimeout = options.datetimeout;

  return options;
}

function getQueryParam(variable, defaultValue) {
  var query = location.search.substring(1);
  var vars = query.split('&');
  for (var i = 0; i < vars.length; i++) {
    var pair = vars[i].split('=');
    if (pair[0] === variable) {
      return decodeURIComponent(pair[1]);
    }
  }
  return defaultValue || false;
}
