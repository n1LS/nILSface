var myAPIKey = 'bb0925e696edf0facb38bfc8cfec630c';
var lastIcon = -1;
var lastTemperature = -9999;
var ready = false;

function iconFromWeatherId(weatherId) {
    if (weatherId < 600) {
        return 2;
    } else if (weatherId < 700) {
        return 3;
    } else if (weatherId > 800) {
        return 1;
    } else {
        return 0;
    }
}

function fetchWeather(latitude, longitude)
{
	if (!ready)
	{
		console.log('(i) featchWeather before ready. Ignoring.');
		return;
	}

	console.log('(i) featchWeather() requesting');

	var req = new XMLHttpRequest();
    req.open('GET', 'http://api.openweathermap.org/data/2.5/weather?' + 
        'lat=' + latitude + '&lon=' + longitude + '&cnt=1&appid=' + myAPIKey, true);
    req.onload = function () {

		if (req.readyState === 4) {
            if (req.status === 200) {
                console.log('Weather request returns:\n', req.responseText);

				var response = JSON.parse(req.responseText);
                var temperature = Math.round(response.main.temp - 273.15);
                var icon = iconFromWeatherId(response.weather[0].id);

                if (icon != lastIcon || temperature != lastTemperature) {
                    // remember 
                    // Assemble data object
                    var dict = 
                    {
                        'Temperature': temperature,
                        'Icon': icon
                    };

                    // Send the object
                    Pebble.sendAppMessage(dict, function () {
                        // success
                    }, function (e) {
                        // error sending message
						console.log('/!\\ ERROR sending message ', e.message);
                    });
                } 
            }
        }
    };

    req.send(null);
}


function locationSuccess(pos) {
	console.log('(i) Location success');

	var coordinates = pos.coords;
    fetchWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
    console.warn('/!\\ Location Error (' + err.code + '): ' + err.message);

	Pebble.sendAppMessage({
        'Icon': -1,
        'Temperature': -273
    });
}

var locationOptions = {
    'timeout': 15000,
    'maximumAge': 60000
};

function readyEvent(e)
{
	console.log('(i) Ready Event');

	ready = true;
}

Pebble.addEventListener('ready', readyEvent);

Pebble.addEventListener('appmessage', function (e) {
	console.log('(i) AppMessage Event');

	window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError,
        locationOptions);
});

Pebble.addEventListener('webviewclosed', function (e) {
    console.log('(i) WebviewClosed Event');
});
