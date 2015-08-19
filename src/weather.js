function getWeather(jsonRoot, hour) {
  var json = jsonRoot.SiteRep.DV.Location.Period[0];
  //log("DATE = " + jsonRoot.SiteRep.DV[1].dataDate);
  //var d = new Date(jsonRoot.SiteRep.DV[1].dataDate);
  //d.setHours(0,0,0,0);
  //var now = new Date();
  //now.setHours(0,0,0,0);
  //log("DATE EQUALITY = " + (d==now));
  // only has one date!!!
  const OFFSET = 90;
  var time = hour * 60;
  log("Looking for timeperiod to match:" + time  + " (" + hour + ":00) on date:" + json.value + " with " + json.Rep.length + " Rep sections.");
  
  var block = json.Rep[0];
  for(var i = 0; i < json.Rep.length; i++) {
    var timestamp = parseInt(json.Rep[i].$);
    if(time > (timestamp + OFFSET) && i < (json.Rep.length-1) ) {
      continue;
    }
    
    block = json.Rep[i];
    return extractWeatherData(block);
  }
  log("NO TIME MATCH FOUND!!!");
  return "";
}

function getWeatherBetween(json, start, end) {
  var now = new Date();
  var day = now.getDay();
  var hour = now.getHours();
  log("Checking for weather between " + start + " and " + end + ". Day:" + day + ", time:" + hour + ":00");
  if(day > 0 && day < 6 ) {
    if(hour < start) {
      return getWeather(json, start);
    }
    if(hour < end) {
      return getWeather(json, hour);
    }
  }
  return null;
}

function getMorningWeather(json){
  return getWeatherBetween(json, 8, 10);
}

function getEveningWeather(json){
  return getWeatherBetween(json, 17, 20);
}

function getCurrentWeather(json){
  var time = ((new Date()).getHours());
  log("Getting current weather");
  return getWeather(json, time);
}
function formatWeatherType(typeCode){
  return WEATHER_TYPES[typeCode];
}
function formatTimestamp(repCode) {
  var hour = repCode / 60;
  var period = hour < 12 ? "am" : "pm";
  return (hour <= 12 ? hour : (hour-12)) + '\n' + period;
}
function formatWindDirection(name) {
  return WIND_DIRECTIONS[name];
}

function extractWeatherData(block){
  var windPointer = formatWindDirection(block.D);
  var typeName = formatWeatherType(block.W);
  var timestamp = formatTimestamp(block.$);
  
  var weatherString = '' + block.F + '\n' + 
    block.S + '\n' + block.G + '\n' +  block.D + '\n' +  windPointer + '\n' +
    block.Pp + "%" + '\n' +
    typeName + '\n' + block.W + '\n' +
    timestamp + '\n';
  
  log('Weather details \n Temp:' + block.F +
      ', Wind Speed/Gust/Direction/Pointer:' + block.S + '/' + block.G + '/' + block.D + '/' + windPointer +
      ', Precipitation:' + block.Pp + "%" + 
      ', WeatherType:' + typeName + "/" + block.W +
      ', Timestamp:' + timestamp);
  return weatherString;
}

function handleMetData(responseText) {
  var json = JSON.parse(responseText);
  var currentWeather = getCurrentWeather(json);
  var morningWeather = getMorningWeather(json);
  var eveningWeather = getEveningWeather(json);
      
  // Assemble dictionary using our keys
  var dictionary = {
    'KEY_WEATHER_CURRENT': currentWeather,
    'KEY_WEATHER_MORNING': morningWeather,
    'KEY_WEATHER_EVENING': eveningWeather
  };

  // Send to Pebble
  Pebble.sendAppMessage(dictionary,
    function(e) {
      log('Weather info sent to Pebble successfully!');
    },
    function(e) {
      log('Error sending weather info to Pebble!');
    }
  );
}      


function getMetData(location)
{
  var url = 'http://datapoint.metoffice.gov.uk/public/data/val/wxfcs/all/json/' + location +
    '?res=3hourly&key=36e36983-8d14-4498-b117-024236b0b96b';
  xhrRequest(url, 'GET', handleMetData);
}

const BRIGHTON = '310037';
const SHOREHAM = '353507';

function getFullWeatherData() {
  getMetData(BRIGHTON);
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    log('PebbleKit JS ready!');
    getFullWeatherData();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    log('AppMessage received!');
    getFullWeatherData();
  }                     
);

//
//
// boring utility things & whatnot
//
//

function pad(digits) {
  if(digits < 10)
    return '0' + digits;
  return digits;
}

function log(message) {
  var date = new Date();
  var formattedDate = date.getUTCDate() + '/' + (date.getUTCMonth() + 1) + ' ' +
    pad(date.getUTCHours()) + ':' + pad(date.getUTCMinutes()) + ':' + pad(date.getUTCSeconds()) + ' - ';
  console.log(formattedDate + message);
}

//var xhrRequest = function (url, type, callback) {
function xhrRequest (url, type, callback) {
  log('Calling webservice at ' + url);
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
}




const WIND_DIRECTIONS = {
  "S": 1,
  "SSW": 2,
  "SW": 3,
  "WSW": 4,
  "W": 5,
  "WNW": 6,
  "NW": 7,
  "NNW": 8,
  "N": 9,
  "NNE": 10,
  "NE": 11,
  "ENE": 12,
  "E": 13,
  "ESE": 14,
  "SE": 15,
  "SSE": 16,
  "": 0
}

const WEATHER_TYPES = {
  0:"Clear night", // clear
  1:"Sunny day", // clear
  2:"Partly cloudy", //\(night)", // partly cloudy
  3:"Partly cloudy", //\(day)", // partly cloudy
  4:"Not used",
  5:"Mist",
  6:"Fog",
  7:"Cloudy", // cloudy
  8:"Overcast", // cloudy
  9:"Light rain shower", //\(night)", // light rain
  10:"Light rain shower", //\(day)", // light rain
  11:"Drizzle", // light rain or drizzle
  12:"Light rain", // light rain
  13:"Heavy rain shower", //\(night)", //heavy rain
  14:"Heavy rain shower", //\(day)", //heavy rain
  15:"Heavy rain", //heavy rain
  16:"Sleet shower", //\(night)", // sleet
  17:"Sleet shower", //\(day)", // sleet
  18:"Sleet", // sleet
  19:"Hail shower", //\(night)", // hail
  20:"Hail shower", //\(day)", // hail
  21:"Hail", // hail
  22:"Light snow shower", //\(night)", // light snow
  23:"Light snow shower", //\(day)", // light snow
  24:"Light snow", // light snow
  25:"Heavy snow shower", //\(night)", // heavy snow
  26:"Heavy snow shower", //\(day)", // heavy snow
  27:"Heavy snow", // heavy snow
  28:"Thunder shower", //\(night)", // thunder
  29:"Thunder shower", //\(day)", // thunder
  30:"Thunder" // thunder
};
