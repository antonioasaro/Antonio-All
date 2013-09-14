<?PHP

if (!isset($_GET['location']) || !isset($_GET['units'])) die();

$location = $_GET['location'];
$units = $_GET['units'];

$json  = curl_get('http://api.openweathermap.org/data/2.5/weather?q='.$location.'&units='.$units);

$weather = process_weather($json);
print json_encode($weather);

function curl_get($url){
    if (!function_exists('curl_init')){
        die('Sorry cURL is not installed!');
    }
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_REFERER, $url);
    curl_setopt($ch, CURLOPT_USERAGENT, "Mozilla/1.0");
    curl_setopt($ch, CURLOPT_HEADER, 0);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_TIMEOUT, 10);
    $output = curl_exec($ch);
    curl_close($ch);
    return $output;
}

function process_weather($json_in) {
    $json_output = json_decode(utf8_decode($json_in));
    if (!$json_output) die(); 

    $weather = $json_output->weather;
    $conditions = $weather[0]->icon;
    $temp = $json_output->main->temp;
	
    $result = array();
    $result[1] = $conditions;
    $result[2] = array('I', round($temp, 1) * 10);
    return $result;
}

/*
{   "id":88319,
    "dt":1345284000,
    "name":"Benghazi",
    "coord":{"lat":32.12,"lon":20.07},
    "main":{"temp":306.15,"pressure":1013,"humidity":44,"temp_min":306,"temp_max":306},
    "wind":{"speed":1,"deg":-7},
    "weather":[
                 {"id":520,"main":"Rain","description":"light intensity shower rain","icon":"09d"},
                 {"id":500,"main":"Rain","description":"light rain","icon":"10d"},
                 {"id":701,"main":"Mist","description":"mist","icon":"50d"}
              ],
    "clouds":{"all":90},
    "rain":{"3h":3}}

Day	Night	
01d.png	 01n.png	 sky is clear
02d.png	 02n.png	 few clouds
03d.png	 03n.png	 scattered clouds
04d.png	 04n.png	 broken clouds
09d.png	 09n.png	 shower rain
10d.png	 10n.png	 Rain
11d.png	 11n.png	 Thunderstorm
13d.png	 13n.png	 snow
50d.png	 50n.png	 mist
    
http://bugs.openweathermap.org/projects/api/wiki/Weather_Data
http://bugs.openweathermap.org/projects/api/wiki/Weather_Condition_Codes
*/
?>
